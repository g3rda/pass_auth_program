#include "header.h"

// check if user with provided username exist in entries
// return value: id of entry with provided username or -1 (if not found)
int UserExists(std::string username) {
    for (int i = 0; i < entries.size(); i++)
        if (entries[i].substr(0, entries[i].find_first_of(':')) == username) {
            return i;
        }
    return -1;
}

// check if account with provided credentials exists
// return value: -1 if account exists and is locked; 1 if credentials are valid; 0 otherwise
int CredentialsMatch(user User) {
    for (std::string i : entries)
        if (i.substr(0, i.find_first_of(':')) == User.username) {
            int f = i.find_first_of(':');
            int s = i.find_first_of(':', f + 1);
            std::string pass = i.substr(f + 1, s - f - 1);
            if (pass == "*") {
                if (User.password.length() == 0) {
                    if (AccountLocked(i)) return -1;
                    else return 1;
                }
            }
            else {
                std::string salt = pass.substr(1, 4);
                std::string hash = pass.substr(6, HASH_LEN);
                if (MakeHash((salt + User.password).c_str()) == hash)
                    if (AccountLocked(i)) return -1;
                    else return 1;
            }
        }
    return 0;
}

// file size
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

// check if file with provided filename exists
bool FileExists(const std::wstring& name) {
    struct stat buffer;
    std::string s(name.begin(), name.end());
    return (stat(s.c_str(), &buffer) == 0);
}

// check if account for provided entry is locked
bool AccountLocked(std::string entry) {
    if (entry.substr(entry.find_last_of(':') + 1, 1) == "0") return false;
    else return true;
}

// check if password is valid
// the function conducts one of two checks based on users meets_req value
bool PasswordValid(user User) {
    if (User.meets_req) {
        std::regex re("(?=.*[[:punct:]])(?=.*[[:upper:]])(?=.*[[:lower:]])(.{4,30})");
        return std::regex_match(User.password, re);
    }
    else {
        std::regex re("(.{4,30})");
        return std::regex_match(User.password, re);
    }
}