#pragma once
#include <iostream>
#include <ctime>
#include <filesystem>
#include <windows.h>

namespace fsManager
{

    class FileSystemManager
    {
    private:
        // добавлена функция для вывода fileSystemFlags
        static void PrintFlags(DWORD flags);
    public:
        // Lists all available drives on the screen
        static void PrintDrivesList();
        // You should pass drive name like this: "C:\\"
        static void PrintDriveInfo(const std::string& driveName);
        // Creates a directory at path. Returns 1 if fails
        static int CreateDir(std::filesystem::path path);
        // Removes a directory at path. Returns 1 if fails
        static int RemoveDir(std::filesystem::path path);
        // Creates a file. Returns 1 if fails
        static int CreateFile_(std::filesystem::path path);
        // Copies file from src to dst. Returns 1 if fails
        static int CopyFile_(std::filesystem::path src, std::filesystem::path dst);
        // Moves or renames file (or dir) from src to dst. Returns 1 if fails
        static int MoveFile_(std::filesystem::path src, std::filesystem::path dst);
        // Prints file attributes (hidden, archive, dir etc.)
        static void PrintFileAttributes_(std::filesystem::path path);
        // Prints file creation and last write time + number of links
        static void PrintExtendedFileInfo(std::filesystem::path path);
        // Sets file attributes passed in DWORD attributes, returns 1 if fails
        static int SetFileAttributes_(std::filesystem::path path, DWORD attributes);
        // Sets last write time of the file, returns 1 if fails
        static int SetFileLastWriteTime(std::filesystem::path path, time_t time);
        // Sets the creation time of the file, returns 1 if fails
        static int SetFileCreationTime(std::filesystem::path path, time_t time);

    };

}