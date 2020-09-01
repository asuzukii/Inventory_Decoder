#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <vector>
#include <list>
#include "Hashing.h"
using namespace std;

Hashing::Hashing()		//hashtable constructor
{
	m_arr.resize(200000);
}

void Hashing::insert(const string &value, int index) //inserting hashes inside table
{
	unsigned long x = hash<string>()(value);		//hash it up
	unsigned long arrindex = x % 199999;
	Node current(value, index);						//create a node to push back into table
	m_arr.at(arrindex).push_back(current);
}

bool Hashing::search(const string &value, int &index)
{
	unsigned long x = hash<string>()(value);		//hash it up
	unsigned long arrindex = x % 199999;

	if (m_arr.at(arrindex).size() <= 0)				//if the bucket is empty
		return false;
	for (auto it = m_arr[arrindex].begin(); it != m_arr[arrindex].end(); it++) //make an iterator
	{
		if (value.size() != it->m_value.size())		//if the size is not the same as hashsize
			exit(1);

		if (value == it->m_value)					//if iterator's value is equal to input
		{
			
				index = it->order;
				return true;
		}
	}
	return false;

}
int Hashing::findBest(const string &oldString, const string &newString, 
	string byteString, int &bestLength, size_t j) const
{
	int bestIndex = -1;
	bestLength = 0;
	unsigned long x = hash<string>()(byteString);		//hash it up
	unsigned long index = x % 199999;

	size_t BSL = byteString.length();				
	for (auto it = m_arr[index].begin(); it != m_arr[index].end(); it++)		//traversing the bucket
	{
		if (it->m_value == byteString)
		{
			int consecutive = 0;
			while (it->order + BSL + consecutive < oldString.length() && j + BSL + consecutive < newString.length()
				&& oldString[it->order + BSL + consecutive] == newString[j + BSL + consecutive])
				//while not out of bounds from strings and the chars traversed are the same
			{
				consecutive++;
			}
			if (consecutive >= bestLength)
			{
				bestIndex = it->order;
				bestLength = consecutive;
			}
		}
	}
	bestLength += BSL;
	return bestIndex;

}