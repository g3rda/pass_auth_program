#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <string>

using namespace std;

const int USERLEN = 30;

// gather data about the user and computer
wstring GatherData() {

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

	wstring result = wstring(&username[0])+wstring(&computername[0])+wstring(&windowsdirectory[0])+wstring(&systemdirectory[0])+to_wstring(mouse_buttons)+to_wstring(screen_height)+to_wstring(memory)+wstring(&volumename[0]);

	return result;
}


int main() {

	// gather data for hashing
	wstring data = GatherData();
	if (data == L"error"){
		return 1;
	}
	
	
	HKEY  hkResult;
	HCRYPTPROV hProv;
	HCRYPTKEY hKey;
	HCRYPTHASH hHash;
	BYTE* pbSignature;
	DWORD dwBlobLen;
	DWORD dwSigLen = 0;
	string s(data.begin(), data.end());
	const BYTE* bytes_data = reinterpret_cast<const BYTE*>(s.c_str());
	
	//cout << s << endl;


	// Acquire a cryptographic provider context handle.

	if (CryptAcquireContext(&hProv, NULL,NULL,PROV_RSA_FULL,0))
	{
		cout << ("CSP context acquired.\n");
	}
	else
	{
		cout<<("Error during CryptAcquireContext.");
	}



	// Get the public at signature key. 

	if (CryptGetUserKey(
		hProv,
		AT_SIGNATURE,
		&hKey))
	{
		cout << ("The signature key has been acquired. \n");
	}
	else
	{
		cout << ("Error during CryptGetUserKey for signkey, creating key.");
		CryptGenKey(hProv, AT_SIGNATURE, NULL, &hKey);
	}
	

	// Create the hash object.

	if (CryptCreateHash(
		hProv,
		CALG_MD5,
		0,
		0,
		&hHash))
	{
		cout << ("Hash object created. \n");
	}
	else
	{
		cout << ("Error during CryptCreateHash.");
	}



	// Compute the cryptographic hash of the buffer.

	if (CryptHashData(
		hHash,
		bytes_data,
		s.length() + 1,
		0))
	{
		cout << ("The data buffer has been hashed.\n");
	}
	else
	{
		cout << ("Error during CryptHashData.");
	}



	// Determine the size of the signature and allocate memory.

	if (CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		NULL,
		&dwSigLen))
	{
		cout << ("Signature length %d found.\n", dwSigLen);
	}
	else
	{
		cout << ("Error during CryptSignHash.");
	}


	// Allocate memory for the signature buffer.

	if (pbSignature = (BYTE*)malloc(dwSigLen))
	{
		cout << ("Memory allocated for the signature.\n");
	}
	else
	{
		cout << ("Out of memory.");
	}


	// Sign the hash object.

	if (CryptSignHash(
		hHash,
		AT_SIGNATURE,
		NULL,
		0,
		pbSignature,
		&dwSigLen))
	{
		cout << ("pbSignature is the hash signature.\n");
	}
	else
	{
		cout << ("Error during CryptSignHash.");
	}

	// Create the registry key
	RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Kostetska",0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkResult, NULL);
	
	// Set value "Signature"
	RegSetValueExA(hkResult, "Signature", 0, REG_BINARY, pbSignature, dwSigLen);
		

	return 0;
}
