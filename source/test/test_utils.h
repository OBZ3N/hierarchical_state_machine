
#pragma once

#include <string>

// replace substrings within a string
extern void replaceString(std::string& source_string, const std::string old_substring, const std::string new_substring);

// - find line with matching substring.
// - make a copy of that line, with tags and all.
// - replace old line with new line with new substring.
// - paste the copy of the line with the tag, so we can re-use it later.
extern void cloneAndReplaceLineWith(std::string& source_string, const std::string& old_substring, const std::string& new_substring);

// check if a file exists.
extern bool fileExists(const std::string& name);

// make a copy of a file, with and added extention '.bak', or .'bak002'.
// presumably before it gets overwritten.
extern void backupFile(const std::string& name);

// remove lines containing a substring.
extern void stripLinesWith(std::string& source_string, const std::string& old_substring);

// current time, for time stamping the generated files.
extern std::string getTimeString();
