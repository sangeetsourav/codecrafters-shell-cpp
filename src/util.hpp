#pragma once
#include <string>
#include <vector>

namespace util {
	/**
	 * @brief Splits the input string into tokens, handling whitespace, single and double quotes, and escape characters.
	 * @param input The string to be tokenized.
	 * @return A vector of strings, each representing a token extracted from the input.
	 */
	std::vector<std::string> tokenize(std::string input);

	/**
	 * @brief Splits a string into a vector of substrings based on a specified delimiter character.
	 * @param s The input string to split.
	 * @param delimiter The character used to separate the substrings.
	 * @return A vector containing the substrings resulting from splitting the input string by the delimiter.
	 */
	std::vector<std::string> split(const std::string& s, char delimiter);

	/**
	 * @brief Searches for an executable file with the specified name in the given list of directories.
	 * @param command The name of the executable file to search for.
	 * @return The full path to the found executable file, or an empty string if not found.
	 */
	std::string find_exec_in_path(std::string command);
}
