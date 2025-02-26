#include "View.h"
#include <locale>
#include <string>

void View::Run()
{
    setlocale(LC_ALL, "russian");

    int menuOption = -1;
    while (menuOption != 0)
    {
        std::cout << "Меню" << std::endl;
        std::cout << "1. Получить список дисков" << std::endl;
        std::cout << "2. Получить информацию о диске" << std::endl;
        std::cout << "3. Создать директорию" << std::endl;
        std::cout << "4. Удалить директорию" << std::endl;
        std::cout << "5. Создать файл" << std::endl;
        std::cout << "6. Переместить файл" << std::endl;
        std::cout << "7. Копировать файл" << std::endl;
        std::cout << "8. Вывести информацию о файле" << std::endl;
        std::cout << "9. Изменить атрибуты файла" << std::endl;
        std::cout << "10. Изменить время создания файла" << std::endl;
        std::cout << "11. Изменить время последней записи в файл" << std::endl;
        std::cout << "0. Выход" << std::endl;

        std::cout << "Введите пункт меню:" << std::endl;
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
            std::cout << "Неверный номер пункта меню" << std::endl;
            break;
        }

        system("pause");
        system("cls");
    }
}

void View::getDriveInfo()
{
    std::string diskName{};
    std::cout << "Введите название диска (например C:\\\\): ";
    std::cin >> diskName;

    FileSystemManager::PrintDriveInfo(diskName);
}

void View::createDir()
{
    std::filesystem::path path{};
    std::cout << "Введите путь директории (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;

    if (FileSystemManager::CreateDir(path) == 0)
        std::cout << "Директория успешно создана" << std::endl;
    else
        std::cout << "Ошибка при создании директории" << std::endl;
}

void View::rmDir()
{
    std::filesystem::path path{};
    std::cout << "Введите путь директории (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;

    if (FileSystemManager::RemoveDir(path) == 0)
        std::cout << "Директория успешно удалена" << std::endl;
    else
        std::cout << "Ошибка при удалении директории: директория не существует или непустая" << std::endl;
}

void View::createF()
{
    std::filesystem::path path{};
    std::cout << "Введите путь нового файла (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;

    if (FileSystemManager::CreateFile_(path) == 0)
        std::cout << "Файл успешно создан" << std::endl;
    else
        std::cout << "Ошибка при создании файла" << std::endl;
}

void View::moveF()
{
    std::filesystem::path pathSrc{};
    std::cout << "Введите путь файла, который нужно переместить (используйте / или \\\\ в качестве разделителя):";
    std::cin >> pathSrc;

    std::filesystem::path pathDst{};
    std::cout << "Введите новый путь файла (включая имя файла!!!) (используйте / или \\\\ в качестве разделителя):";
    std::cin >> pathDst;

    if(FileSystemManager::MoveFile_(pathSrc, pathDst) == 0)
        std::cout << "Файл успешно перемещён" << std::endl;
    else
        std::cout << "Ошибка при перемещении файла:" << GetLastError() << std::endl;
}

void View::copyF()
{
    std::filesystem::path pathSrc{};
    std::cout << "Введите путь файла, который нужно копировать (используйте \\\\ в качестве разделителя):";
    std::cin >> pathSrc;

    std::filesystem::path pathDst{};
    std::cout << "Введите путь, куда файл будет копирован (включая имя файла!!!) (используйте \\\\ в качестве разделителя):";
    std::cin >> pathDst;

    if (FileSystemManager::CopyFile_(pathSrc, pathDst) == 0)
        std::cout << "Файл успешно копирован" << std::endl;
    else
        std::cout << "Ошибка при копировании файла:" << GetLastError() <<  std::endl;
}

void View::printFileInfo()
{
    std::filesystem::path path{};
    std::cout << "Введите путь файла (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;
    
    FileSystemManager::PrintExtendedFileInfo(path);
    FileSystemManager::PrintFileAttributes_(path);
}

void View::setFileAttributes()
{
    std::cout << "1. Скрытый\n";
    std::cout << "2. Только для чтения\n";
    std::cout << "3. Архив\n";
    std::cout << "4. Директория\n";
    std::cout << "5. Обычный файл\n";

    std::string attributesStr{};
    DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL;
    std::cout << "Выберите атрибуты из списка и напишите их номера без разделителей (пример 1234):";
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
            std::cout << "Была допущена ошибка во вводе атрибутов!" << std::endl;
            return;
            break;
        }
    }

    std::filesystem::path path{};
    std::cout << "Введите путь файла (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;

    if (FileSystemManager::SetFileAttributes_(path, dwAttributes) == 0)
        std::cout << "Атрибуты успешно изменены" << std::endl;
    else
        std::cout << "Ошибка при изменении атрибутов файла" << std::endl;
}

void View::setFileTime(int method)
{
    struct tm datetime;
    time_t timestamp;

    std::filesystem::path path{};
    std::cout << "Введите путь файла (используйте / или \\\\ в качестве разделителя):";
    std::cin >> path;

    int year, mon, day, hour, min, sec;
    std::cout << "Введите день, месяц, год, час, минуту и секунду создания файла через пробел:";
    std::cin >> day >> mon >> year >> hour >> min >> sec;

    datetime.tm_year = year - 1900; // Number of years since 1900
    datetime.tm_mon = mon - 1; // Number of months since January
    datetime.tm_mday = day;
    datetime.tm_hour = hour - 1;
    datetime.tm_min = min;
    datetime.tm_sec = sec;
    datetime.tm_isdst = 0;

    timestamp = mktime(&datetime);

    if (method == 0)
    {
        if (FileSystemManager::SetFileCreationTime(path, timestamp) == 0)
            std::cout << "Время создания файла успешно изменено" << std::endl;
        else
            std::cout << "Ошибка при изменении времени создания файла" << std::endl;
    }
    else if (method == 1)
    {
        if (FileSystemManager::SetFileLastWriteTime(path, timestamp) == 0)
            std::cout << "Время последней записи в файл успешно изменено" << std::endl;
        else
            std::cout << "Ошибка при изменении времени последней записи в файл" << std::endl;
    }
}
