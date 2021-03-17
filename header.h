#pragma once
#include <windows.h>
#include <CommCtrl.h>
#include <map>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <regex>
#include <random>

// structure for storing info about user
struct user{
	std::string username;
	std::string password = "";
	bool meets_req = true;
	bool is_locked = false;
};

// function for parent window
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// crypto and other useful functions
// in file crypto_functions.cpp
std::string MakeHash(const char* data);
std::string StringToHex(const std::string& input);
std::string GetRandomString();
const char* const BoolToString(bool b);

// all kinds of checks (if password if valid, if user exists, etc)
// in file checks_functions.cpp
int UserExists(std::string username);
int CredentialsMatch(user User);
bool FileExists(const std::string& name);
bool AccountLocked(std::string entry);
bool PasswordValid(user User);

// function for displaying or hiding all kinds of info
// in file display_fuctions.cpp
void DisplayMenu();
void DisplayMap(std::map < std::string, HWND> myMap, int show);
void ShowMyMessage(HWND hwnd, std::wstring message);
void ClearFields(std::map<std::string, HWND> Map);
void GetListViewHeaders(HWND hwnd, int which);
void GetListViewItems(HWND hwnd, int which);
void AddUserToListView(HWND hwnd, LVITEM LvItem, user User);
BOOL CALLBACK HideChildWindows(HWND hwnd, LPARAM lParam);

// functions for preparing needed windows and parent window menu
// in file prep_functions.cpp
void CreateWindows(HWND hwnd);
void AddMenus(HWND hwnd);

// functions to work with user entries
// in file user_functions.cpp
std::string CreateNewEntry(struct user myUser);
struct user GetUser(std::string entry);
void ProccessUsernameAndModifyEntryBool(std::map<std::string, HWND> Map, std::string option, int mode);
void ModifyEntryBool(user User, std::string param, int lock);
void ModifyEntryString(user User);
std::vector<std::string> GetEntriesFromFile(std::string fileName);
bool AddEntryToFile(std::string entry);
void DeleteUser(int ind);
int AddSuspiciousUser(std::string username);


#define FILENAME "shadow.txt"
#define ICONFILENAME "file.ico"
#define HASH_LEN 32

#define BTN_ABOUT 1
#define BTN_HOME 2

#define BTN_LOGIN 10
#define BTN_REGISTER 11
#define BTN_LOGIN_SUBM 12
#define BTN_REG_SUBM 13

#define BTN_LOGOUT 20
#define BTN_CHPASS 21
#define BTN_CHPASS_SUBM 22

#define BTN_ADDUSER 40
#define BTN_ADDUSER_SUBMIT 41
#define BTN_BLOCKUSER 42
#define BTN_BLOCKUSER_SUBMIT 43
#define BTN_UNBLOCKUSER_SUBMIT 44
#define BTN_PASSREQ 45
#define BTN_ENPASSREQ_SUBMIT 46
#define BTN_DISPASSREQ_SUBMIT 47
#define BTN_LISTUSERS 48
#define BTN_SHOWUSER 49
#define BTN_USER_NEXT 50
#define BTN_USER_PREV 51
#define BTN_USER_FIRST 52
#define BTN_USER_LAST 53
#define BTN_DELETE_USER 54


// all global vars 
#ifndef __HEADER_H__
#define __HEADER_H__

extern HINSTANCE hInst;
extern HWND login, regist, listViewOne;
extern int showing_user;

extern std::string logged_in_user;
extern std::map<std::string, int> suspicious_users;
extern std::vector<std::string> entries;

extern std::map<std::string, HWND> default_menu, unlogged_user_menu, logged_user_menu, admin_menu;
extern std::map<std::string, HWND> login_form, regist_form;
extern std::map<std::string, HWND> changepass_form, adduser_form, blockuser_form, passreq_form;

#endif


