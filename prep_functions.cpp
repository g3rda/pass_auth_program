#include "header.h"

void startup(LPCSTR lpApplicationName)
{
    // additional information
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcessA
    (
        lpApplicationName,   // the path
        NULL,                // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,     // Opens file in a separate console
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi           // Pointer to PROCESS_INFORMATION structure
    );
    // Close process and thread handles.
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


// gather data for hashing
std::wstring GatherData() {
    const int USERLEN = 30;
    // get user name
    TCHAR username[USERLEN + 1];
    DWORD len = USERLEN + 1;
    if (!GetUserName(username, &len)) {
        return L"error";
    }

    // get computer name
    TCHAR computername[MAX_COMPUTERNAME_LENGTH + 1];
    len = MAX_COMPUTERNAME_LENGTH + 1;
    if (!GetComputerName(computername, &len)) {
        return L"error";
    }

    // get path to Windows OS folder
    TCHAR windowsdirectory[MAX_PATH + 1];
    len = MAX_PATH + 1;
    if (GetWindowsDirectory(windowsdirectory, len) == 0) {
        return L"error";
    }

    // get path to Windows system files folder
    TCHAR systemdirectory[MAX_PATH + 1];
    len = MAX_PATH + 1;
    if (GetSystemDirectory(systemdirectory, len) == 0) {
        return L"error";
    }

    // get mouse buttons number
    int mouse_buttons = GetSystemMetrics(SM_CMOUSEBUTTONS);

    // get screen height
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // get physical memory volume
    MEMORYSTATUS stat;
    GlobalMemoryStatus(&stat);
    int memory = stat.dwTotalPhys;

    // get volume name
    TCHAR volumename[MAX_PATH + 1];
    len = MAX_PATH + 1;
    if (GetVolumeInformation(NULL, volumename, len, NULL, NULL, NULL, NULL, NULL) == 0) {
        return L"error";
    }

    std::wstring result = std::wstring(&username[0]) + std::wstring(&computername[0]) + std::wstring(&windowsdirectory[0]) + std::wstring(&systemdirectory[0]) + std::to_wstring(mouse_buttons) + std::to_wstring(screen_height) + std::to_wstring(memory) + std::wstring(&volumename[0]);

    return result;
}

// create menu 'help' and its submenu 'about'
void AddMenus(HWND hwnd) {
    //Create menu bar
    HMENU hMenubar;
    HMENU hMenu;

    hMenubar = CreateMenu();
    hMenu = CreateMenu();

    AppendMenuW(hMenu, MF_STRING, BTN_ABOUT, L"About");

    AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"Help");
    SetMenu(hwnd, hMenubar);
}

