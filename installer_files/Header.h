#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <string>
#include <fstream>
#include <random>

#define AES_BLOCK_SIZE 16



std::string GetRandomString(int n);
BOOL AesInitialization(const char* phrase, int phrLen);
std::string AesEncrypt(std::string& data, std::string& iv);
std::string AesDecrypt(std::string& data, std::string& iv);

// all global vars 
#ifndef __HEADER_H__
#define __HEADER_H__

extern std::wstring FILENAME;
extern std::wstring FILENAME_ENCR;

#endif