
#include <array>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <regex>
#include "hsm/debug.h"
#include "test/test_utils.h"
#include "test/test_generator.h"

namespace test
{
    StateMachineGenerator::StateMachineGenerator(const hsm::schema::StateMachine& schema)
        : m_schema(schema)
    {}

    // generate .cpp / .h files from templates.
    // also generate the state factory function in a separate cpp file.
    void StateMachineGenerator::generateCppFiles(const std::string& templates_path, const std::string& generated_path, int modes)
    {
        std::ifstream state_header_file(templates_path + std::string("state.h.template"));
        std::ifstream state_source_file(templates_path + std::string("state.cpp.template"));
        std::ifstream factory_source_file(templates_path + std::string("state_factory_table.cpp.template"));

        if (!state_header_file.is_open() ||
            !state_source_file.is_open() ||
            !factory_source_file.is_open())
        {
            debug::logFatal("failed to load source template files (template_path = '%s').", templates_path.c_str());
            return;
        }

        std::stringstream state_header_file_content;
        state_header_file_content << state_header_file.rdbuf();

        std::stringstream state_source_file_content;
        state_source_file_content << state_source_file.rdbuf();

        std::stringstream factory_source_file_content;
        factory_source_file_content << factory_source_file.rdbuf();

        // add instantiation function to the factory table.
        std::string factory_source_file_str = factory_source_file_content.str();

        std::string time_string = getTimeString();
        std::string xml_filename = m_schema.m_filename;

        for (auto it : m_schema.m_states)
        {
            std::string state_fullname = it.second.m_fullname;
            std::string state_typename = state_fullname;
            std::string state_filename;

            replaceString(state_typename, "\\\\", "");
            replaceString(state_typename, "\\", "_");

            state_filename = std::string("state_") + state_typename.c_str();
            std::transform(state_filename.begin(), state_filename.end(), state_filename.begin(), ::tolower);

            // generate the .h and .cpp files.
            std::string state_header_file_content_str = state_header_file_content.str();
            std::string state_source_file_content_str = state_source_file_content.str();

            // timestamp the files.
            replaceString(state_header_file_content_str, "[#template.xml_input_file]", xml_filename);
            replaceString(state_source_file_content_str, "[#template.xml_input_file]", xml_filename);

            // timestamp the files.
            replaceString(state_header_file_content_str, "[#template.time_and_date]", time_string);
            replaceString(state_source_file_content_str, "[#template.time_and_date]", time_string);

            replaceString(state_header_file_content_str, "[#template.state.type]", state_typename);
            replaceString(state_source_file_content_str, "[#template.state.type]", state_typename);

            //careful printing a string with formattings. Need to 'double up' on the '\'.
            std::string fullname_embedded = state_fullname;
            replaceString(fullname_embedded, "\\", "\\\\");

            replaceString(state_header_file_content_str, "[#template.state.name]", fullname_embedded);
            replaceString(state_source_file_content_str, "[#template.state.name]", fullname_embedded);

            replaceString(state_header_file_content_str, "[#template.state.filename]", state_filename);
            replaceString(state_source_file_content_str, "[#template.state.filename]", state_filename);

            std::string state_header_file_name = state_filename + std::string(".h");
            std::string state_source_file_name = state_filename + std::string(".cpp");

            std::transform(state_header_file_name.begin(), state_header_file_name.end(), state_header_file_name.begin(), ::tolower);
            std::transform(state_source_file_name.begin(), state_source_file_name.end(), state_source_file_name.begin(), ::tolower);

            // backup files first if they exist. Else we'll be potentially doing some damage and overwriting some old code.
            if ((modes & Modes::Backup) != 0)
            {
                backupFile(generated_path + state_header_file_name);
                backupFile(generated_path + state_source_file_name);
            }

            // create header files, if we can.
            if (!fileExists(generated_path + state_header_file_name) || (modes & Modes::Rebuild) != 0)
            {
                std::ofstream state_header_file(generated_path + state_header_file_name, std::ios::out);
                if (!state_header_file.is_open())
                {
                    debug::logFatal("failed to open file '%s' for writing.", state_header_file_name.c_str());
                    return;
                }
                state_header_file << state_header_file_content_str;
            }

            // create source files, if we can.
            if (!fileExists(generated_path + state_source_file_name) || (modes & Modes::Rebuild) != 0)
            {
                std::ofstream state_source_file(generated_path + state_source_file_name, std::ios::out);
                if (!state_source_file.is_open())
                {
                    debug::logFatal("failed to open file '%s' for writing.", state_source_file_name.c_str());
                    return;
                }
                state_source_file << state_source_file_content_str;
            }

            // register state with the factory.
            cloneAndReplaceLineWith(factory_source_file_str, "[#template.state.filename]", state_filename);
            cloneAndReplaceLineWith(factory_source_file_str, "[#template.factory.state.type]", state_typename);

            state_header_file.close();
            state_source_file.close();
        }

        // timestamp the file.
        replaceString(factory_source_file_str, "[#template.xml_input_file]", xml_filename);
        replaceString(factory_source_file_str, "[#template.time_and_date]", time_string);

        // once all done, strip the templated lines.
        stripLinesWith(factory_source_file_str, "[#");

        std::string factory_source_file_name = std::string("state_factory_table.cpp");
        std::transform(factory_source_file_name.begin(), factory_source_file_name.end(), factory_source_file_name.begin(), ::tolower);

        // backup files first if they exist. Else we'll be potentially doing some damage and overwriting some old code.
        if ((modes & Modes::Backup) != 0)
        {
            backupFile(generated_path + factory_source_file_name);
        }

        std::ofstream factory_source_file_out(generated_path + factory_source_file_name, std::ios::out);
        if (!factory_source_file_out.is_open())
        {
            debug::logFatal("failed to open file '%s' for writing.", factory_source_file_name.c_str());
            return;
        }

        factory_source_file_out << factory_source_file_str;
        factory_source_file_out.close();
    }
}
