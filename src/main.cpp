#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <cstdlib>
#include <filesystem>

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

int main() {
	// Flush after every std::cout / std:cerr
	std::cout << std::unitbuf;
	std::cerr << std::unitbuf;

	// Get PATH
	const char* path_var = std::getenv("PATH");
	// Get all directories in PATH
	std::vector <std::string> path_dirs = split(path_var, ':');

	// Get HOME
	const char* home_var = std::getenv("HOME");
	std::filesystem::path home_path = home_var;

	std::map<std::string, std::string> builtins =
	{
	  {"echo", "echo is a shell builtin"},
	  {"type", "type is a shell builtin"},
	  {"exit", "exit is a shell builtin"},
	  {"pwd", "pwd is a shell builtin"},
	  {"cd", "cd is a shell builtin"}
	};

	while (true)
	{

		std::cout << "$ ";
		std::string input;
		std::getline(std::cin, input);

		// Tokenize the input
		std::vector<std::string> input_tokens = tokenize(input);

		// Print newline and continue if input is empty
		if (input_tokens.size() == 0)
		{
			std::cout << "\n";
			continue;
		}

		// Extract command and arguments
		std::string command;
		std::vector<std::string> args;

		command = input_tokens[0];
		args.insert(args.end(), input_tokens.begin() + 1, input_tokens.end());

		// Check if command is a builtin
		if (builtins.count(command) != 0)
		{
			// exit
			if (command == "exit")
			{
				exit(std::stoi(args[0]));
			}
			// pwd
			else if (command == "pwd")
			{
				std::cout<<std::filesystem::current_path().string() << "\n";
			}
			//cd
			else if (command == "cd")
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
			// echo
			else if (command == "echo")
			{
				for (const auto& arg : args)
				{
					std::cout << arg << " ";
				}
				std::cout << "\n";
			}
			// type
			else if (command == "type")
			{
				// Find command
				for (const auto& arg : args)
				{
					// First check builtins
					if (builtins.count(arg) != 0)
					{
						std::cout << builtins[arg] << "\n";
					}
					else
					{
						// Search in PATH
						std::string exec_path = find_exec_in_path(arg, path_dirs);

						if(!exec_path.empty())
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
