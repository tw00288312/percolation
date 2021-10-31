#include "Percolation.h"

using std::vector;

namespace
{
	const int dir[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

	bool isValid(int row, int col, int n)
	{
		return row >= 0 && row < n && col >= 0 && col < n;
	}

	int hash(int row, int col, int n)
	{
		return col + row * n;
	}

	int n = 0; // length of grid
}

Percolation::Percolation(int n) : m_grid(n, vector<bool>(n, false)), m_uf(n*n) 
{
	::n = n; 

	for (int i = 1; i < n; ++i)
	{
		// connect top row
		m_uf.connect(0, i);

		// connect bottom row
		m_uf.connect(hash(n - 1, n - 1, n), hash(n - 1, n - 1, n) - i);
	}
}

void Percolation::open(int row, int col)
{
	if (!isValid(row, col, n*n)) return; // [TODO] throw exception

	if (m_grid[row][col]) return;

	m_grid[row][col] = true;

	// check 4 direction for "flow into"
	for (int i = 0; i < 4; ++i)
	{
		int newRow = row + dir[i][0], newCol = col + dir[i][1];
		if (!isValid(newRow, newCol, n) || !m_grid[newRow][newCol]) continue;

		// flow into i-direction
		m_uf.connect(hash(row, col, n), hash(newRow, newCol, n));
	}
}

bool Percolation::isOpen(int row, int col)
{
	return m_grid[row][col];
}

bool Percolation::isFull(int row, int col)
{
	return isOpen(row, col) && m_uf.find(0) == m_uf.find(hash(row, col, n));
}

bool Percolation::percolate()
{
	return m_uf.find(0) == m_uf.find(hash(n-1, n-1, n));
}
