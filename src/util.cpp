#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <cstring>
#include <unistd.h>
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
	 * @brief Searches for an executable file with the specified name in the given list of directories.
	 * @param command The name of the executable file to search for.
	 * @return The full path to the found executable file, or an empty string if not found.
	 */
	std::string find_exec_in_path(std::string command)
	{
		// Reload PATH
		builtins::path_var = std::getenv("PATH");
		builtins::path_dirs = util::split(builtins::path_var, ':');

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
				if (!access(entry.path().c_str(), X_OK))
				{
					if (command == entry.path().filename())
					{
						// Return the path
						return entry.path().string();
					}
				}
			}
		}

		return "";
	}

	void Trie::insert(std::string& word)
	{
		TrieNode* curr = &root;

		for (auto c : word)
		{
			if (curr->children.count(c) == 0)
			{
				curr->children[c] = new TrieNode(false);
			}

			curr = curr->children[c];
		}

		curr->end_of_word = true;
	}

	bool Trie::search(std::string& word)
	{
		TrieNode* curr = &root;

		for (auto c : word)
		{
			if (curr->children.count(c) == 0)
			{
				return false;
			}

			curr = curr->children[c];
		}

		return curr->end_of_word;
	}

	std::vector<std::string> Trie::matches(std::string partial_word)
	{
		TrieNode* curr = &root;
		std::vector<std::string> matches;

		for (auto c : partial_word)
		{
			if (curr->children.count(c) != 0)
			{
				curr = curr->children[c];
			}
		}

		if (curr->is_root)
		{
			return matches;
		}
		else
		{
			return go_deep(curr, partial_word);
		}
	}

	std::vector<std::string> Trie::go_deep(TrieNode* node, std::string word)
	{
		std::vector<std::string> words;

		if (node->children.empty())
		{
			words.push_back(word);
			return words;
		}
		else
		{
			for (const auto& kv : node->children)
			{
				std::string new_word = word;
				new_word.push_back(kv.first);

				if (kv.second->end_of_word)
				{
					words.push_back(new_word);
				}

				std::vector<std::string> child_words = go_deep(node->children[kv.first], new_word);

				words.insert(words.end(), child_words.begin(), child_words.end());
			}

		}

		return words;
	}

	void Trie_Commands::update_from_path()
	{
		// Reload PATH
		builtins::path_var = std::getenv("PATH");
		builtins::path_dirs = util::split(builtins::path_var, ':');

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
				std::string fname = entry.path().filename().string();
				insert(fname);
			}
		}

	}

	Trie_Commands commands_all;

	/**
	 * @brief Generates possible completions for a given input text.
	 * @param text The partial text for which to generate a completion.
	 * @param state It is zero the first time the function is called, allowing the generator to perform any necessary initialization, and a positive non-zero integer for each subsequent call.
	 * @return A pointer to a string containing the next possible completion, or NULL if there are no more completions.
	 */
	char* completion_generator(const char* text, int state)
	{
		static std::vector<std::string> matches;
		static int match_index;

		if (state==0)
		{
			commands_all.update_from_path();
			matches = util::commands_all.matches(text);
			match_index = 0;
		}

		char* match = NULL;

		if (match_index < matches.size())
		{	
			// Use strdup to allocate space on the heap. (It copies the string and the calls malloc)
			// If it is allocated on the stack then the pointer would be invalid when this function goes out of scope.
			match = strdup(matches[match_index].c_str());
			match_index++;
		}

		return match;
	}
}