#include "header.h"

// based on logged_in_user value display menus
void DisplayMenu() {
    DisplayMap(default_menu, SW_SHOW);
    if (logged_in_user == "") {
        DisplayMap(unlogged_user_menu, SW_SHOW);
    }
    else {
        DisplayMap(logged_user_menu, SW_SHOW);
    }
    if (logged_in_user == "admin") DisplayMap(admin_menu, SW_SHOW);
}

// display or hide all windows in the map except message window
void DisplayMap(std::map < std::string, HWND> myMap, int show) {

    for (std::map<std::string, HWND>::iterator it = myMap.begin(); it != myMap.end(); ++it)
    {
        if (it->first != "message_text") {
            ShowWindow(it->second, show);
        }
    }
}

// display any message inside provided window
void ShowMyMessage(HWND hwnd, std::wstring message) {
    SetWindowTextW(hwnd, message.c_str());
    ShowWindow(hwnd, SW_SHOW);
}

// clear all edit controls values within provided map
void ClearFields(std::map<std::string, HWND> Map) {
    //clear all fields
    char tmp[30] = { 0 };
    for (std::map<std::string, HWND>::iterator it = Map.begin(); it != Map.end(); ++it)
    {
        GetClassNameA(it->second, tmp, 30);
        if (std::string(tmp) == "Edit") {
            SetWindowTextW(it->second, L"");
        }
    }

}

// set headers (column names) for provided list view
void GetListViewHeaders(HWND hwnd, int which) {
    LVCOLUMN LvCol;

    memset(&LvCol, 0, sizeof(LvCol));                  
    LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;    
    LvCol.cx = 0x28;                                  
    LvCol.pszText = const_cast<LPTSTR>(L"username");                           
    LvCol.cx = 200;                                   
    SendMessage(hwnd, LVM_INSERTCOLUMN, 0, (LPARAM)&LvCol); 

    LvCol.pszText = const_cast<LPTSTR>(L"pass_req");                            
    SendMessage(hwnd, LVM_INSERTCOLUMN, 1, (LPARAM)&LvCol); 

    LvCol.pszText = const_cast<LPTSTR>(L"is_blocked");                            
    SendMessage(hwnd, LVM_INSERTCOLUMN, 2, (LPARAM)&LvCol);

    // call to function to set items
    GetListViewItems(hwnd, which);
}

// set item(s) for provided list view
void GetListViewItems(HWND hwnd, int which) {
    LVITEM LvItem;
    memset(&LvItem, 0, sizeof(LvItem)); 

    LvItem.mask = LVIF_TEXT;
    LvItem.cchTextMax = 256;

    // based on 'which' argument, insert all user entries into list or just one
    if (which == -1)
    {
        int n = 0;
        for (std::string i : entries)
        {
            user tmp = GetUser(i);
            LvItem.iItem = n;        
            n++;
            AddUserToListView(hwnd, LvItem, tmp);
        }
    }
    else {
        user tmp = GetUser(entries[which]);
        LvItem.iItem = 0;
        AddUserToListView(hwnd, LvItem, tmp);
    }

}

// insert info about provided user into the list view
void AddUserToListView(HWND hwnd, LVITEM LvItem, user User) {
    wchar_t Temp[300];

    LvItem.iSubItem = 0;      
    swprintf_s(Temp, 300, L"%s", std::wstring(User.username.begin(), User.username.end()).c_str());
    LvItem.pszText = Temp;

    SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)&LvItem); 

    LvItem.iSubItem = 1;
    if (User.meets_req) swprintf_s(Temp, 300, L"true");
    else swprintf_s(Temp, 300, L"false");

    LvItem.pszText = Temp;
    SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)&LvItem); 

    LvItem.iSubItem = 2;
    if (User.is_locked) swprintf_s(Temp, 300, L"true");
    else swprintf_s(Temp, 300, L"false");

    LvItem.pszText = Temp;
    SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)&LvItem); 
}

// hide all child windows of provided window
BOOL CALLBACK HideChildWindows(HWND hwnd, LPARAM lParam) {
    ShowWindow(hwnd, SW_HIDE);
    return true;
}
