#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <fstream>

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::TrieNode::TrieNode(char a)
{
	value = a;
	endOfWord = false;
	children = new vector<TrieNode*>(27, nullptr);
}

StudentSpellCheck::TrieNode::~TrieNode()
{
	for (int i = 0; i < 27; i++)
	{
		if ((*children)[i] != nullptr)
		{
			delete (*children)[i];
		}
	}
	delete children;
}

// Recursively insert word into our dictionary trie
// O(L) where L is the length of the word
void StudentSpellCheck::TrieNode::insert(string& word, int curSpot)
{
	// If word is empty, then do nothing
	if (curSpot >= word.length())
	{
		endOfWord = true;
		return;
	}

	// Add first character as node to the trie (if such a node does not already exist)
	int index;
	char letter = tolower(word[curSpot]);
	if (letter != '\'')
	{
		index = letter - 'a';
	}
	else index = 26;

	// Add the rest of the word to the trie
	if ((*children)[index] == nullptr)
	{
		(*children)[index] = new TrieNode(letter);
	}
	(*children)[index]->insert(word, curSpot + 1);
}

// Recursively find word in our dictionary trie
// O(L) where L is the length of the word
bool StudentSpellCheck::TrieNode::findWord(string& word, int curSpot)
{
	if (curSpot >= word.size())
		return endOfWord;
	int index;
	char letter = tolower(word[curSpot]);
	if (letter != '\'')
	{
		index = letter - 'a';
	}
	else index = 26;

	if ((*children)[index] == nullptr)
		return false;
	return (*children)[index]->findWord(word, curSpot + 1);
}

StudentSpellCheck::StudentSpellCheck()
{
	root = new TrieNode();
}

StudentSpellCheck::~StudentSpellCheck() {
	delete root;
}

// Strip string of invalid characters
// O(L^2) where L is string length (string length will be strictly bounded for our use)
void StudentSpellCheck::strip(string& str)
{
	int i = 0;
	while (i < str.size())
	{
		int l = tolower(str[i]);
		if (l == '\'' || (l >= 'a' && l <= 'z'))
		{
			i++;
		}
		else
			str.erase(i,1);
	}
}

// Load dictionary into trie
// O(N) where N is number of lines in dictionary
bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile)
		return false;

	string tmp;
	while (getline(infile, tmp))  // O(N) where N is no. of lines
	{
		strip(tmp); // O(1) since size of line is bounded
		root->insert(tmp); // O(1) for same reason as above
	}
	return true;
}

// Generate suggestions up to a cap
// O(L^2 + max) time where L is size of word
void StudentSpellCheck::TrieNode::generateRecomendations(string& word, int index, int max, std::vector<std::string>& suggestions)
{
	if (suggestions.size() >= max || index >= word.size())
		return;
	string latter = word.substr(index + 1);
	// For every possible letter at current index
	for (int i = 0; i < 27; i++)  // O(L)
	{
		if (suggestions.size() >= max)
			return;
		// If letter + latter exist as a word
		if ((*children)[i] != nullptr && (*children)[i]->findWord(latter))   // O(L)
		{
			char c;
			if (i != 26)
				c = 'a' + i;
			else
				c = '\'';
			// Then the entire word is recommendable
			string rec = word.substr(0, index) + c + latter;   // O(L)
			suggestions.push_back(rec);
		}
	}
	char c = tolower(word[index]);
	int i;
	if (c == '\'')
		i = 26;
	else
		i = c - 'a';
	if((*children)[i] != nullptr)
		(*children)[i]->generateRecomendations(word, index + 1, max, suggestions);
}

// Check spelling and make suggestions if word is misspelled
// O(L^2 + max) time where L is size of word
bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	if (root->findWord(word))
		return true;
	string lower = "";
	for (int i = 0; i < word.size(); i++)  // O(L^2)
	{
		lower += tolower(word[i]);
	}
	suggestions.clear();
	root->generateRecomendations(lower, 0, max_suggestions, suggestions); // O(L^2 + max)
	return false;
}

// Store positions of all mispelled words in string line
void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	int i = 0;
	problems.clear();
	// For each character in the given line
	while (i < line.size())
	{
		int start, end;
		char l = tolower(line[i]);
		// If we find the start of a word
		if (l == '\'' || (l >= 'a' && l <= 'z'))
		{
			start = i;
			// Iterate until the end of the word
			while (i < line.size() && (l == '\'' || (l >= 'a' && l <= 'z')))
			{
				i++;
				l = tolower(line[i]);
			}
			// Build the word with the start/end positions
			end = i - 1;
			string str = line.substr(start, end - start + 1);
			// If word is not in dictionary
			if (!root->findWord(str))
			{
				// Add the position to the problems vector
				Position p;
				p.start = start;
				p.end = end;
				problems.push_back(p);
			}
		}
		else
		{
			i++;
		}
	}
}