// insert into maps needed windows
void CreateWindows(HWND hwnd) {

    unlogged_user_menu.insert(std::pair<std::string, HWND>("login", CreateWindowEx(0, L"button", L"Log In", WS_CHILD, 675, 20, 100, 40, hwnd, (HMENU)BTN_LOGIN, hInst, NULL)));
    unlogged_user_menu.insert(std::pair<std::string, HWND>("regist", CreateWindowEx(0, L"button", L"Register", WS_CHILD, 800, 20, 100, 40, hwnd, (HMENU)BTN_REGISTER, hInst, NULL)));

    login_form.insert(std::pair<std::string, HWND>("form_name", CreateWindowEx(0, L"STATIC", L"Log In Form", WS_CHILD | SS_CENTER, 425, 100, 100, 20, hwnd, NULL, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("username_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("password_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("username_text", CreateWindowEx(0, L"STATIC", L"Username:", WS_CHILD, 300, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("password_text", CreateWindowEx(0, L"STATIC", L"Password:", WS_CHILD, 300, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("login_button", CreateWindowEx(0, L"button", L"log in", WS_CHILD, 440, 240, 75, 40, hwnd, (HMENU)BTN_LOGIN_SUBM, hInst, NULL)));
    login_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));

    regist_form.insert(std::pair<std::string, HWND>("form_name", CreateWindowEx(0, L"STATIC", L"Registration Form", WS_CHILD | SS_CENTER, 375, 100, 200, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("username_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("password_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("password_again_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 250, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("username_text", CreateWindowEx(0, L"STATIC", L"Username:", WS_CHILD, 250, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("password_text", CreateWindowEx(0, L"STATIC", L"Password:", WS_CHILD, 250, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("password_again_text", CreateWindowEx(0, L"STATIC", L"Repeat password:", WS_CHILD, 250, 250, 150, 20, hwnd, NULL, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("reg_button", CreateWindowEx(0, L"button", L"register", WS_CHILD, 440, 290, 75, 40, hwnd, (HMENU)BTN_REG_SUBM, hInst, NULL)));
    regist_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));

    logged_user_menu.insert(std::pair<std::string, HWND>("logout", CreateWindowEx(0, L"button", L"Log Out", WS_CHILD, 820, 20, 100, 40, hwnd, (HMENU)BTN_LOGOUT, hInst, NULL)));
    logged_user_menu.insert(std::pair<std::string, HWND>("chpass", CreateWindowEx(0, L"button", L"Change Password", WS_CHILD, 650, 20, 150, 40, hwnd, (HMENU)BTN_CHPASS, hInst, NULL)));

    admin_menu.insert(std::pair<std::string, HWND>("listusers", CreateWindowEx(0, L"button", L"List Users", WS_CHILD, 530, 20, 100, 40, hwnd, (HMENU)BTN_LISTUSERS, hInst, NULL)));
    admin_menu.insert(std::pair<std::string, HWND>("showuser", CreateWindowEx(0, L"button", L"Show User", WS_CHILD, 390, 20, 120, 40, hwnd, (HMENU)BTN_SHOWUSER, hInst, NULL)));
    admin_menu.insert(std::pair<std::string, HWND>("adduser", CreateWindowEx(0, L"button", L"Add User", WS_CHILD, 300, 20, 80, 40, hwnd, (HMENU)BTN_ADDUSER, hInst, NULL)));
    admin_menu.insert(std::pair<std::string, HWND>("blockuser", CreateWindowEx(0, L"button", L"Block User", WS_CHILD, 180, 20, 100, 40, hwnd, (HMENU)BTN_BLOCKUSER, hInst, NULL)));
    admin_menu.insert(std::pair<std::string, HWND>("passreq", CreateWindowEx(0, L"button", L"Change Pass Req", WS_CHILD, 20, 20, 140, 40, hwnd, (HMENU)BTN_PASSREQ, hInst, NULL)));

    adduser_form.insert(std::pair<std::string, HWND>("form_name", CreateWindowEx(0, L"STATIC", L"Add user with empty password and username:", WS_CHILD | SS_CENTER, 375, 100, 200, 40, hwnd, NULL, hInst, NULL)));
    adduser_form.insert(std::pair<std::string, HWND>("username_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    adduser_form.insert(std::pair<std::string, HWND>("create_button", CreateWindowEx(0, L"button", L"add", WS_CHILD, 440, 240, 75, 40, hwnd, (HMENU)BTN_ADDUSER_SUBMIT, hInst, NULL)));
    adduser_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));

    blockuser_form.insert(std::pair<std::string, HWND>("form_name", CreateWindowEx(0, L"STATIC", L"(Un)block user with username:", WS_CHILD | SS_CENTER, 375, 100, 200, 40, hwnd, NULL, hInst, NULL)));
    blockuser_form.insert(std::pair<std::string, HWND>("username_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    blockuser_form.insert(std::pair<std::string, HWND>("block_button", CreateWindowEx(0, L"button", L"block", WS_CHILD, 390, 240, 75, 40, hwnd, (HMENU)BTN_BLOCKUSER_SUBMIT, hInst, NULL)));
    blockuser_form.insert(std::pair<std::string, HWND>("unblock_button", CreateWindowEx(0, L"button", L"unblock", WS_CHILD, 490, 240, 75, 40, hwnd, (HMENU)BTN_UNBLOCKUSER_SUBMIT, hInst, NULL)));
    blockuser_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));

    passreq_form.insert(std::pair<std::string, HWND>("form_name", CreateWindowEx(0, L"STATIC", L"(En|Dis)able limitations for password for user with username:", WS_CHILD | SS_CENTER, 300, 100, 350, 20, hwnd, NULL, hInst, NULL)));
    passreq_form.insert(std::pair<std::string, HWND>("username_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    passreq_form.insert(std::pair<std::string, HWND>("block_button", CreateWindowEx(0, L"button", L"enable", WS_CHILD, 390, 240, 75, 40, hwnd, (HMENU)BTN_ENPASSREQ_SUBMIT, hInst, NULL)));
    passreq_form.insert(std::pair<std::string, HWND>("unblock_button", CreateWindowEx(0, L"button", L"disable", WS_CHILD, 490, 240, 75, 40, hwnd, (HMENU)BTN_DISPASSREQ_SUBMIT, hInst, NULL)));
    passreq_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));

    default_menu.insert(std::pair<std::string, HWND>("home_page", CreateWindowEx(0, L"button", L"Home", WS_CHILD | WS_VISIBLE, 675, 600, 100, 40, hwnd, (HMENU)BTN_HOME, hInst, NULL)));
    default_menu.insert(std::pair<std::string, HWND>("exit", CreateWindowEx(0, L"button", L"Exit", WS_CHILD | WS_VISIBLE, 800, 600, 100, 40, hwnd, (HMENU)WM_CLOSE, hInst, NULL)));

    changepass_form.insert(std::pair<std::string, HWND>("curpass_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("pass_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("pass_again_field", CreateWindowEx(0, L"EDIT", 0, WS_CHILD | ES_PASSWORD, 400, 250, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("curpass_text", CreateWindowEx(0, L"STATIC", L"Current password:", WS_CHILD, 225, 150, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("pass_text", CreateWindowEx(0, L"STATIC", L"New password:", WS_CHILD, 225, 200, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("pass_again_text", CreateWindowEx(0, L"STATIC", L"Repeat new password:", WS_CHILD, 225, 250, 150, 20, hwnd, NULL, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("chpass_button", CreateWindowEx(0, L"button", L"change", WS_CHILD, 440, 290, 75, 40, hwnd, (HMENU)BTN_CHPASS_SUBM, hInst, NULL)));
    changepass_form.insert(std::pair<std::string, HWND>("message_text", CreateWindowEx(0, L"STATIC", L"", WS_CHILD | SS_CENTER, 650, 150, 200, 300, hwnd, NULL, hInst, NULL)));
}

