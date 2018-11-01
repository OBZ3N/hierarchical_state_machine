
#include "hsm/state_machine_xml_loader.h"
#include "test/test_generator.h"

struct Settings
{
    Settings();

    bool        m_generate_states;
    int         m_generate_states_options;
    std::string m_generate_states_template_folder;
    std::string m_generate_states_output_folder;

    bool        m_generate_xml;
    std::string m_generate_xml_output_filename;

    std::string m_state_machine_input_xml;

};

Settings::Settings()
{
    m_generate_states = true;
    m_generate_states_options = (test::StateMachineGenerator::Modes::Build);
    m_generate_states_template_folder = "templates\\";
    m_generate_states_output_folder = "source\\test\\states\\";

    m_generate_xml = false;
    m_generate_xml_output_filename = "xmls\\state_machine_generated.xml";
    
    m_state_machine_input_xml = "xmls\\state_machine.xml";
}

void main()
{
    Settings settings;
    
    // the state machine.
    hsm::StateMachineXmlLoader state_machine_xml_loader;

    bool is_loaded = state_machine_xml_loader.load(settings.m_state_machine_input_xml);

    if ( !is_loaded )
    {
        exit( -1 );
    }

    test::StateMachineGenerator state_machine_generator( state_machine_xml_loader.getSchema() );

    if ( settings.m_generate_states )
    {
        state_machine_generator.generateCppFiles(settings.m_generate_states_template_folder, settings.m_generate_states_output_folder, settings.m_generate_states_options);
    }

    if ( settings.m_generate_xml )
    {
        state_machine_xml_loader.generateXml( settings.m_generate_xml_output_filename, state_machine_generator.getSchema() );
    }

    exit( 0 );
}