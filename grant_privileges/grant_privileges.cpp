#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>

inline std::string GetValue(const HKEY hKey, const std::string& sub_key, const std::string& key) {
    HKEY hkey = nullptr;
    LSTATUS res = ::RegOpenKeyExA(hKey, sub_key.c_str(), 0, KEY_READ, &hkey);
    if (res != ERROR_SUCCESS) {
        return "";
    }
    std::shared_ptr<void> close_key(nullptr, [&](void*) {
        if (hkey != nullptr) {
            ::RegCloseKey(hkey);
            hkey = nullptr;
        }
        });
    DWORD type = REG_SZ;
    DWORD size = 0;
    res = ::RegQueryValueExA(hkey, key.c_str(), 0, &type, nullptr, &size);
    if (res != ERROR_SUCCESS || size <= 0) {
        return "";
    }
    std::vector<BYTE> value_data(size);
    res = ::RegQueryValueExA(hkey, key.c_str(), 0, &type, value_data.data(), &size);
    if (res != ERROR_SUCCESS) {
        return "";
    }
    return std::string(value_data.begin(), value_data.end());
}

inline bool SetValue(const HKEY hKey, const std::string& sub_key, const std::string& key, const std::string& value) {
    HKEY hkey = nullptr;
    LSTATUS res = ::RegOpenKeyExA(hKey, sub_key.c_str(), 0, KEY_WRITE, &hkey);
    if (res != ERROR_SUCCESS) {
        res = ::RegCreateKeyA(hKey, sub_key.c_str(), &hkey);
    }
    if (res != ERROR_SUCCESS) {
        return false;
    }
    std::shared_ptr<void> close_key(nullptr, [&](void*) {
        if (hkey != nullptr) {
            ::RegCloseKey(hkey);
            hkey = nullptr;
        }
        });
    res = ::RegSetValueExA(hkey, key.c_str(), 0, REG_SZ, (BYTE*)value.c_str(), value.length());
    if (res != ERROR_SUCCESS) {
        return false;
    }
    return true;
}

int main()
{
    std::string json_filepath{ GetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\AppModel\\Lookaside\\machine\\Software\\Google\\Chrome\\NativeMessagingHosts\\com.apple.passwordmanager", "") };
    if (json_filepath.empty())
    {
        std::cout << "Extension for Chrome/Edge should be installed and enabled via iCloud first." << std::endl;
        getchar();
        return;
    }
    /* uint32_t last_backslash;
     for (uint32_t i = 0; i < json_file.length(); ++i)
     {
         if (json_file[i] == '\\')
         {
             last_backslash = i;
         }
     }
     std::string json_path{ json_file.substr(0, static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(last_backslash) + 1) };
     std::cout << json_file_location << std::endl;
     std::cout << json_file_location.substr(0, last_backslash + 1) << std::endl;*/
    std::ifstream json_file{ json_filepath };
    std::ostringstream tmp;
    tmp << json_file.rdbuf();
    std::string str = tmp.str();
    std::cout << str << std::endl;
    return 0;
}
