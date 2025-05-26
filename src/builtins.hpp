#pragma once
#include <string>
#include <vector>
#include <map>
#include <filesystem>

// PATH
extern const char* path_var;

// All directories in PATH
extern std::vector <std::string> path_dirs;

// HOME
extern const char* home_var;
extern std::filesystem::path home_path;

/**
 * @brief Implements an echo command that outputs text or redirects it to a file, supporting standard output and error redirection.
 * @param args A vector of strings representing the command-line arguments, including the text to echo and any redirection operators (e.g., ">", ">>", "1>", "2>", etc.).
 */
void command_echo(std::vector<std::string>& args);

/**
 * @brief Prints the type or location of each command specified in the argument list.
 * @param args A vector of command names to look up.
 */
void command_type(std::vector<std::string>& args);

/**
 * @brief Changes the current working directory based on the provided path argument.
 * @param args A vector of strings containing the arguments for the 'cd' command. The first element should be the target directory path.
 */
void command_cd(std::vector<std::string>& args);

/**
 * @brief Exits the shell with the provided exit code.
 * @param args A vector of strings where the first element is the exit code as a string.
 */
void command_exit(std::vector<std::string>& args);
/**
 * @brief Prints the current working directory to standard output.
 * @param args A vector of strings containing the arguments for the 'pwd' command (unused).
 */
void command_pwd(std::vector<std::string>& args);

/**
 * @brief A map containing descriptions and function pointers of shell built-in commands.
 */
extern std::map<std::string, std::pair<std::string, void(*)(std::vector<std::string>&)>> builtins;