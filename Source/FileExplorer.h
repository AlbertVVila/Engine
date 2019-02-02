#ifndef _FILE_EXPLORER_H
#define _FILE_EXPLORER_H

#include <string>
#include <stack>

enum class FILETYPE;

enum class MenuOperations
{
	NONE = 0,
	LOAD,
	SAVE
};

class FileExplorer
{
public:
	FileExplorer();
	~FileExplorer();

	void Reset(); //set up a new file dialog request
	bool Open(); //returns true on accepted, false on cancelled or running
	void Draw();
	void DrawPath();

	//members
public:
	bool openFileExplorer = false;
	char title[100] = "File Explorer";
	char file[1024] = "";
	std::string path = "";
	std::stack<std::string> pathStack;

	FILETYPE extensionToFilter;
	MenuOperations currentOperation = MenuOperations::NONE;
};

#endif