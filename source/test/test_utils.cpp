
#include <array>
#include <ctime>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "test/test_utils.h"

// replace a substring with another substring, recursively.
void replaceString(std::string& source_string, const std::string old_substring, const std::string new_substring)
{
    // Can't replace nothing.
    if (old_substring.empty())
        return;

    // Find the first occurrence of the substring we want to replace.
    size_t substring_position = source_string.find(old_substring);

    // If not found, there is nothing to replace.
    if (substring_position == std::string::npos)
        return;

    // right side of the replaced string.
    size_t right_start = substring_position + old_substring.length();
    size_t right_size = source_string.length() - right_start;
    std::string right_side = source_string.substr(right_start, right_size);

    // replace string on the right side.
    replaceString(right_side, old_substring, new_substring);

    // left side of the string.
    std::string left_side = source_string.substr(0, substring_position);

    // concatenate left and right side, with the replacement string in the middle.
    source_string = left_side + new_substring + right_side;
}

// - find line with matching substring.
// - make a copy of that line, with tags and all.
// - replace old line with new line with new substring.
// - paste the copy of the line with the tag, so we can re-use it later.
void cloneAndReplaceLineWith(std::string& source_string, const std::string& old_substring, const std::string& new_substring)
{
    // Can't replace nothing.
    if (old_substring.empty())
        return;

    // Find the first occurrence of the substring we want to replace.
    size_t substring_position = source_string.find(old_substring);

    // If not found, there is nothing to replace.
    if (substring_position == std::string::npos)
        return;

    size_t line_start_position = source_string.rfind("\n", substring_position);
    size_t line_end_position = source_string.find("\n", substring_position);

    if (line_start_position == std::string::npos ||
        line_end_position == std::string::npos ||
        line_end_position <= line_start_position)
        return;

    // line containing the string.
    std::string line_to_clone = source_string.substr(line_start_position, (line_end_position - line_start_position));

    std::string line_added = line_to_clone;
    replaceString(line_added, old_substring, new_substring);

    source_string.insert(line_end_position, line_added);
}

// check if a file exists.
bool fileExists(const std::string& name)
{
    std::ifstream f(name.c_str());
    return f.good();
}

// make a copy of a file, with and added extention '.bak', or .'bak002'.
// presumably before it gets overwritten.
void backupFile(const std::string& name)
{
    // check if the file needs a backup.
    if (fileExists(name))
    {
        // find a valid filename for backup.
        std::ostringstream backup_extention;
        backup_extention << ".bak";

        std::string backup_name = name + backup_extention.str();

        int backup_index = 0;

        // check if backup exists with that filename.
        while (fileExists(backup_name))
        {
            backup_index++;

            std::ostringstream  backup_extention;

            backup_extention << ".bak" << std::setfill('0') << std::setw(3) << backup_index;

            backup_name = name + backup_extention.str();
        }

        // read file content.
        std::ifstream file_to_backup(name.c_str());
        std::stringstream file_content;
        file_content << file_to_backup.rdbuf();

        // write file content to backup.
        std::ofstream file_backup(backup_name.c_str(), std::ios::out);
        file_backup << file_content.str();
        file_backup.close();
    }
}

// remove lines containing a substring.
void stripLinesWith(std::string& source_string, const std::string& old_substring)
{
    // Can't replace nothing.
    if (old_substring.empty())
        return;

    // Find the first occurrence of the substring we want to replace.
    size_t substring_position = source_string.find(old_substring);

    // If not found, there is nothing to replace.
    if (substring_position == std::string::npos)
        return;


    size_t line_start_position = source_string.rfind("\n", substring_position);
    size_t line_end_position = source_string.find("\n", substring_position);

    if (line_start_position == std::string::npos ||
        line_end_position == std::string::npos ||
        line_end_position <= line_start_position)
        return;

    std::string left_side = source_string.substr(0, line_start_position);

    // right side, and strip it as well.
    std::string right_side = source_string.substr(line_end_position, std::string::npos);
    stripLinesWith(right_side, old_substring);

    // final source string.
    source_string = left_side + right_side;
}

// current time, for time stamping the generated files.
std::string getTimeString()
{
    std::array<char, 128> buffer;
    buffer.fill(0);

    std::time_t rawtime;
    std::time(&rawtime);
    const auto timeinfo = std::localtime(&rawtime);

    std::strftime(buffer.data(), sizeof(buffer), "%A %d %B %Y, %H:%M:%S", timeinfo);

    std::string time_string(buffer.data());

    return time_string;
}
