#ifndef HISTORY
#define HISTORY

#include "globals.h"

class History
{
    public:
        History(int nRows, int nCols);
        bool record(int r, int c);
        void display() const;
    
    private:
        int m_rows;
        int m_cols;
        char grid[MAXROWS][MAXCOLS];
};

#endif