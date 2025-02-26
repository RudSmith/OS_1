#include "FileSystemManager.h"

#pragma warning(disable:4996)

namespace fsManager
{

    void FileSystemManager::PrintDrivesList()
    {
        DWORD dwSize = MAX_PATH;
        wchar_t szLogicalDrives[MAX_PATH] = { 0 };
        DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);

        if (dwResult > 0 && dwResult <= MAX_PATH)
        {
            wchar_t* szSingleDrive = szLogicalDrives;
            while (*szSingleDrive)
            {
                wprintf(L"Drive: %s\n", szSingleDrive);

                // get the next drive
                szSingleDrive += wcslen(szSingleDrive) + 1;
            }
        }
    }

    void FileSystemManager::PrintDriveInfo(const std::string& driveName)
    {
        UINT type = GetDriveTypeA(driveName.c_str());

        switch (type)
        {
        case 0:
            std::cout << "Drive unknown.\n";
            return;
            break;
        case 1:
            std::cout << "Invalid root path Error.\n";
            return;
            break;
        case 2:
            std::cout << "Drive is removable.\n";
            break;
        case 3:
            std::cout << "Drive is fixed.\n";
            break;
        case 4:
            std::cout << "Drive is remote \n";
            break;
        case 5:
            std::cout << "Drive is CD-ROM.\n";
            break;
        case 6:
            std::cout << "Drive is a RAMDISK.\n";
            break;
        }

        char NameBuffer[MAX_PATH];
        DWORD serial;
        char fsName[MAX_PATH];
        // fixme добавить вывод системных флагов
        GetVolumeInformationA(driveName.c_str(), NameBuffer, MAX_PATH, &serial, NULL, NULL, fsName, MAX_PATH);

        printf("Volume name: %s\n", NameBuffer);
        printf("Volume serial number: %d\n", serial);
        printf("Filesystem name: %s\n", fsName);

        ULARGE_INTEGER totalBytes;
        ULARGE_INTEGER freeBytes;


        GetDiskFreeSpaceExA(driveName.c_str(), NULL, &totalBytes, &freeBytes);
        printf("Available space %llu bytes of %llu bytes total\n", freeBytes, totalBytes);
    }

    int FileSystemManager::CreateDir(std::filesystem::path path)
    {
        if (CreateDirectoryA(path.string().c_str(), NULL) == FALSE)
            return 1;
        else
            return 0;
    }

    int FileSystemManager::RemoveDir(std::filesystem::path path)
    {
        if (RemoveDirectoryA(path.string().c_str()) == FALSE)
            return 1;
        else
            return 0;
    }

    int FileSystemManager::CreateFile_(std::filesystem::path path)
    {
        HANDLE fileHwnd = NULL;
        fileHwnd = CreateFileA(path.string().c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);

        if (fileHwnd == INVALID_HANDLE_VALUE)
            return 1;
        else
            return 0;
    }

    int FileSystemManager::CopyFile_(std::filesystem::path src, std::filesystem::path dst)
    {
        if (CopyFileA(src.string().c_str(), dst.string().c_str(), TRUE) == FALSE)
            return 1;
        else
            return 0;
    }

    int FileSystemManager::MoveFile_(std::filesystem::path src, std::filesystem::path dst)
    {
        if (MoveFileA(src.string().c_str(), dst.string().c_str()) == FALSE)
            return 1;
        else
            return 0;
    }

    void FileSystemManager::PrintFileAttributes_(std::filesystem::path path)
    {
        DWORD attributes = GetFileAttributesA(path.string().c_str());

        switch (attributes)
        {
        case FILE_ATTRIBUTE_ARCHIVE:
            printf("File is an archive\n");
            break;
        case FILE_ATTRIBUTE_NORMAL:
            printf("File is just a regular file\n");
            break;
        case FILE_ATTRIBUTE_DIRECTORY:
            printf("File is a directory\n");
            break;
        case FILE_ATTRIBUTE_READONLY:
            printf("File is readonly\n");
            break;
        case FILE_ATTRIBUTE_HIDDEN:
            printf("File is hidden\n");
            break;
        default:
            printf("Error getting file attributes\n");
            break;
        }
    }

    void FileSystemManager::PrintExtendedFileInfo(std::filesystem::path path)
    {
        BY_HANDLE_FILE_INFORMATION fileinfo;
        // clear everything in the structure, this is optional
        ZeroMemory(&fileinfo, sizeof(BY_HANDLE_FILE_INFORMATION));

        // obtain a handle to the file, in this case the file
        // must be in the same directory as your application
        HANDLE myfile = NULL;
        myfile = CreateFileA(path.string().c_str(), 0x00, 0x00, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        // if we managed to obtain the desired handle
        if (myfile != INVALID_HANDLE_VALUE)
        {
            //try to fill the structure with info regarding the file
            if (GetFileInformationByHandle(myfile, &fileinfo))
            {
                SYSTEMTIME stCreation;
                FileTimeToSystemTime(&fileinfo.ftCreationTime, &stCreation);
                std::cout << "Creation time: " << stCreation.wDay << "/" << stCreation.wMonth << "/" << stCreation.wYear << " " << stCreation.wHour + 3 << ":" << stCreation.wMinute << std::endl;

                SYSTEMTIME stLastWrite;
                FileTimeToSystemTime(&fileinfo.ftLastWriteTime, &stLastWrite);
                std::cout << "Last write time: " << stLastWrite.wDay << "/" << stLastWrite.wMonth << "/" << stLastWrite.wYear << " " << stLastWrite.wHour + 3 << ":" << stLastWrite.wMinute << std::endl;

                std::cout << "Number of links: " << fileinfo.nNumberOfLinks << std::endl;
            }
            CloseHandle(myfile);
        }
    }

    int FileSystemManager::SetFileAttributes_(std::filesystem::path path, DWORD attributes)
    {
        if (SetFileAttributesA(path.string().c_str(), attributes) == FALSE)
            return 1;
        else
            return 0;
    }

    // взял из https://learn.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time
    void TimetToFileTime(time_t t, LPFILETIME pft)
    {
        ULARGE_INTEGER time_value;
        time_value.QuadPart = (t * 10000000LL) + 116444736000000000LL;
        pft->dwLowDateTime = time_value.LowPart;
        pft->dwHighDateTime = time_value.HighPart;
    }

    int FileSystemManager::SetFileLastWriteTime(std::filesystem::path path, time_t time)
    {
        FILETIME ft;
        if (time != -1)
        {
            HANDLE hFile = CreateFileA(path.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0x00, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                TimetToFileTime(time, &ft);

                if (SetFileTime(hFile, NULL, NULL, &ft) == FALSE)
                {
                    std::cout << GetLastError();
                    CloseHandle(hFile);
                    return 1;
                }
                else
                {
                    CloseHandle(hFile);
                    return 0;
                }
            }
            else
                return 1;
        }
    }

    int FileSystemManager::SetFileCreationTime(std::filesystem::path path, time_t time)
    {
        FILETIME ft;
        if (time != -1)
        {
            HANDLE hFile = CreateFileA(path.string().c_str(), GENERIC_READ | GENERIC_WRITE, 0x00, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                TimetToFileTime(time, &ft);

                if (SetFileTime(hFile, &ft, NULL, NULL) == FALSE)
                {
                    std::cout << GetLastError();
                    CloseHandle(hFile);
                    return 1;
                }
                else
                {
                    CloseHandle(hFile);
                    return 0;
                }
            }
            else
                return 1;
        }
    }

}