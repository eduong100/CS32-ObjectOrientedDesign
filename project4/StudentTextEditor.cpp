#include "StudentTextEditor.h"
#include "Undo.h"   

#include <string>
#include <vector>
#include <fstream>

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

// Initialize list data structure (linked list of strings) and member variables
// O(1) time
StudentTextEditor::StudentTextEditor(Undo* undo)  
 : TextEditor(undo) {
	text = new list<string>;
	text->push_back("");
	curRow = text->begin();

	colIndex = 0;
	rowIndex = 0;
	lineCount = 1;
}

// Destructor: Delete dynamically allocated list
// O(N) time where N is total number of lines (nodes) in text
StudentTextEditor::~StudentTextEditor()
{
	text->clear();
	delete text;
}

// Open file and load text into data structure
// O(M+N) where M is line number of old text and N is line number of new text.
bool StudentTextEditor::load(std::string file) { 
	// Atempt to open file
	ifstream infile(file);
	if (!infile)
		return false;
	// Empty linked list
	reset();
	// Use getline() to load line into string; Then load string into linked list
	string tmp;
	while (getline(infile, tmp))   
	{
		text->push_back(tmp);
		string* tmp = &text->back();
		size_t len = tmp->length();
		if (len > 0 && (*tmp)[len - 1] == '\r')
			tmp->erase(len - 1);
		lineCount++;
	}
	curRow = text->begin();
	return true;
}

// Save text into new file
// O(M) time where M is number of lines currently being edited
bool StudentTextEditor::save(std::string file) {  
	ofstream outfile(file);
	if (!outfile)
		return false;  
	list<string>::iterator it = text->begin();
	while (it != text->end())
	{
		outfile << *it << '\n';
		it++;
	}
	return true;
}

// Clear linked list data structure for future use
// O(N) where N is the total number of lines
void StudentTextEditor::reset() {		 
	text->clear();
	rowIndex = 0;
	colIndex = 0;
	lineCount = 0;
}

// Move cursor in a specific direction
// O(1) time
void StudentTextEditor::move(Dir dir) {   
	switch (dir)
	{
	case Dir::UP:
		if (rowIndex > 0)
		{
			curRow--;
			rowIndex--;
			colIndex = min((int)curRow->size(), colIndex);
		}
		break;
	case Dir::DOWN:
		if (rowIndex < lineCount - 1)
		{
			curRow++;
			rowIndex++;
			colIndex = min((int)curRow->size(), colIndex);
		}
		break;
	case Dir::LEFT:
		if (colIndex > 0)
			colIndex--;
		break;
	case Dir::RIGHT:
		if (colIndex < curRow->size())
			colIndex++;
		break;
	case Dir::HOME:
		colIndex = 0;
		break;
	case Dir::END:
		colIndex = curRow->size();
		break;
	}
}

// Either delete a character or move next line up
// O(L) for deletion in middle... O(L1+L2) for deletion at end
void StudentTextEditor::del() {  
	// If we are at the end of the current row and there is a line beneath us
	if (colIndex >= curRow->size() && rowIndex < lineCount - 1)
	{
		// Move previous line up
		getUndo()->submit(Undo::Action::JOIN, rowIndex, colIndex);
		list<string>::iterator tmp = curRow;
		tmp++;
		*curRow += *tmp;
		text->erase(tmp);
		lineCount--;
	}
	// Otherwise delete highlighted character
	else
	{
		getUndo()->submit(Undo::Action::DELETE, rowIndex, colIndex, (*curRow)[colIndex]);
		curRow->erase(colIndex, 1);
	}
}

