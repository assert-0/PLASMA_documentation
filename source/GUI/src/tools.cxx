#include "tools.h"

#include <fstream>
#include <cstdio>
#include <windows.h>
#include <direct.h>

//Loads a file and saves it to the byte vector.
bool loadFromFileToBuffer(std::string path, std::vector<char> &data)
{
    int filesize = 0;
    std::ifstream str(path, std::ios::binary);
    if(!str.is_open())
        return false;
    str.seekg(0, std::ios::end);
    filesize = str.tellg();
    data.resize(filesize);
    str.seekg(0, std::ios::beg);
    str.read(&data.front(), filesize);
    return true;
}

//Writes the contents of the byte vector to a file.
void dumpToFileFromBuffer(std::string path, const std::vector<char> &data)
{
    std::ofstream str(path, std::ios::binary);
    if(!str.is_open())
        return;
    str.write((const char*)&data.front(), data.size());
}

void getFileFromExplorer(std::vector<std::string> &results)
{
    OPENFILENAME ofn;

    int MessageSize = 0;
    std::string res = "", tmp = "", dir = "";
    res.resize(4096);
    dir.resize(4096);

    res[0] = '\0';

    GetCurrentDirectory(4096, (char*)&dir[0]);

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = (char*)&res[0];
    ofn.nMaxFile = 4096;
    ofn.lpstrFilter = "";
    ofn.lpstrTitle = "File selection";
    ofn.Flags = OFN_EXPLORER;

    if(!GetOpenFileName(&ofn))
    {
        results.resize(1);
        results[0] += '\0';
        return;
    }

    results.resize(0);

    for(int a = 0; a < 4096 - 1; a++){
        if(res[a] == 0 && res[a + 1] == 0)
        {
            results.push_back(tmp);
            break;
        }
        else if(res[a] == 0)
        {
            results.push_back(tmp);
            tmp = "";
        }
        else
        {
            tmp += res[a];
        }
    }

    _chdir(dir.c_str());
}
