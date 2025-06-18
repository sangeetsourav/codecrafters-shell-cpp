#pragma once
#include <string>
#include <vector>
#include <map>

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

	/**
	 * @brief Generates possible completions for a given input text.
	 * @param text The partial text for which to generate a completion.
	 * @param state It is zero the first time the function is called, allowing the generator to perform any necessary initialization, and a positive non-zero integer for each subsequent call.
	 * @return A pointer to a string containing the next possible completion, or NULL if there are no more completions.
	 */
	char* completion_generator(const char* text, int state);

	class TrieNode
	{
	public:
		std::map<char, TrieNode*> children;
		bool end_of_word = false;
		bool is_root = false;

		TrieNode(bool root)
		{
			is_root = root;
		}
	};

	class Trie
	{
	public:
		TrieNode root{ true };

		void insert(std::string &word);
		bool search(std::string& word);
		std::vector<std::string> go_deep(TrieNode* node, std::string word);
		std::vector<std::string> matches(std::string partial_word);
	};

	class Trie_Builtins : public Trie
	{
	public:
		Trie_Builtins(std::map<std::string, std::pair<std::string, void(*)(std::vector<std::string>&)>> commands)
		{
			for (const auto &kv: commands)
			{
				std::string word = kv.first;
				insert(word);
			}
		}
	};

	extern Trie_Builtins trie_builtin;
}
