#ifndef _FILE_EXPLORER_H
#define _FILE_EXPLORER_H

#include <string>
#include <stack>

enum class FILETYPE;

enum class MenuOperations
{
	NONE = 0,
	LOAD,
	ADD,
	SAVE
};

class FileExplorer
{
public:
	FileExplorer();
	~FileExplorer();

	void Reset();		// Resets the file explorer
	void ResetPath();	// Resets only path related variables
	bool Open();		// Returns true on accepted, false on cancelled or running
	void Draw();		// Draws the modal pop-up
	void DrawPath();	// Draws the path directories bar
	void FilterByFileType(std::string file);	// Draws a selectable for the file given if has the extension selected to filter

public:
	bool openFileExplorer = false;
	char title[100] = "File Explorer";
	char filename[1024] = "";
	std::string path = "";
	std::stack<std::string> pathStack;

	FILETYPE extensionToFilter;
	MenuOperations currentOperation = MenuOperations::NONE;
};

#endif