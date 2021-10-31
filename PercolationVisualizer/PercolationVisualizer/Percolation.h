#pragma once

#include "WeightQuickUnionUF.h"
#include <vector>

class Percolation
{
public:
	Percolation(int n);

	void open(int row, int col);

	bool isOpen(int row, int col);
	bool isFull(int row, int col);

	bool percolate();

private:
	std::vector<std::vector<bool>> m_grid;
	WeightQuickUnionUF m_uf;
};
