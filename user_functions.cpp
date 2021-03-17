#include "header.h"

// create new user entry based on provided user
// return value: entry as a string with all user's info
std::string CreateNewEntry(struct user myUser) {

    std::string entry;
    std::string salt = GetRandomString();

    if (myUser.password.length() != 0) {
        entry = myUser.username + ":$" + salt + "$" + MakeHash((salt + myUser.password).c_str()) + ":" + std::to_string(int((myUser.meets_req))) + ":" + std::to_string(int((myUser.is_locked)));
    }

    else entry = myUser.username + ":*:" + std::to_string(int((myUser.meets_req))) + ":" + std::to_string(int((myUser.is_locked)));
    return entry;
}

// based on provided entry, parse info about the user (except the password)
// return value: user object
user GetUser(std::string entry) {
    user TmpUser;
    int f = entry.find_first_of(':');
    int s = entry.find_first_of(':', f + 1);
    TmpUser.username = entry.substr(0, f);
    TmpUser.is_locked = atoi(entry.substr(entry.find_last_of(':') + 1, 1).c_str());
    TmpUser.meets_req = atoi(entry.substr(s + 1, 1).c_str());
    return TmpUser;
}

// process provided form, extract username and modify entry
void ProccessUsernameAndModifyEntryBool(std::map<std::string, HWND> Map, std::string option, int mode) {
    //hide window with messages for user
    ShowWindow(Map["message_text"], SW_HIDE);

    int lenOut;
    char tmp[30] = { 0 };

    //get username
    lenOut = GetWindowTextA(Map["username_field"], tmp, 30);

    if (UserExists(std::string(tmp)) == -1) {
        ShowMyMessage(Map["message_text"], L"error: username doesn't exist");
        return;
    }

    user newUser;
    newUser.username = std::string(tmp);

    // modify user entry
    // option: block|req
    // mode: 0|1
    ModifyEntryBool(newUser, option, mode);

    switch (mode) {
    case 1:
    {
        if (option == "block") ShowMyMessage(Map["message_text"], L"the account has been blocked.");
        else ShowMyMessage(Map["message_text"], L"the password should meet the requirements now.");
        break;
    }
    case 0:
    {
        if (option == "block") ShowMyMessage(Map["message_text"], L"the account has been unblocked.");
        else ShowMyMessage(Map["message_text"], L"the password should not meet the requirements now.");
        break;
    }
    }
}

// modify one bool in the entry for provided user
void ModifyEntryBool(user User, std::string param, int lock) {
    int off;

    // define offset of needed value from the end of user entry
    if (param == "block") {
        off = -3;
    }
    else if (param == "req") off = -5;
    else return;

    int siz = 0;
    for (int i = 0; i < entries.size(); i++) {
        siz += entries[i].length() + 2;
        if (entries[i].substr(0, entries[i].find_first_of(':')) == User.username) {
            std::fstream file(FILENAME, std::ios::in | std::ios::out | std::ios::binary);
            file.seekg(siz + off, std::ios::beg);
            file.put(lock + '0');
            file.close();
        }
    }

    // update entries
    entries = GetEntriesFromFile(FILENAME);
}

// modify whole entry (after changing user password)
void ModifyEntryString(user User) {
    std::string newStr;

    //create new entry for the user
    newStr = CreateNewEntry(User);
    int siz = 0;

    //find the users old entry and replace it with a new one
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i].substr(0, entries[i].find_first_of(':')) == User.username) {
            std::fstream file(FILENAME, std::ios::in | std::ios::out | std::ios::binary);
            file.seekg(siz, std::ios::beg);
            for (int j = 0; j < newStr.length(); j++) {
                file.put(newStr[j]);
            }
            file.close();
        }
        siz += entries[i].length() + 2;
    }

    // update entries
    entries = GetEntriesFromFile(FILENAME);
}

// get entries from file
std::vector<std::string> GetEntriesFromFile(std::string fileName) {
    std::vector<std::string> tmp;
    std::ifstream file(fileName.c_str());

    //read file contents into a vector
    std::copy(std::istream_iterator<std::string>(file), std::istream_iterator<std::string>(), back_inserter(tmp));

    return tmp;
}

//add entry to the file
bool AddEntryToFile(std::string entry) {
    std::ofstream file;
    file.open(FILENAME, std::ios::app);
    if (file.is_open()) {
        file << entry + "\n";
        file.close();
        return true;
    }
    else return false;
}

// delete user entry
void DeleteUser(int ind) {
    // update entries
    entries = GetEntriesFromFile(FILENAME);
    
    //remove file with entries
    remove(FILENAME);

    //add all entries to file except entry[id]
    for (int i = 0; i < entries.size(); i++) {
        if (i != ind) {
            AddEntryToFile(entries[i]);
        }
    }

    // update entries
    entries = GetEntriesFromFile(FILENAME);
}

// add user that failed log in to the suspicious_users vector
// return value: 0
// note: the program blocks user after 3 unsuccessful logins
int AddSuspiciousUser(std::string username) {
    try {
        suspicious_users.at(username)++;
    }
    catch (const std::out_of_range& oor) {
        suspicious_users.insert(std::pair<std::string, int>(username, 1));
    }

    if (suspicious_users[username] == 3) {
        ProccessUsernameAndModifyEntryBool(login_form, "block", 1);

        // uncomment next line to exit program after 3 unsuccessful logins
        //return 1;
    }

    return 0;
}
