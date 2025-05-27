#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include "util.hpp"
#include "builtins.hpp"

namespace util {
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
		for (int i = 0; i < input.size(); i++)
		{
			char c = input[i];

			// Check if single quote has started and it has not been escaped
			if (!in_single_quotes && !in_double_quotes && !escape_next_char && c == '\'')
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
	 * @brief A map used to cache command path searches.
	 */
	std::map<std::string, std::string> exec_cache;

	/**
	 * @brief Searches for an executable file with the specified name in the given list of directories.
	 * @param command The name of the executable file to search for.
	 * @return The full path to the found executable file, or an empty string if not found.
	 */
	std::string find_exec_in_path(std::string command)
	{
		// Reload PATH
		std::string curr_path_var = std::getenv("PATH");

		// Check if path has changed
		bool path_changed = false;
		if (curr_path_var.compare(builtins::path_var) != 0)
		{
			// Update the path and the path directories
			builtins::path_var = curr_path_var;
			builtins::path_dirs = util::split(builtins::path_var, ':');
			path_changed = true;
		}

		// Clear command path cache if path has been updated
		if (path_changed)
		{
			exec_cache.clear();
		}

		// Check the cache first
		if (exec_cache.count(command) != 0)
		{
			return exec_cache[command];
		}

		// Look through the path directories
		std::filesystem::directory_iterator iter_dir;

		for (const auto& dir : builtins::path_dirs)
		{
			// Try to access the directory
			try
			{
				iter_dir = std::filesystem::directory_iterator(dir);
			}
			// Skip if failed
			catch (const std::filesystem::filesystem_error)
			{
				continue;
			}

			// Go through each file
			for (const auto& entry : iter_dir)
			{
				if (command == entry.path().filename())
				{
					// Cache the path for this command for future use
					exec_cache[command] = entry.path().string();

					return exec_cache[command];
				}
			}
		}

		// If execution has reached here then the command has not been found
		exec_cache[command] = "";

		return exec_cache[command];
	}
}