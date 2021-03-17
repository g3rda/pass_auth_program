#include "header.h"

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

// get random string of chars and letters of length 4
std::string GetRandomString()
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 4);
}

// convert bool value to string
const char* const BoolToString(bool b)
{
    return b ? "true" : "false";
}
