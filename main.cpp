#include "header.h"


// global vars
HINSTANCE hInst;
HWND login, regist, listViewOne;
int showing_user;

std::string logged_in_user = "";
std::map<std::string, int> suspicious_users;
std::vector<std::string> entries;

std::map<std::string, HWND> default_menu, unlogged_user_menu, logged_user_menu, admin_menu;
std::map<std::string, HWND> login_form, regist_form;
std::map<std::string, HWND> changepass_form, adduser_form, blockuser_form, passreq_form;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    
    BYTE buffer[512];
    DWORD dwType, dwLen;
    HKEY hkResult;
    HCRYPTPROV hProv;
    HCRYPTHASH hHash;
    HCRYPTKEY hKey;

    // open registry key
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Kostetska", 0, KEY_READ, &hkResult) != ERROR_SUCCESS) {
        MessageBoxW(NULL, (LPCWSTR)L"The registry not found", (LPCWSTR)L"Error", MB_OK);
        return 1;
    }

    // try to read the signature
    if (RegQueryValueExA(hkResult, "Signature", NULL, &dwType, (LPBYTE)buffer, &dwLen) == ERROR_SUCCESS) {
        RegCloseKey(hkResult);
    }
    else {
        MessageBoxW(NULL, (LPCWSTR)L"Could not read from the registry", (LPCWSTR)L"Error", MB_OK);
        return 1;
    }

    // if len(signature)==0 (which means that the program was installed and is running for the first time), run Setup and exit
    if (dwLen == 0) {
        startup("SetupProgram_v1.exe");
        MessageBoxW(NULL, (LPCWSTR)L"We have prepared everything, you may start the program now.", (LPCWSTR)L"Message", MB_OK);
        return 0;
    }

    // gather data for hashing
    std::wstring data = GatherData();
    if (data == L"error") {
        return 1;
    }

    std::string s(data.begin(), data.end());
    const BYTE* bytes_data = reinterpret_cast<const BYTE*>(s.c_str());



    // Acquire a cryptographic provider context handle.
    
    if (CryptAcquireContext( &hProv, NULL, NULL, PROV_RSA_FULL, 0))    {
        //MessageBoxA(NULL, std::string("CSP context acquired.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }  else  {
        MessageBoxA(NULL, std::string("Error during CryptAcquireContext.").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }
    
    
    // get user key 

    if (CryptGetUserKey(  hProv,  AT_SIGNATURE, &hKey))  {
        //MessageBoxA(NULL, std::string("The signature key has been acquired. \n").c_str(), (LPCSTR)"Error", MB_OK);
    }  else  {
        MessageBoxA(NULL, std::string("Error during CryptGetUserKey for signkey.").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }


    // Create the hash object.

    if (CryptCreateHash( hProv, CALG_MD5,  0, 0, &hHash))
    {
       // MessageBoxA(NULL, std::string("Hash object created. \n").c_str(), (LPCSTR)"Error", MB_OK);
    } else {
        MessageBoxA(NULL, std::string("Error during CryptCreateHash.").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }


    // Compute the cryptographic hash of the buffer.

    if (CryptHashData( hHash, bytes_data, s.length() + 1, 0))    {
        //MessageBoxA(NULL, std::string("The data buffer has been hashed.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }   else   {
        MessageBoxA(NULL, std::string("Error during CryptHashData.").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }


    // Validate the digital signature.

    if (CryptVerifySignature(hHash, buffer, dwLen,  hKey, NULL, 0)) {
        //MessageBoxA(NULL, std::string("The signature has been verified.\n").c_str(), (LPCSTR)"Error", MB_OK);
    } else {
        MessageBoxA(NULL, std::string("Signature not validated!\n").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }


    // if no file with user records, create one with admin
    if (!FileExists(FILENAME)) {
        std::ofstream file(FILENAME);
        file << "admin:*:1:0\n";
        file.close();
    }

    // get existing entries from the file
    entries = GetEntriesFromFile(FILENAME);
    
    showing_user = 0;

    // register the window class.
    const wchar_t myclass[] = L"Window Class";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = myclass;
    wc.hbrBackground = CreateSolidBrush(RGB(229, 204, 255));
    wc.hIcon = (HICON)LoadImage(NULL, TEXT(ICONFILENAME), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    RegisterClass(&wc);

    // create the window.

    HWND hwnd = CreateWindowEx(0, myclass, L"Password authentication", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdcStatic = NULL;

    // always display menu
    DisplayMenu();

    switch (uMsg)
    {
    case WM_CREATE:
    {
        // after creation, add menu to the parent window and create needed child windows
        AddMenus(hwnd);
        CreateWindows(hwnd);

        // go to home
        SendMessage(hwnd, WM_COMMAND, BTN_HOME, lParam);
        break;
    }
    case WM_CTLCOLORSTATIC:
    {
        // set transparent background for static child windows
        hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkMode(hdcStatic, TRANSPARENT);

        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    case WM_DESTROY:
    {
        //exit program
        PostQuitMessage(0);
        return 0;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {

        // show "About" messagebox
        case BTN_ABOUT:
        {
            int msgbox = MessageBoxW(NULL, (LPCWSTR)L"This program was created by Alina Kostetska (FB-83).\nIndividual task: password should contain at least one uppercase letter, one lowercase letter, and one punctuation mark.", (LPCWSTR)L"Program details", MB_OK);
            break;
        }

        // home tab to display a message for user
        case BTN_HOME:      
        {
            // delete child windows and display a message
            EnumChildWindows(hwnd, HideChildWindows, NULL);

            if (logged_in_user == "") {
                CreateWindowEx(0, L"STATIC", L"Hello!\nLog in to fully enjoy the program.", WS_CHILD | WS_VISIBLE | SS_CENTER, 355, 250, 250, 250, hwnd, NULL, hInst, NULL);
            }
            else {
                std::string tmp = "Hello, "+logged_in_user+".";
                CreateWindowExA(0, "STATIC", tmp.c_str(), WS_CHILD | WS_VISIBLE| SS_CENTER, 355, 250, 250, 250, hwnd, NULL, hInst, NULL);
            }
            break;
        }

        // show message box, exit program
        case WM_CLOSE:
        {
            if (MessageBox(hwnd, L"Really quit?", L"", MB_OKCANCEL) == IDOK)
            {
                PostQuitMessage(0);
            }
            return 0;
        }

        // show login form
        case BTN_LOGIN:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);
            DisplayMap(login_form, SW_SHOW);
            break;
        }

        // show registration form
        case BTN_REGISTER:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);
            DisplayMap(regist_form, SW_SHOW);
            break;
        }

        // submit login form
        case BTN_LOGIN_SUBM:
        {
            //hide window with messages for user
            ShowWindow(login_form["message_text"], SW_HIDE);

            int lenOut;
            char tmp[30] = { 0 };

            user logUser;

            // get username from the field
            lenOut = GetWindowTextA(login_form["username_field"], tmp, 30);
            if (lenOut == 0) {
                ShowMyMessage(login_form["message_text"], L"error: enter username");
                break;
            }

            logUser.username = std::string(tmp);

            // get password from the field
            lenOut = GetWindowTextA(login_form["password_field"], tmp, 30);

            logUser.password = std::string(tmp);

            // check if credentials match
            int ret = CredentialsMatch(logUser);

            if (ret == 0) {
                ShowMyMessage(login_form["message_text"], L"error: username and/or password incorrect");
                if (AddSuspiciousUser(logUser.username)==1) SendMessage(hwnd, WM_DESTROY, NULL, lParam);
            }
            else if (ret == -1) {
                ShowMyMessage(login_form["message_text"], L"error: you account is locked. contact admin");
            }
            else {
                logged_in_user = logUser.username;

                // log user in and show home tab
                SendMessage(hwnd, WM_COMMAND, BTN_HOME, lParam);
            }

            // clear any values in edit fields
            ClearFields(login_form);

            break;
        }

        // submit registration form
        case BTN_REG_SUBM:
        {
            // hide window with messages for user
            ShowWindow(regist_form["message_text"], SW_HIDE);

            int lenOut;
            char tmp[30] = { 0 };

            // get username and check if len(username > 1) and if user with the username already exists
            lenOut = GetWindowTextA(regist_form["username_field"], tmp, 30);

            if (lenOut == 0 || lenOut == 1) {
                ShowMyMessage(regist_form["message_text"], L"error: username should be length 2-30");
                break;
            }

            if (UserExists(std::string(tmp))!=-1) {
                ShowMyMessage(regist_form["message_text"], L"error: username exists");
                break;
            }

            user newUser;
            newUser.username = std::string(tmp);

            // get password and check if its len > 3 and if it matches requirements
            lenOut = GetWindowTextA(regist_form["password_field"], tmp, 30);
            
            
            newUser.password = std::string(tmp);

            if (lenOut < 4 || !PasswordValid(newUser)) {
                ShowMyMessage(regist_form["message_text"], L"error: password should be length 4-30 with at least one uppercase, lowercase letters and punctuation mark");
                break;
            }

            

            // check if passwords match
            lenOut = GetWindowTextA(regist_form["password_again_field"], tmp, 30);

            if (std::string(tmp) != newUser.password) {
                ShowMyMessage(regist_form["message_text"], L"error: passwords do not match");
                break;
            }

            // create new record for the registered user
            std::string entry = CreateNewEntry(newUser);
            entries.push_back(entry);
            AddEntryToFile(entry);

            // clear all edit fields
            ClearFields(regist_form);

            // message about registratopm success
            ShowMyMessage(regist_form["message_text"], L"new user successfully registered. you can log in now.");
            break;
        }

        // go to change password tab
        case BTN_CHPASS:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);
            DisplayMap(changepass_form, SW_SHOW);
            break;
        }

        // submit change password form
        case BTN_CHPASS_SUBM:
        {
            // hide window with messages for user
            ShowWindow(changepass_form["message_text"], SW_HIDE);

            int lenOut;
            char tmp[30] = { 0 };

            user newUser;
            newUser.username = std::string(logged_in_user);

            // create new user struct with username from logged in user and password from the form
            lenOut = GetWindowTextA(changepass_form["curpass_field"], tmp, 30);
            newUser.password = std::string(tmp);

            // check if password is correct and if account is blocked
            int res = CredentialsMatch(newUser);
            if (res == 0) {
                ShowMyMessage(changepass_form["message_text"], L"error: password is incorrect");
                ClearFields(changepass_form);
                break;
            }
            else if (res == -1) {
                ShowMyMessage(changepass_form["message_text"], L"error: account is blocked");
                ClearFields(changepass_form);
                break;
            }

            // get new password 
            lenOut = GetWindowTextA(changepass_form["pass_field"], tmp, 30);

            newUser.password = std::string(tmp);
            int ind = UserExists(newUser.username);

            // obtain current users characteristics and put it into new user
            user oldUser = GetUser(entries[ind]);
            newUser.is_locked = oldUser.is_locked;
            newUser.meets_req = oldUser.meets_req;

            // check if new passwords len > 3 and if it matches requirements
            if (lenOut < 4 || !PasswordValid(newUser)) {
                if (newUser.meets_req) {
                    ShowMyMessage(changepass_form["message_text"], L"error: password should be length 4-30 with at least one uppercase and lowercase letter and punctuation mark");
                }
                else 
                    ShowMyMessage(changepass_form["message_text"], L"error: password should be length 4-30");
                ClearFields(changepass_form);
                break;
            }


            //check if passwords match
            lenOut = GetWindowTextA(changepass_form["pass_again_field"], tmp, 30);

            if (std::string(tmp) != newUser.password) {
                ShowMyMessage(changepass_form["message_text"], L"error: passwords do not match");
                ClearFields(changepass_form);
                break;
            }

            // modify entry about user
            ModifyEntryString(newUser);

            //clear all fields
            ClearFields(changepass_form);

            //message about registratopm success
            ShowMyMessage(changepass_form["message_text"], L"your password was successfully changed.");

            break;
        }

        // open 'list users' tab
        case BTN_LISTUSERS:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);

            // create list view to list all users and fill it
            HWND listview = CreateWindowExW(0, WC_LISTVIEW, L"SysListView32", WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 100, 100, 600, 480, hwnd, NULL, hInst, NULL);
            GetListViewHeaders(listview, -1);

            // make it visible
            ShowWindow(listview, SW_SHOW);

            break;
        }

        // open 'show user' tab
        case BTN_SHOWUSER:
        {
            
            if (lParam != 1)
            {
                // if it is the first time opening the tab, create child windows and show one user (showing_user)
                EnumChildWindows(hwnd, HideChildWindows, NULL);
                listViewOne = CreateWindowExW(0, WC_LISTVIEW, L"SysListView32", WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 150, 120, 600, 40, hwnd, NULL, hInst, NULL);
                GetListViewHeaders(listViewOne, showing_user);
                ShowWindow(listViewOne, SW_SHOW);

                CreateWindowEx(0, L"button", L"next user", WS_CHILD | WS_VISIBLE, 200, 200, 100, 40, hwnd, (HMENU)BTN_USER_NEXT, hInst, NULL);
                CreateWindowEx(0, L"button", L"previous user", WS_CHILD | WS_VISIBLE, 500, 200, 100, 40, hwnd, (HMENU)BTN_USER_PREV, hInst, NULL);
                CreateWindowEx(0, L"button", L"first user", WS_CHILD | WS_VISIBLE, 200, 300, 100, 40, hwnd, (HMENU)BTN_USER_FIRST, hInst, NULL);
                CreateWindowEx(0, L"button", L"last user", WS_CHILD | WS_VISIBLE, 500, 300, 100, 40, hwnd, (HMENU)BTN_USER_LAST, hInst, NULL);
                CreateWindowEx(0, L"button", L"delete user", WS_CHILD | WS_VISIBLE, 200, 400, 400, 40, hwnd, (HMENU)BTN_DELETE_USER, hInst, NULL);
            }
            else {
                // delete current user in list view and insert a new one (stored in showing_user)
                SendMessage(listViewOne, LVM_DELETEITEM, 0, 0);
                GetListViewItems(listViewOne, showing_user);
            }
            
            break;
        }

        // show user+1 in 'show user' tab
        case BTN_USER_NEXT:
        {
            if (entries.size() - 1 >= showing_user + 1) showing_user += 1;
            else showing_user = 0;

            SendMessage(hwnd, WM_COMMAND, BTN_SHOWUSER, 1);
            break;
        }

        // show user-1 in 'show user' tab
        case BTN_USER_PREV:
        {
            if (0 <= showing_user - 1) showing_user -= 1;
            else showing_user = entries.size() - 1;

            SendMessage(hwnd, WM_COMMAND, BTN_SHOWUSER, 1);
            break;
        }

        // show first user in 'show user' tab
        case BTN_USER_FIRST:
        {
            showing_user = 0;

            SendMessage(hwnd, WM_COMMAND, BTN_SHOWUSER, 1);
            break;
        }

        // show last user in 'show user' tab
        case BTN_USER_LAST:
        {
            showing_user = entries.size() - 1;

            SendMessage(hwnd, WM_COMMAND, BTN_SHOWUSER, 1);
            break;
        }

        // delete user that is currently shown
        case BTN_DELETE_USER:
        {
            DeleteUser(showing_user);

            SendMessage(hwnd, WM_COMMAND, BTN_USER_NEXT, NULL);
            break;
        }

        // open 'add user' tab
        case BTN_ADDUSER:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);
            DisplayMap(adduser_form, SW_SHOW);
            break;
        }

        // submit add user form
        case BTN_ADDUSER_SUBMIT:
        {
            // hide window with messages for user
            ShowWindow(adduser_form["message_text"], SW_HIDE);

            int lenOut;
            char tmp[30] = { 0 };

            // get username and check if len(username > 1) and if user with the username already exists
            lenOut = GetWindowTextA(adduser_form["username_field"], tmp, 30);

            if (lenOut == 0 || lenOut == 1) {
                ShowMyMessage(adduser_form["message_text"], L"error: username should be length 2-30");
                break;
            }

            if (UserExists(std::string(tmp)) != -1) {
                ShowMyMessage(adduser_form["message_text"], L"error: username exists");
                break;
            }

            user newUser;
            newUser.username = std::string(tmp);

            // create new entry for the user and add it to the file
            std::string entry = CreateNewEntry(newUser);
            entries.push_back(entry);
            AddEntryToFile(entry);

            // clear edit fields in the form
            ClearFields(adduser_form);

            // message about 'add user' success
            ShowMyMessage(adduser_form["message_text"], L"new user successfully registered.");
            break;

        }

        // open 'block user' tab
        case BTN_BLOCKUSER:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);
            DisplayMap(blockuser_form, SW_SHOW);
            break;
        }

        // block or unblock user from 'block user' tab  
        case BTN_BLOCKUSER_SUBMIT:
        {
            ProccessUsernameAndModifyEntryBool(blockuser_form, "block", 1);
            break;
        }

        case BTN_UNBLOCKUSER_SUBMIT:
        {
            ProccessUsernameAndModifyEntryBool(blockuser_form, "block", 0);
            break;
        }

        // open 'change pass requirements' tab
        case BTN_PASSREQ:
        {
            EnumChildWindows(hwnd, HideChildWindows, NULL);

            DisplayMap(passreq_form, SW_SHOW);
            break;
        }

        // enable or disable pass requirements for user from 'change pass requirements' tab
        case BTN_ENPASSREQ_SUBMIT:
        {
            ProccessUsernameAndModifyEntryBool(passreq_form, "req", 1);
            break;
        }

        case BTN_DISPASSREQ_SUBMIT:
        {
            ProccessUsernameAndModifyEntryBool(passreq_form, "req", 0);
            break;
        }

        // log out and go to home tab
        case BTN_LOGOUT:
        {
            logged_in_user = "";            
            SendMessage(hwnd, WM_COMMAND, BTN_HOME, lParam);
        }

        }

    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
