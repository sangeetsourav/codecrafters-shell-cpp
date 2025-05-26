#include <iostream>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>
#include "builtins.hpp"
#include "util.hpp"

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
		std::vector<std::string> input_tokens = util::tokenize(input);

		// Extract command and arguments
		std::string command;
		std::vector<std::string> args;

		command = input_tokens[0];
		args.insert(args.end(), input_tokens.begin() + 1, input_tokens.end());

		// Check if command is a builtin
		if (builtins::commands.count(command) != 0)
		{
			// Run the builtin command
			builtins::commands[command].second(args);
		}
		// Try to find command in path
		else
		{
			// Search in PATH
			std::string exec_path = util::find_exec_in_path(command);

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