// Either delete preceding character or move current line up
// O(L) for backspace in middle... O(L1+L2) for backspace at end
void StudentTextEditor::backspace() {
	// If we backspace at the start of a line
	if (colIndex == 0)
	{
		// and we are not at the first line
		if (rowIndex > 0)
		{
			// Move current line up
			list<string>::iterator tmp = curRow;
			curRow--;
			rowIndex--;
			colIndex = curRow->size();
			*curRow += *tmp;     // O(L1 + L2)
			text->erase(tmp);    // O(1)
			lineCount--;
			getUndo()->submit(Undo::Action::JOIN, rowIndex, colIndex);
		}
	}
	// Otherwise delete preceding character
	else
	{
		colIndex--;
		getUndo()->submit(Undo::Action::DELETE, rowIndex, colIndex, (*curRow)[colIndex]);
		curRow->erase(colIndex, 1);   // O(L)
	}

}

// Insert new character at cursor position
// O(L) where L is length of current line
void StudentTextEditor::insert(char ch) {    
	string tmp;
	int i = 1;
	if (ch == '\t') {
		tmp = "    ";
		i = 4;
	}
	else {
		tmp = ch;
	}
	curRow->insert(colIndex, tmp);  // O(L)
	colIndex += i;
	getUndo()->submit(Undo::Action::INSERT, rowIndex, colIndex, ch);
}

// Insert new line below; Move everything in current line passed the cursor to the next line
// O(L) where L is length of current line
void StudentTextEditor::enter() {        
	getUndo()->submit(Undo::Action::SPLIT, rowIndex, colIndex);

	list<string>::iterator tmp = curRow;
	tmp++;

	// Get text to be inserted below
	string toBeInserted = curRow->substr(colIndex);   // O(L)
	// Remove said text from the current line and insert it below
	curRow->erase(colIndex);						  // O(L)
	text->insert(tmp, toBeInserted);				  // O(1)

	curRow++;
	rowIndex++;
	lineCount++;
	colIndex = 0;
}

// Fetch current cursor coordinates
// O(1) time
void StudentTextEditor::getPos(int& row, int& col) const {   
	row = rowIndex;
	col = colIndex;
}

// Fill vector with designated lines from the text
int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {  
	if (startRow < 0 || numRows < 0 || startRow >= lineCount)
		return -1;

	// Find starting position
	list<string>::iterator it = curRow;
	int rowIt = rowIndex;
	while (rowIt < startRow)
	{
		it++;
		rowIt++;
	}
	while (rowIt > startRow)
	{
		it--;
		rowIt--;
	}

	// For specified number of rows, copy strings into lines
	lines.clear();
	for (int i = 0; i < numRows && it != text->end(); i++, it++)
	{
		lines.push_back(*it);
	}
	return lines.size();
}

// Undo last action
void StudentTextEditor::undo() {
	Undo::Action action;
	int row, col, count;
	string toInsert;
	action = getUndo()->get(row, col, count, toInsert);
	if (action == Undo::Action::ERROR)
		return;

	moveTo(row, col);
	if (action == Undo::Action::JOIN)
	{
		list<string>::iterator tmp = curRow;
		tmp++;
		*curRow += *tmp;
		text->erase(tmp);
		lineCount--;
	}
	
	else if (action == Undo::Action::DELETE)
	{
		for (int i = 0; i < count; i++)
		{
			colIndex--;
			curRow->erase(colIndex, 1);
		}
	}

	else if (action == Undo::Action::INSERT)
	{
		curRow->insert(colIndex, toInsert);
	}

	else if (action == Undo::Action::SPLIT)
	{
		list<string>::iterator tmp = curRow;
		tmp++;

		string toBeInserted = curRow->substr(colIndex);   
		curRow->erase(colIndex);						  
		text->insert(tmp, toBeInserted);				  
		lineCount++;
	}

}

// Move cursor to given coordiante
// O(abs(currentRow - row) + abs(currentCol - col))
void StudentTextEditor::moveTo(int row, int col)
{
	if (row < 0 || row >= lineCount)
	{
		return;
	}
	colIndex = col;
	if (rowIndex == row)
		return;
	if (rowIndex > row)
	{
		while (rowIndex > row)
		{
			rowIndex--;
			curRow--;
		}
	}
	else if (rowIndex < row)
	{
		while (rowIndex < row)
		{
			rowIndex++;
			curRow++;
		}
	}
}