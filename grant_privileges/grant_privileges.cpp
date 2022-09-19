#include "grant_privileges.h"

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
    std::string icloud_dir;
    if (json_filepath.empty())
    {
        std::cout << "Extension for Chrome/Edge should be installed and enabled via iCloud first." << std::endl;
        getchar();
        return 0;
    }
    else
    {
        uint64_t last_backslash{ json_filepath.length() };
        for (auto i = last_backslash; i > 0;)
        {
            if (json_filepath[--i] == '\\')
            {

                last_backslash = i;
                break;
            }
        }
        if (last_backslash == 0)
        {
            std::cout << "Failed to read iCloud Client path, please try to re-enable iCloud Passwords for Chrome/Edge." << std::endl;
        }
        icloud_dir = json_filepath.substr(0, last_backslash + 1);
    }
    std::ifstream json_file{ json_filepath };
    configor::json j;
    json_file >> j;
    std::string firefox_json_filepath{ getenv("USERPROFILE") };
    if (firefox_json_filepath[firefox_json_filepath.length()] != '\\') {
        firefox_json_filepath += "\\.config";
    }
    else
    {
        firefox_json_filepath += ".config";
    }
    if (0 != _access(firefox_json_filepath.c_str(), 0))
    {
        if (0 != _mkdir(firefox_json_filepath.c_str()))
        {
            std::cout << "Failed to mkdir, please try to rerun using Administrator privilege.";
        }
    }
    firefox_json_filepath += "\\passwordmanager_for_firefox";
    if (0 != _access(firefox_json_filepath.c_str(), 0))
    {
        if (0 != _mkdir(firefox_json_filepath.c_str()))
        {
            std::cout << "Failed to mkdir, please try to rerun using Administrator privilege.";
        }
    }
    firefox_json_filepath += "\\FirefoxPwdMgrHostApp_manifest.json";
    configor::json j2{
        { "name", j["name"]},
        { "description", "Apple iCloud Firefox Password Manager Host App" },
        { "path", icloud_dir + (std::string)j["path"]},
        { "type", j["type"]},
        { "allowed_extensions", { "{724e544e-ae3e-4fc9-9262-7f650d731cd8}" } },
    };
    std::ofstream outfile{ firefox_json_filepath };
    outfile << std::setw(4) << j2 << std::endl;
    if (SetValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\AppModel\\Lookaside\\machine\\Software\\Mozilla\\NativeMessagingHosts\\com.apple.passwordmanager", "", firefox_json_filepath))
    {
        std::cout << "The Firefox Extension is ready to run." << std::endl;
    }
    else
    {
        std::cout << "Failed to set REG value, please try to rerun using Administrator privilege." << std::endl;
    }
    getchar();
    return 0;
}
