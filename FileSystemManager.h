#pragma once
#include <iostream>
#include <ctime>
#include <filesystem>
#include <windows.h>

namespace fsManager
{

    class FileSystemManager
    {
    public:
        // Lists all available drives on the screen
        void PrintDrivesList();
        // You should pass drive name like this: "C:\\"
        void PrintDriveInfo(const std::string& driveName);
        // Creates a directory at path. Returns 1 if fails
        int CreateDir(std::filesystem::path path);
        // Removes a directory at path. Returns 1 if fails
        int RemoveDir(std::filesystem::path path);
        // Creates a file. Returns 1 if fails
        int CreateFile_(std::filesystem::path path);
        // Copies file from src to dst. Returns 1 if fails
        int CopyFile_(std::filesystem::path src, std::filesystem::path dst);
        // Moves or renames file (or dir) from src to dst. Returns 1 if fails
        int MoveFile_(std::filesystem::path src, std::filesystem::path dst);
        // Prints file attributes (hidden, archive, dir etc.)
        void PrintFileAttributes_(std::filesystem::path path);
        // Prints file creation and last write time + number of links
        void PrintExtendedFileInfo(std::filesystem::path path);
        // Sets file attributes passed in DWORD attributes, returns 1 if fails
        int SetFileAttributes_(std::filesystem::path path, DWORD attributes);
        // Sets last write time of the file, returns 1 if fails
        int SetFileLastWriteTime(std::filesystem::path path, time_t time = -1);
        // Sets the creation time of the file, returns 1 if fails
        int SetFileCreationTime(std::filesystem::path path, time_t time = -1);
    };

}