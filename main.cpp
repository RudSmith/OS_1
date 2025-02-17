#include <Windows.h>
#include "FileSystemManager.h"

int main()
{
    fsManager::FileSystemManager m; 
    
    m.PrintExtendedFileInfo("C:\\Users\\IRudenskiy\\projects\\test\\certs.ps1");
   
    m.PrintFileAttributes_("C:\\Users\\IRudenskiy\\projects\\test");
    m.PrintDrivesList();
    m.PrintDriveInfo("C:\\");

    struct tm datetime;
    time_t timestamp;

    datetime.tm_year = 2025 - 1900; // Number of years since 1900
    datetime.tm_mon = 1; // Number of months since January
    datetime.tm_mday = 16;
    datetime.tm_hour = 22;
    datetime.tm_min = 22;
    datetime.tm_sec = 22;
    // Daylight Savings must be specified
    // -1 uses the computer's timezone setting
    datetime.tm_isdst = -1;

    timestamp = mktime(&datetime);

    std::cout << m.SetFileCreationTime("C:\\Users\\IRudenskiy\\projects\\test\\test.test", timestamp);

}