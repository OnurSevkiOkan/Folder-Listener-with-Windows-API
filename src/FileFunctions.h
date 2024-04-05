#include <string>

class FileManagement
{
private:
	bool isFolderCreated;
	std::wstring appdataFolder;
	std::wstring downloadsFolder;
public:
	void CreateFolder();
	void createListener(const wchar_t* directory);
	std::wstring GetAppdataFolderPath() const;
	std::wstring GetDownloadsFolderPath() const;
};