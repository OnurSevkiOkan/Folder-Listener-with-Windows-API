#include <Windows.h>
#include <iostream>
#include "FileFunctions.h"
#include <string>


FileManagement fileManager;

int main()
{
	FreeConsole();
	fileManager.CreateFolder();
	std::wstring DownloadsFolderPath = fileManager.GetDownloadsFolderPath();
	//std::wstring AppdataFolderPath = fileManager.GetAppdataFolderPath();

	const wchar_t* DownloadsfolderPathCStr = DownloadsFolderPath.c_str();
	//const wchar_t* AppdatafolderPathCStr = AppdataFolderPath.c_str();

	fileManager.createListener(DownloadsfolderPathCStr);

	return 0;
}