#include "builtins.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <iterator>
#include <cstdlib>
#include <filesystem>
#include "util.hpp"

namespace builtins {
	// Get PATH
	std::string path_var = std::getenv("PATH");

	// Get all directories in PATH
	std::vector <std::string> path_dirs = util::split(path_var, ':');

	// Get HOME
	std::string home_var = std::getenv("HOME");
	std::filesystem::path home_path = home_var;

	/**
	 * @brief A map containing descriptions and function pointers of shell built-in commands.
	 */
	std::map<std::string, std::pair<std::string, void(*)(std::vector<std::string>&)>> commands =
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
	void command_echo(std::vector<std::string>& args)
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
	void command_cd(std::vector<std::string>& args)
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
	void command_type(std::vector<std::string>& args)
	{
		// Find command
		for (const auto& arg : args)
		{
			// First check builtins
			if (commands.count(arg) != 0)
			{
				std::cout << commands[arg].first << "\n";
			}
			else
			{
				// Search in PATH
				std::string exec_path = util::find_exec_in_path(arg);

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
	void command_exit(std::vector<std::string>& args)
	{
		// Call the exit function with the integer present in the first argument
		exit(std::stoi(args[0]));
	}

	/**
	 * @brief Prints the current working directory to standard output.
	 * @param args A vector of strings containing the arguments for the 'pwd' command (unused).
	 */
	void command_pwd(std::vector<std::string>& args)
	{
		// Print the current path
		std::cout << std::filesystem::current_path().string() << "\n";
	}
}