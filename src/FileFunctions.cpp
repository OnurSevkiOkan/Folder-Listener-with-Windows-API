#include "FileFunctions.h"
#include <Windows.h>
#include <iostream>
#include <Shlobj.h>


void FileManagement::CreateFolder() { // Creates a folder in appdata
    wchar_t localAppData[MAX_PATH];

    if (GetEnvironmentVariable(L"LOCALAPPDATA", localAppData, MAX_PATH) == 0) {
        std::cout << "Error: %LOCALAPPDATA% environment variable not found.\n";
        return;
    }

    appdataFolder = std::wstring(localAppData) + L"\\FolderName"; // Change the "FolderName" to what you want

    BOOL createDir;

    createDir = CreateDirectory( // Creates directory at Appdata local 
        appdataFolder.c_str(),
        NULL
    );

    if (createDir == FALSE) {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            std::cout << "Folder already exists.\n";
        }
        else {
            std::cerr << "Failed to create folder. Error code: " << error << "\n";
        }
    }
    else {
        std::cout << "Folder created successfully.\n";
    }
}

// Function to get the folder path
std::wstring FileManagement::GetAppdataFolderPath() const {
    return appdataFolder;
}

std::wstring FileManagement::GetDownloadsFolderPath() const { // Gets the download path of user
    if (FAILED(CoInitialize(NULL))) {
        std::cerr << "Failed to initialize COM." << std::endl;
        return L"";
    }

    // Pointer to receive the path
    PWSTR path = nullptr;

    // Get the path to the Downloads folder
    HRESULT result = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &path);
    if (FAILED(result)) {
        std::cerr << "Failed to get Downloads folder path. Error code: " << result << std::endl;
        CoUninitialize(); 
        return L"";
    }

    // Convert the path to std::wstring
    std::wstring downloadsPath = path;
    CoTaskMemFree(path);
    CoUninitialize();

    return downloadsPath;
}

void FileManagement::createListener(const wchar_t* directory)
{
    HANDLE hDirectory = CreateFile(directory,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL);


    if (hDirectory == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open directory: " << GetLastError() << std::endl;
        return;
    }

    DWORD dwBytesReturned;
    char buffer[1024];

    while (true)
    {
        // Creates listener
        if (!ReadDirectoryChangesW(hDirectory,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
            &dwBytesReturned,
            NULL,
            NULL)) {
            std::cerr << "ReadDirectoryChangesW failed: " << GetLastError() << std::endl;
            break;
        }

        // Just prints the changes
        std::cout << "Changes detected:" << std::endl;
        FILE_NOTIFY_INFORMATION* fileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
        do {
            // Print out the action and file name
            std::wcout << L"Action: " << fileInfo->Action << L", File: " << std::wstring(fileInfo->FileName, fileInfo->FileNameLength / sizeof(WCHAR)) << std::endl;

            std::wstring filePath = appdataFolder + L"\\" + std::wstring(fileInfo->FileName, fileInfo->FileNameLength / sizeof(WCHAR));
            std::wstring sourceFilePath = std::wstring(directory) + L"\\" + std::wstring(fileInfo->FileName, fileInfo->FileNameLength / sizeof(WCHAR));
            std::wcout << filePath << "\n";
            if (!CopyFile(sourceFilePath.c_str(), filePath.c_str(), FALSE)) {
                std::cerr << "Failed to copy file: " << GetLastError() << std::endl;
            }

            fileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(fileInfo) + fileInfo->NextEntryOffset);
        }

        while (fileInfo->NextEntryOffset != 0);
    }

    CloseHandle(hDirectory);

}
