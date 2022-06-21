#include "History.h"
using namespace std;

History::History(int nRows, int nCols)
{
    m_rows = nRows;
    m_cols = nCols;
    for(int i = 0; i < nRows; i++)
    {
        for(int j = 0; j < nCols; j++)
        {
            grid[i][j] = '.';
        }
    }
}

bool History::record(int r, int c)
{
    if(r < 1 || r > m_rows || c < 1 || c > m_cols)
    {
        return false;
    }
    if(grid[r-1][c-1] == '.')
    {
        grid[r-1][c-1] = 'A';
    }
    else if(grid[r-1][c-1] >= 65 && grid[r-1][c-1] <= 89)
    {
        grid[r-1][c-1] += 1;
    }
    else if(grid[r-1][c-1] == 90)
    {
        grid[r-1][c-1] = 'Z';
    }
    return true;
}

void History::display() const
{
    clearScreen();
    for (int r = 0; r < m_rows; r++)
    {
        for (int c = 0; c < m_cols; c++)
            cout << grid[r][c];
        cout << endl;
    }
    cout << endl;
}