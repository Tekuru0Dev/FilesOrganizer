#include <iostream>
#include <filesystem>
#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <windows.h>

using namespace std;
namespace fs = filesystem;

void createDirectory(const string &path)
{
    if (!fs::exists(path))
    {
        try
        {
            fs::create_directories(path);
            cout << "Created directory: " << path << endl;
        }
        catch (fs::filesystem_error &e)
        {
            cerr << "Error creating directory: " << path << " - " << e.what() << endl;
        }
    }
}

void moveFileSafely(const fs::path &source, const fs::path &dest)
{
    try
    {
        if (fs::exists(dest))
        {
            string base = dest.stem().string();
            string ext = dest.extension().string();
            int counter = 1;
            fs::path newTarget;
            do
            {
                newTarget = dest.parent_path() / (base + "_" + to_string(counter) + ext);
                counter++;
            } while (fs::exists(newTarget));
            fs::rename(source, newTarget);
            cout << "Moved with rename: " << source << " to " << newTarget << endl;
        }
        else
        {
            fs::rename(source, dest);
            cout << "Moved: " << source << " to " << dest << endl;
        }
    }
    catch (const fs::filesystem_error &e)
    {
        cerr << "Failed to move: " << source << " -> " << dest << endl;
        cerr << "Reason: " << e.what() << endl;
    }
}

void organizeDownloads(const string &downloadDir, const string &organizeDirectory, const map<string, string> &fileMap)
{
    createDirectory(organizeDirectory);
    for (const auto &entry : fs::directory_iterator(downloadDir))
    {
        if (fs::is_regular_file(entry.status()))
        {
            fs::path filePath = entry.path();
            string extension = filePath.extension().string();

            auto it = fileMap.find(extension);
            if (it != fileMap.end())
            {
                string category = it->second;
                fs::path categoryPath = fs::path(organizeDirectory) / category;
                createDirectory(categoryPath.string());

                fs::path destPath = categoryPath / filePath.filename();
                moveFileSafely(filePath, destPath);
            }
        }
    }
}

string getExecDir()
{
    char buffer[_MAX_PATH];
    GetModuleFileNameA(NULL,buffer,_MAX_PATH);
    fs::path exePath(buffer);
    return exePath.parent_path().string();
}

int main()
{
    string userProfile = getenv("USERPROFILE");
    string downloadDir = userProfile + "\\Downloads";
    fs::path currentDir = fs::current_path();
    string currentDirectory = currentDir.string();
    string organizedDir = downloadDir + "\\Organized";

    map<string, string> fileMap = {
        {".pdf", "Documents"},
        {".docx", "Documents"},
        {".txt", "Documents"},
        {".jpg", "Pictures"},
        {".jpeg", "Pictures"},
        {".png", "Pictures"},
        {".gif", "Pictures"},
        {".mp4", "Videos"},
        {".mkv", "Videos"},
        {".mp3", "Music"},
        {".wav", "Music"},
        {".zip", "Archives"},
        {".rar", "Archives"},
        {".exe", "Software"},
        {".msi", "Software"}};

    // organizeDownloads(downloadDir, organizedDir, fileMap);
    organizeDownloads(currentDirectory, organizedDir, fileMap);

    cout << "\nCurrent directory organized successfully.\n";
    return 0;
}
