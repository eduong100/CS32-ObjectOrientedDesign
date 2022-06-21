#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

// Add last action to the top of stack
// O(1) time on average; Up to O(N) time where N is current line (only when deleting)
void StudentUndo::submit(const Action action, int row, int col, char ch) {   
	if (action == Action::DELETE)
	{
		// If stack is not empty and the top is a delete near where we just deleted 
		if (!actionStack.empty() && actionStack.top()[0] == Action::DELETE
			&& (actionStack.top()[2] - col == 1 || actionStack.top()[2] - col == 0))
		{
			// If the most recent delete was backspace()
			if (actionStack.top()[2] - col == 1)
			{
				// Add the backspaced character to front of word
				actionStack.top()[2] = col;
				wordStack.top() = ch + wordStack.top();
			}
			// If the most recent delete was del()
			else
			{
				// Add the deleted character to back of word
				wordStack.top() += ch;
			}
			return;
		}
		// Otherwise, push this new delete's string onto the wordStack
		else
		{
			string tmp = "";
			tmp += ch;
			wordStack.push(tmp);
		}
	}

	// Push action into the actionStack
	vector<int> tmp(3);
	tmp[0] = action;
	tmp[1] = row;
	tmp[2] = col;
	actionStack.push(tmp);
}

// Fetch data needed to undo the most recent action
// O(1) time on average. O(N) time if undoing insertions where N is number of insertions needed to undo
StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {

	if (actionStack.empty())
		return Action::ERROR;
	row = actionStack.top()[1];
	col = actionStack.top()[2];
	int topAction = actionStack.top()[0];
	text = "";
	actionStack.pop();
	count = 1;

	if (topAction == Action::DELETE)
	{
		text = wordStack.top();
		wordStack.pop();
		return Action::INSERT;
	}
	if (topAction == Action::INSERT)
	{
		while (!actionStack.empty() && actionStack.top()[0] == Action::INSERT)
		{
			int colTmp = actionStack.top()[2];
			int rowTmp = actionStack.top()[1];
			if (colTmp != col - count || rowTmp != row)
				break;
			count++;
			actionStack.pop();
		}
		return Action::DELETE;
	}
	if (topAction == Action::JOIN)
	{
		return Action::SPLIT;
	}
	if (topAction == Action::SPLIT)
	{
		return Action::JOIN;
	}
    return Action::ERROR;  
}

// Clear both stacks
// O(N) time where N is size of actionStack since wordStack.size() <= actionStack.size()
void StudentUndo::clear() {                               
	while (!actionStack.empty())
		actionStack.pop();
	while (!wordStack.empty())
		wordStack.pop();
}
