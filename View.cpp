#include "View.h"
#include <locale>
#include <string>

void View::Run()
{
    setlocale(LC_ALL, "russian");

    int menuOption = -1;
    while (menuOption != 0)
    {
        std::cout << "����" << std::endl;
        std::cout << "1. �������� ������ ������" << std::endl;
        std::cout << "2. �������� ���������� � �����" << std::endl;
        std::cout << "3. ������� ����������" << std::endl;
        std::cout << "4. ������� ����������" << std::endl;
        std::cout << "5. ������� ����" << std::endl;
        std::cout << "6. ����������� ����" << std::endl;
        std::cout << "7. ���������� ����" << std::endl;
        std::cout << "8. ������� ���������� � �����" << std::endl;
        std::cout << "9. �������� �������� �����" << std::endl;
        std::cout << "10. �������� ����� �������� �����" << std::endl;
        std::cout << "11. �������� ����� ��������� ������ � ����" << std::endl;
        std::cout << "0. �����" << std::endl;

        std::cout << "������� ����� ����:" << std::endl;
        std::cin >> menuOption;

        switch (menuOption)
        {
        case 1:
            FileSystemManager::PrintDrivesList();
            break;
        case 2:
            getDriveInfo();
            break;
        case 3:
            createDir();
            break;
        case 4:
            rmDir();
            break;
        case 5:
            createF();
            break;
        case 6:
            moveF();
            break;
        case 7:
            copyF();
            break;
        case 8:
            printFileInfo();
            break;
        case 9:
            setFileAttributes();
            break;
        case 10:
            setFileTime(0);
            break;
        case 11:
            setFileTime(1);
            break;
        case 0:
            return;
            break;
        default:
            std::cout << "�������� ����� ������ ����" << std::endl;
            break;
        }

        system("pause");
        system("cls");
    }
}

void View::getDriveInfo()
{
    std::string diskName{};
    std::cout << "������� �������� ����� (�������� C:\\\\): ";
    std::cin >> diskName;

    FileSystemManager::PrintDriveInfo(diskName);
}

void View::createDir()
{
    std::filesystem::path path{};
    std::cout << "������� ���� ���������� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;

    if (FileSystemManager::CreateDir(path) == 0)
        std::cout << "���������� ������� �������" << std::endl;
    else
        std::cout << "������ ��� �������� ����������" << std::endl;
}

void View::rmDir()
{
    std::filesystem::path path{};
    std::cout << "������� ���� ���������� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;

    if (FileSystemManager::RemoveDir(path) == 0)
        std::cout << "���������� ������� �������" << std::endl;
    else
        std::cout << "������ ��� �������� ����������: ���������� �� ���������� ��� ��������" << std::endl;
}

void View::createF()
{
    std::filesystem::path path{};
    std::cout << "������� ���� ������ ����� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;

    if (FileSystemManager::CreateFile_(path) == 0)
        std::cout << "���� ������� ������" << std::endl;
    else
        std::cout << "������ ��� �������� �����" << std::endl;
}

void View::moveF()
{
    std::filesystem::path pathSrc{};
    std::cout << "������� ���� �����, ������� ����� ����������� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> pathSrc;

    std::filesystem::path pathDst{};
    std::cout << "������� ����� ���� ����� (������� ��� �����!!!) (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> pathDst;

    if(FileSystemManager::MoveFile_(pathSrc, pathDst) == 0)
        std::cout << "���� ������� ���������" << std::endl;
    else
        std::cout << "������ ��� ����������� �����" << std::endl;
}

void View::copyF()
{
    std::filesystem::path pathSrc{};
    std::cout << "������� ���� �����, ������� ����� ���������� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> pathSrc;

    std::filesystem::path pathDst{};
    std::cout << "������� ����, ���� ���� ����� ��������� (������� ��� �����!!!) (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> pathDst;

    if (FileSystemManager::CopyFile_(pathSrc, pathDst) == 0)
        std::cout << "���� ������� ���������" << std::endl;
    else
        std::cout << "������ ��� ����������� �����:" << GetLastError() <<  std::endl;
}

void View::printFileInfo()
{
    std::filesystem::path path{};
    std::cout << "������� ���� ����� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;
    
    FileSystemManager::PrintExtendedFileInfo(path);
    FileSystemManager::PrintFileAttributes_(path);
}

void View::setFileAttributes()
{
    std::cout << "1. �������\n";
    std::cout << "2. ������ ��� ������\n";
    std::cout << "3. �����\n";
    std::cout << "4. ����������\n";
    std::cout << "5. ������� ����\n";

    std::string attributesStr{};
    DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL;
    std::cout << "�������� �������� �� ������ � �������� �� ������ ��� ������������ (������ 1234):";
    std::cin >> attributesStr;

    for (char c : attributesStr)
    {
        switch (c)
        {
        case '1':
            dwAttributes |= FILE_ATTRIBUTE_HIDDEN;
            break;
        case '2':
            dwAttributes |= FILE_ATTRIBUTE_READONLY;
            break;
        case '3':
            dwAttributes |= FILE_ATTRIBUTE_ARCHIVE;
            break;
        case '4':
            dwAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            break;
        case '5':
            break;
        default:
            std::cout << "���� �������� ������ �� ����� ���������!" << std::endl;
            return;
            break;
        }
    }

    std::filesystem::path path{};
    std::cout << "������� ���� ����� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;

    if (FileSystemManager::SetFileAttributes_(path, dwAttributes) == 0)
        std::cout << "�������� ������� ��������" << std::endl;
    else
        std::cout << "������ ��� ��������� ��������� �����" << std::endl;
}

void View::setFileTime(int method)
{
    struct tm datetime;
    time_t timestamp;

    std::filesystem::path path{};
    std::cout << "������� ���� ����� (����������� / ��� \\\\ � �������� �����������):";
    std::cin >> path;

    int year, mon, day, hour, min, sec;
    std::cout << "������� ����, �����, ���, ���, ������ � ������� �������� ����� ����� ������:";
    std::cin >> day >> mon >> year >> hour >> min >> sec;

    datetime.tm_year = year - 1900; // Number of years since 1900
    datetime.tm_mon = mon - 1; // Number of months since January
    datetime.tm_mday = day;
    datetime.tm_hour = hour;
    datetime.tm_min = min;
    datetime.tm_sec = sec;
    datetime.tm_isdst = -1;

    timestamp = mktime(&datetime);

    if (method == 0)
    {
        if (FileSystemManager::SetFileCreationTime(path, timestamp) == 0)
            std::cout << "����� �������� ����� ������� ��������" << std::endl;
        else
            std::cout << "������ ��� ��������� ������� �������� �����" << std::endl;
    }
    else if (method == 1)
    {
        if (FileSystemManager::SetFileLastWriteTime(path, timestamp) == 0)
            std::cout << "����� ��������� ������ � ���� ������� ��������" << std::endl;
        else
            std::cout << "������ ��� ��������� ������� ��������� ������ � ����" << std::endl;
    }
}
