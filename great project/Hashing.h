#ifndef HASHING_H
#define HASHING_H
#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <list>
using namespace std;

struct Node
{
	Node(const string &myVal, int index)
	{
		m_value = myVal;
		order = index;
	}
	string m_value;
	int order;
};

class Hashing
{
public: 
	Hashing();
	void insert(const string &value, int index);
	bool search(const string &value, int &index);
	int findBest(const string &oldString, const string &newString, 
		string byteString, int &bestLength, size_t j) const;
	
private:
	vector<list<Node>> m_arr;
};

#endif