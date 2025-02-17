#pragma once
#include "FileSystemManager.h"

using namespace fsManager;

class View
{
public:
    void Run();

private:
    void getDriveInfo();
    void createDir();
    void rmDir();
    void createF();
    void moveF();
    void copyF();
    void printFileInfo();
    void setFileAttributes();
    // pass 0 for creation and 1 for write time
    void setFileTime(int method);
};

