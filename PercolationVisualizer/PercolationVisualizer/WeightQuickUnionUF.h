#pragma once

#include <vector>

class UnionFind
{
public:
	virtual void connect(int p, int q) = 0;
	virtual int find(int p) = 0;
};

class WeightQuickUnionUF : public UnionFind
{
public:
	WeightQuickUnionUF(int n) : m_parents(n), m_treeSize(n, 1)
	{
		// make each element parent of itself
		for (int i = 0; i < n; ++i)
			m_parents[i] = i;
	};

	void connect(int p, int q) override
	{
		int rootP = root(p);
		int rootQ = root(q);
		
		if (m_treeSize[rootP] >= m_treeSize[rootQ])
			m_parents[rootQ] = rootP;
		else
			m_parents[rootP] = rootQ;
	}

	int find(int p) override
	{
		return root(p);
	}

private:
	int root(int p) {
		if (p >= m_parents.size() || p < 0)
			return -1;

		while (m_parents[p] != p)
		{
			m_parents[p] = m_parents[m_parents[p]];
			p = m_parents[p];
		}

		return p;
	}

private:
	std::vector<int> m_parents;
	std::vector<int> m_treeSize;
};