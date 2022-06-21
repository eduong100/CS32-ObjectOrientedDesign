#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

using namespace std;

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct TrieNode
	{
		TrieNode(char a = '?');
		~TrieNode();
		void insert(string& word, int curSpot = 0); 
		bool findWord(string& word, int curSpot = 0);
		void generateRecomendations(string& word, int index, int max, std::vector<std::string>& suggestions);

		vector<TrieNode*>* children;  // Pointer to a vector which points to TrieNodes
		char value;
		bool endOfWord;
		// void destroyTrie(TrieNode* node);
	};
	TrieNode* root;
	
	void strip(string& str);
};

#endif  // STUDENTSPELLCHECK_H_
