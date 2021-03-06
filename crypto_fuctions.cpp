#include "header.h"


// global vars
HCRYPTPROV hProv;
HCRYPTKEY hKeyDecryptEncrypt;

//create md5 hash
std::string MakeHash(const char* data)
{
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE* pbHash = NULL;
    DWORD dwHashLen;

    BYTE* pbBuffer = NULL;
    DWORD dwCount;
    DWORD i;
    unsigned long bufLen = 0;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0)) {
        return "0";
    }
    if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
        return "0";
    }

    bufLen = strlen(data);

    pbBuffer = (BYTE*)malloc(bufLen + 1);
    memset(pbBuffer, 0, bufLen + 1);




    for (i = 0; i < bufLen; i++) {
        pbBuffer[i] = (BYTE)data[i];
    }

    if (!CryptHashData(hHash, pbBuffer, bufLen, 0)) {
        return "0";
    }

    dwCount = sizeof(DWORD);
    if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&dwHashLen, &dwCount, 0)) {
        return "0";
    }
    if ((pbHash = (unsigned char*)malloc(dwHashLen)) == NULL) {
        return "0";
    }


    memset(pbHash, 0, dwHashLen);

    if (!CryptGetHashParam(hHash, HP_HASHVAL, pbHash, &dwHashLen, 0)) {
        return "0";
    }


    if (hHash) CryptDestroyHash(hHash);
    if (hProv) CryptReleaseContext(hProv, 0);

    return StringToHex(std::string((char*)pbHash).substr(0, dwHashLen));
}

// convert any string to its hex representation
std::string StringToHex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

// get random string of chars and letters of length n
std::string GetRandomString(int n)
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, n);
}

// convert bool value to string
const char* const BoolToString(bool b)
{
    return b ? "true" : "false";
}


BOOL AesInitialization(const char* phrase, int phrLen) {

    HCRYPTHASH hHashDecryptEncrypt;

    // Acquire a cryptographic provider context handle.

    if (CryptAcquireContext(&hProv, NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, NULL)) {
        //MessageBoxA(NULL, std::string("CSP context acquired.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        MessageBoxA(NULL, std::string("Error during CryptAcquireContext.").c_str(), (LPCSTR)"Error", MB_OK);
        return 1;
    }
    // Create the hash object.

    if (CryptCreateHash(hProv, CALG_MD4, 0, 0, &hHashDecryptEncrypt))
    {
        // MessageBoxA(NULL, std::string("Hash object created. \n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        MessageBoxA(NULL, std::string("Error during CryptCreateHash.").c_str(), (LPCSTR)"Error", MB_OK);
        return FALSE;
    }


    // Compute the cryptographic hash of the buffer.

    if (CryptHashData(hHashDecryptEncrypt, reinterpret_cast<const BYTE*>(phrase), phrLen + 1, 0)) {
        //MessageBoxA(NULL, std::string("The data buffer has been hashed.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        MessageBoxA(NULL, std::string("Error during CryptHashData.").c_str(), (LPCSTR)"Error", MB_OK);
        return FALSE;
    }

    // Derive the key

    if (CryptDeriveKey(hProv, CALG_AES_256, hHashDecryptEncrypt, CRYPT_NO_SALT, &hKeyDecryptEncrypt)) {
        //MessageBoxA(NULL, std::string("The key has been derived\n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        MessageBoxA(NULL, std::string("Error during CryptDeriveKey.").c_str(), (LPCSTR)"Error", MB_OK);
        return FALSE;
    }

    // Set key params
    DWORD dwMode = CRYPT_MODE_CBC;
    if (CryptSetKeyParam(hKeyDecryptEncrypt, KP_MODE, (BYTE*)&dwMode, 0)) {
        //MessageBoxA(NULL, std::string("Parameters for the key have been set.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        MessageBoxA(NULL, std::string("Error during CryptSetKeyParam.").c_str(), (LPCSTR)"Error", MB_OK);
        return FALSE;
    }

    if (hHashDecryptEncrypt != NULL) CryptDestroyHash(hHashDecryptEncrypt);

    return TRUE;
}


std::string AesEncrypt(std::string& data, std::string& iv)
{
    BOOL success = TRUE;

    if (success) {
        iv.resize(AES_BLOCK_SIZE);  // allocate space for random IV
        success = CryptGenRandom(hProv, AES_BLOCK_SIZE, (BYTE*)&iv[0]);
    }
    if (success) {
        success = CryptSetKeyParam(hKeyDecryptEncrypt, KP_IV, (BYTE*)&iv[0], 0);
    }
    if (success) {
        DWORD dwDataLen = data.length();
        data.resize(dwDataLen + AES_BLOCK_SIZE); // give enough space for padding
        success = CryptEncrypt(hKeyDecryptEncrypt, NULL, TRUE, 0, (BYTE*)&data[0], &dwDataLen, data.length());
        data.resize(dwDataLen);  // resize to actual ciphertext length
    }

    if (!success) {
        MessageBoxA(NULL, std::string("Error during encryption. Enter correct passphrase or exit.").c_str(), (LPCSTR)"Error", MB_OK);
        return FALSE;
    }
    return data;
}

std::string AesDecrypt(std::string& data, std::string& iv)
{
    BOOL success = TRUE;

    if (success) {
        iv.resize(AES_BLOCK_SIZE);  // ensure IV has the correct size
        success = CryptSetKeyParam(hKeyDecryptEncrypt, KP_IV, (BYTE*)&iv[0], 0);
    }
    if (success) {
        DWORD dwDataLen = data.length();
        success = CryptDecrypt(hKeyDecryptEncrypt, NULL, TRUE, 0, (BYTE*)&data[0], &dwDataLen);
        /*void* lpBuffer;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            ::GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR)&lpBuffer,
            0,
            NULL);

        MessageBox(NULL, (LPCTSTR)lpBuffer, TEXT("LastRrror"), MB_OK);
        LocalFree(lpBuffer);*/
        data.resize(dwDataLen);  // resize to actual ciphertext length
    }

    if (!success) {
        MessageBoxA(NULL, std::string("Error during decryption.").c_str(), (LPCSTR)"Error", MB_OK);
        
    }
    return data;
}