#include <iostream>
#include <fstream>
#include <string>
#include "Hashing.h"
#include <sstream>  // for istringstream and ostringstream

#include <algorithm>
#include <iterator>
#include <cassert>
using namespace std;

void createDiff(istream& fold, istream& fnew, ostream& fdiff);
bool getInt(istream& inf, int& n);
bool getCommand(istream& inf, char& cmd, int& length, int& offset);
int countDigit(int n);
bool applyDiff(istream& fold, istream& fdiff, ostream& fnew);


void runtest(string oldtext, string newtext)
{
	istringstream oldFile(oldtext);
	istringstream newFile(newtext);
	ostringstream diffFile;
	createDiff(oldFile, newFile, diffFile);
	string result = diffFile.str();
	cout << "The diff file length is " << result.size()
		<< " and its text is " << endl;
	cout << result << endl;

	oldFile.clear();   // clear the end of file condition
	oldFile.seekg(0);  // reset back to beginning of the stream
	istringstream diffFile2(result);
	ostringstream newFile2;
	assert(applyDiff(oldFile, diffFile2, newFile2));
	assert(newtext == newFile2.str());
}

int main()
{
	runtest("There's a bathroom on the right.",
		"There's a bad moon on the rise.");
	runtest("abcdefgh12345678",
		"12345678abcd5fgh");
	runtest("ABCDEFGHIJBLAHPQRSTUVPQRSTUV",
		"XYABCDEFGHIJBLETCHPQRSTUVPQRSTQQELF");
	cout << "All tests passed" << endl;
	ifstream fold("/Users/Akira/Desktop/test_files/greeneggs1.txt");
	ifstream fnew("/Users/Akira/Desktop/test_files/greeneggs2.txt");
	ofstream fdiff("/Users/Akira/Desktop/test_files/diff.txt");
	createDiff(fold, fnew, fdiff);
	cout << "\ndone\n";
	return 0;
}

void createDiff(istream& fold, istream& fnew, ostream& fdiff)
{
	//Read in the entire contents of the old file into a string. 
	char c;
	string oldString = "";
	while (fold.get(c))
	{
		oldString += c;
	}

	//Read the entire contents of the new file into another string.
	string newString = "";
	while (fnew.get(c))
	{
		newString += c;
	}
	size_t hashsize = 8;
	if (newString.length() > 50000)
		hashsize = 32;

	

	if (oldString == newString || newString.length() == 0)
	{
		fdiff << '\n';
		return;
	}

	if (oldString.length() < hashsize || newString.length() < hashsize)
	{
		fdiff << "A" << newString.length() << ":" << newString;
		return;
	}

	Hashing hashTable;

	for (size_t i = 0; i <= oldString.length() - hashsize; i++)	//inserting every sequence in the oldString
	{
		hashTable.insert(oldString.substr(i, hashsize), i);
	}

	//Look up the N - byte sequence which starts at offset j([j, j + N - 1]) in the new file's string 
	//in the table you created from the old file's string.
	int index = 0;
	size_t j = 0;
	for (; j <= newString.length() - hashsize; j++)
	{
		if (hashTable.search(newString.substr(j, hashsize), index))	//if there is an equal
		{
			/*int index2 = index - 1;									//traverse for more matches
			for (; j + hashsize < newString.length() && hashTable.search(newString.substr(j, hashsize), index2, true); j++)
			{}
			j--;
			fdiff << "C" << (index2 - index + hashsize) << "," << index;	//write out to ostream how much I can copy
			j += 7;*/
			int bestLength = 0;
			int bestIndex = hashTable.findBest(oldString, newString, newString.substr(j, hashsize), bestLength, j);
			fdiff << "C" << (bestLength) << "," << bestIndex;
			j = j + bestLength - 1;
		}
		else //if I cannot find the byte
		{
			//Write an instruction to the diff file to Add the current character.
			//but first we want to find if there is a way make a copy command
			int k = 0;
			for (; k + j <= newString.length() - hashsize; k++)
			{
				if (hashTable.search(newString.substr(j + k, hashsize), index))
				{
					fdiff << "A" << k << ":" << newString.substr(j, k);
					j = j + k - 1;
					break;
				}
			}
			if (k + j > newString.length() - hashsize)
			{
				fdiff << "A" << (newString.length() - j) << ":" << newString.substr(j);
				return;
			}
										
		}
	}
	if (newString.substr(j).length() > 0)
		fdiff << "A" << (newString.length() - j) << ":" << newString.substr(j);

}

bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, int& length, int& offset)
{
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	char ch;
	switch (cmd)
	{
	case 'A':
		return getInt(inf, length) && inf.get(ch) && ch == ':';
	case 'C':
		return getInt(inf, length) && inf.get(ch) && ch == ',' && getInt(inf, offset);
	case '\r':
	case '\n':
		return true;
	}
	return false;
}


bool applyDiff(istream& fold, istream& fdiff, ostream& fnew)
{
	//take in fold as a string
	char c;
	string oldString = "";
	while (fold.get(c))
	{
		oldString += c;
	}

	while (fdiff)
	{
		//A2:XYC12, 0A3 : ETCC13, 13A5 : QQELF
		char cmd = '!';
		int length = 0, offset = 0;
		if (!getCommand(fdiff, cmd, length, offset))
			return false;
		switch (cmd)
		{
		case 'A':
			if (length < 0)
				return false;
			while (length != 0)
			{
				if (!fdiff.get(c))
					return false;
				fnew << c;
				length--;
			}
			break;
		case 'C':
			if (offset + length > static_cast<int>(oldString.size()) || offset < 0 || length < 0)
				return false;
			fnew << oldString.substr(offset, length);
			break;
		case 'x':
			return true;
		case '\n':
		case '\r':
		default:
			break;
		}
	}
	return true;
}