#include "Header.h"

// global vars
HCRYPTPROV hProv;
HCRYPTKEY hKeyDecryptEncrypt;

// get random string of chars and letters of length n
std::string GetRandomString(int n)
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, n);
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
        std::cout << GetLastError();
        system("pause");
        return FALSE;
    }

    // Set key params
    DWORD dwMode = CRYPT_MODE_CBC;
    if (CryptSetKeyParam(hKeyDecryptEncrypt, KP_MODE, (BYTE*)&dwMode, 0)) {
        //MessageBoxA(NULL, std::string("Parameters for the key have been set.\n").c_str(), (LPCSTR)"Error", MB_OK);
    }
    else {
        std::cout << GetLastError();
        system("pause");
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
        MessageBoxA(NULL, std::string("Error during encryption.").c_str(), (LPCSTR)"Error", MB_OK);
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
        data.resize(dwDataLen);  // resize to actual ciphertext length
    }

    if (!success) {
        MessageBoxA(NULL, std::string("Error during decryption.").c_str(), (LPCSTR)"Error", MB_OK);
       
        return FALSE;
    }
    return data;
}