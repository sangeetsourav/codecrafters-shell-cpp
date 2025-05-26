#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <set>
#include <iterator>
#include <cstdlib>
#include <filesystem>
#include <readline/readline.h>
#include <readline/history.h>

/**
 * @brief Splits the input string into tokens, handling whitespace, single and double quotes, and escape characters.
 * @param input The string to be tokenized.
 * @return A vector of strings, each representing a token extracted from the input.
 */
std::vector<std::string> tokenize(std::string input)
{
	std::vector<std::string> tokens;

	bool in_single_quotes = false;
	bool in_double_quotes = false;
	bool escape_next_char = false;

	std::string token;
	for (int i = 0; i<input.size(); i++)
	{
		char c = input[i];

		// Check if single quote has started and it has not been escaped
		if(!in_single_quotes && !in_double_quotes && !escape_next_char && c == '\'')
		{
			in_single_quotes = true;
		}
		// Check if double quote has started and it has not been escaped
		else if (!in_single_quotes && !in_double_quotes && !escape_next_char && c == '\"')
		{
			in_double_quotes = true;
		}
		// Check if single quote has ended
		else if (in_single_quotes && c == '\'')
		{
			in_single_quotes = false;
		}
		// Check if double quote has ended and that the ending quote has not been escaped
		else if (in_double_quotes && c == '\"' && !escape_next_char)
		{
			in_double_quotes = false;
		}
		// Check if text is neither single-quoted nor double-quoted
		else if (!in_single_quotes && !in_double_quotes)
		{
			// Check if backslash has been encountered
			if (c == '\\')
			{
				escape_next_char = true;
			}
			// Push character after backslash
			else if (escape_next_char)
			{
				token.push_back(c);
				escape_next_char = false;
			}
			// If backslash has not been encountered
			else
			{
				// Keep increasing token size if whitespace is not encountered
				if (c != ' ')
				{
					token.push_back(c);
				}
				else
				{
					// When whitespace is encountered, push token to token list and clear token
					if (!token.empty())
					{
						tokens.push_back(token);
						token.clear();
					}
				}
			}
		}
		// Text is either single-quoted or double-quoted
		else
		{
			if (in_double_quotes)
			{
				// Escape character
				if (escape_next_char)
				{
					token.push_back(c);
					escape_next_char = false;
				}
				// Check if backslash has been encountered
				else if (c == '\\' && !escape_next_char)
				{	

					if (input[i + 1] == '\\' || input[i + 1] == '$' || input[i + 1] == '\"')
					{
						escape_next_char = true;
					}
					else
					{
						// Keep increasing token size
						token.push_back(c);
					}

				}
				else
				{
					// Keep increasing token size
					token.push_back(c);
				}
			}

			if (in_single_quotes)
			{
				// Keep increasing token size
				token.push_back(c);
			}
		}

	}

	// Push last non-empty token into token list
	if (!token.empty())
	{
		tokens.push_back(token);
		token.clear();
	}

	return tokens;
}

/**
 * @brief Splits a string into a vector of substrings based on a specified delimiter character.
 * @param s The input string to split.
 * @param delimiter The character used to separate the substrings.
 * @return A vector containing the substrings resulting from splitting the input string by the delimiter.
 */
std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

/**
 * @brief Searches for an executable file with the specified name in the given list of directories.
 * @param command The name of the executable file to search for.
 * @param path_dirs A vector of directory paths to search within.
 * @return The full path to the found executable file, or an empty string if not found.
 */
std::string find_exec_in_path(std::string command, std::vector <std::string> path_dirs)
{
	for (const auto& dir : path_dirs)
	{
		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			if (command == entry.path().filename())
			{
				return entry.path().string();
			}
		}
	}

	return "";
}

// Get PATH
const char* path_var = std::getenv("PATH");

// Get all directories in PATH
std::vector <std::string> path_dirs = split(path_var, ':');

// Get HOME
const char* home_var = std::getenv("HOME");
std::filesystem::path home_path = home_var;

// Builtin functions declarations
void command_echo(std::vector<std::string>&);
void command_cd(std::vector<std::string>&);
void command_type(std::vector<std::string>&);
void command_pwd(std::vector<std::string>&);
void command_exit(std::vector<std::string>&);

/**
 * @brief A map containing descriptions and function pointers of shell built-in commands.
 */
std::map<std::string, std::pair<std::string,void(*)(std::vector<std::string>&)>> builtins =
{
  {"echo", std::make_pair("echo is a shell builtin", command_echo)},
  {"type", std::make_pair("type is a shell builtin", command_type)},
  {"exit", std::make_pair("exit is a shell builtin", command_exit)},
  {"pwd", std::make_pair("pwd is a shell builtin", command_pwd)},
  {"cd", std::make_pair("cd is a shell builtin", command_cd)}
};

/**
 * @brief Implements an echo command that outputs text or redirects it to a file, supporting standard output and error redirection.
 * @param args A vector of strings representing the command-line arguments, including the text to echo and any redirection operators (e.g., ">", ">>", "1>", "2>", etc.).
 */
