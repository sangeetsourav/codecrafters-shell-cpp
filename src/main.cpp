#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <cstdlib>
#include <filesystem>

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

		// Split input string based on whitespace of unknown length
		std::istringstream iss(input);
		std::vector<std::string> split_input(std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>());

		// Print newline and continue if input is empty
		if (split_input.size() == 0)
		{
			std::cout << "\n";
			continue;
		}

		// Extract command and arguments
		std::string command;
		std::vector<std::string> args;

		command = split_input[0];
		args.insert(args.end(), split_input.begin() + 1, split_input.end());

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
				if (std::filesystem::exists(inputpath))
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