void command_echo(std::vector<std::string> &args)
{
	std::string echo_output;
	bool stdout_redirect = false;
	bool stderr_redirect = false;
	bool redirect_append = false;

	std::string redirect_destination;

	for (int i = 0; i < args.size(); i++)
	{
		// Check for stdout redirect
		if (args[i].compare(">") == 0 || args[i].compare("1>") == 0)
		{
			stdout_redirect = true;
			redirect_destination = args[i + 1];
			i++;
		}
		// Check for sterr redirect
		else if (args[i].compare("2>") == 0)
		{
			stderr_redirect = true;
			redirect_destination = args[i + 1];
			i++;
		}
		// Check for stdout redirect with append
		else if (args[i].compare(">>") == 0 || args[i].compare("1>>") == 0)
		{
			stdout_redirect = true;
			redirect_append = true;
			// The next token is the path
			redirect_destination = args[i + 1];
			// Jump to token after the path (if available)
			i++;
		}
		// Check for stderr redirect with append
		else if (args[i].compare("2>>") == 0)
		{
			stderr_redirect = true;
			redirect_append = true;
			redirect_destination = args[i + 1];
			i++;
		}
		else
		{
			// Keep building the echo output
			if (echo_output.empty())
			{
				echo_output.append(args[i]);
			}
			else
			{
				echo_output.append(" ");
				echo_output.append(args[i]);
			}
		}

	}

	// No stdout redirection
	if (!stdout_redirect)
	{
		std::cout << echo_output << "\n";
	}
	// Stdout redirection
	else
	{
		std::ofstream redirect_echo;

		// If append redirect
		if (redirect_append)
		{
			redirect_echo.open(redirect_destination, std::ofstream::app);
		}
		else
		{
			redirect_echo.open(redirect_destination);
		}

		redirect_echo << echo_output << "\n";
		redirect_echo.close();
	}

	// stderr redirection
	if (stderr_redirect)
	{
		std::ofstream redirect_echo;

		// if append redirect
		if (redirect_append)
		{
			redirect_echo.open(redirect_destination, std::ofstream::app);
		}
		else
		{
			redirect_echo.open(redirect_destination);
		}

		redirect_echo.close();
	}
}

/**
 * @brief Changes the current working directory based on the provided path argument.
 * @param args A vector of strings containing the arguments for the 'cd' command. The first element should be the target directory path.
 */
void command_cd(std::vector<std::string> &args)
{
	std::filesystem::path inputpath = args[0];

	if (args[0] == "~")
	{
		std::filesystem::current_path(home_path);
	}
	// Check if path exists
	else if (std::filesystem::exists(inputpath))
	{
		// Change current working directory, inputpath can be relative also
		std::filesystem::current_path(inputpath);
	}
	else
	{
		std::cout << "cd: " << args[0] << ": No such file or directory\n";
	}
}

/**
 * @brief Prints the type or location of each command specified in the argument list.
 * @param args A vector of command names to look up.
 */
void command_type(std::vector<std::string> &args)
{
	// Find command
	for (const auto& arg : args)
	{
		// First check builtins
		if (builtins.count(arg) != 0)
		{
			std::cout << builtins[arg].first << "\n";
		}
		else
		{
			// Search in PATH
			std::string exec_path = find_exec_in_path(arg, path_dirs);

			if (!exec_path.empty())
			{
				std::cout << arg << " is " << exec_path << "\n";
			}
			else
			{
				std::cout << arg << ": not found" << "\n";
			}
		}
	}
}

/**
 * @brief Exits the shell with the provided exit code.
 * @param args A vector of strings where the first element is the exit code as a string.
 */
void command_exit(std::vector<std::string> &args)
{
	// Call the exit function with the integer present in the first argument
	exit(std::stoi(args[0]));
}

/**
 * @brief Prints the current working directory to standard output.
 * @param args A vector of strings containing the arguments for the 'pwd' command (unused).
 */
void command_pwd(std::vector<std::string> &args)
{
	// Print the current path
	std::cout << std::filesystem::current_path().string() << "\n";
}

int main() {
	// Flush after every std::cout / std:cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	while (true)
	{

		std::string prompt = "$ ";
		std::string input;

		// Use readline to get input
		input = readline(prompt.c_str());

		// Print newline and continue if input is empty
		if (input.empty())
		{
			std::cout << "\n";
			continue;
		}
		else
		{	
			// Add command to history
			add_history(input.c_str());
		}

		// Tokenize the input
		std::vector<std::string> input_tokens = tokenize(input);

		// Extract command and arguments
		std::string command;
		std::vector<std::string> args;

		command = input_tokens[0];
		args.insert(args.end(), input_tokens.begin() + 1, input_tokens.end());

		// Check if command is a builtin
		if (builtins.count(command) != 0)
		{
			// Run the builtin command
			builtins[command].second(args);
		}
		// Try to find command in path
		else
		{
			// Search in PATH
			std::string exec_path = find_exec_in_path(command, path_dirs);

			if (!exec_path.empty())
			{
				std::system(input.c_str());
			}
			else
			{
				std::cout << command << ": command not found\n";
			}
		}
	}
}
