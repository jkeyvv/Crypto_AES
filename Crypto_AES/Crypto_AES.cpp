#include "pch.h"
#include "aes.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>

#pragma comment(lib, "libcrypto.lib")

#define BUFLEN 16

using namespace std;

//加密函数，接受三个参数，分别是密码、输入文件流、输出文件流
void enc(const string &password, ifstream &infileStream, ofstream &outFileStream)
{
	unsigned char aes_keybuf[32];
	AES_KEY aeskey;
	// 准备32字节(256位)的AES密码字节
	memset(aes_keybuf, 0x90, 32);

	int len = password.size() < 32 ? password.size() : 32;
	for (int i = 0; i < len; i++)
		aes_keybuf[i] = password[i];

	AES_set_encrypt_key(aes_keybuf, 256, &aeskey);

	while (infileStream)
	{
		unsigned char buf[BUFLEN];
		unsigned char buf2[BUFLEN];
		memset(buf, 0, 16);
		memset(buf2, 0, 16);

		infileStream.read((char *)buf, BUFLEN);
		auto inCount = infileStream.gcount();
		AES_encrypt(buf, buf2, &aeskey);
		outFileStream.write((char *)buf2, BUFLEN);

		if (inCount != BUFLEN)
		{
			//将最后一次读取的字节数放在末尾，这是最后一次加密时，有效数据的个数
			outFileStream.put(inCount);
		}

	}
}

//解密函数，参数同加密函数
void dec(const string &passwd, ifstream &inFileStream, ofstream &outFileStream)
{
	unsigned char aes_keybuf[32];
	AES_KEY aeskey;

	// 准备32字节(256位)的AES密码字节
	memset(aes_keybuf, 0x90, 32);
	int len = passwd.size() < 32 ? passwd.size() : 32;
	for (int i = 0; i < len; i++)
		aes_keybuf[i] = passwd[i];

	// 密文串的解密
	AES_set_decrypt_key(aes_keybuf, 256, &aeskey);

	unsigned char buf[BUFLEN];
	unsigned char buf2[BUFLEN];
	memset(buf, 0, 16);
	memset(buf2, 0, 16);

	inFileStream.seekg(-1, inFileStream.end);//将当前定位到文件末端-1
	char lastInCount = 0;
	inFileStream.get(lastInCount);//取出最后一次加密中有效的数据的个数
	inFileStream.seekg(0, inFileStream.beg);//重新指向文件开始

	while (inFileStream)
	{
		inFileStream.read((char *)buf, BUFLEN);
		AES_decrypt(buf, buf2, &aeskey);

		inFileStream.read((char *)buf, BUFLEN);
		auto inCount = inFileStream.gcount();

		if (inCount == 1)
		{
			//将最后一次加密时的有效数据输出，数目是lastInCount个字节
			outFileStream.write((char *)buf2, lastInCount);
		}
		if (inCount == BUFLEN)
		{
			inFileStream.seekg(0 - BUFLEN, ios::cur);
			outFileStream.write((char *)buf2, BUFLEN);
		}
	}
}

int main(int argc, char* argv[])
{
	while (true)
	{
		string mode;
		string inFileName;
		string outFileName;
		ifstream inFileStream;
		ofstream outFileStream;
		string password;

		cout << "***********************************************" << endl;
		cout << "请输入参数，格式为："
			<< "参数1(enc or dec) "
			<< "参数2(加密文件/解密文件文件名) "
			<< "参数3(加密/解密密码) "
			<< endl;

		cin >> mode >> inFileName >> password;

		inFileStream.open(inFileName, ifstream::binary | ifstream::in);
		if (!inFileStream.is_open())
		{
			cout << "文件打开出错！" << endl;
			continue;
		}

		if (mode == "enc")//加密
		{
			outFileName = "enc_" + inFileName;
			outFileStream.open(outFileName, ofstream::out | ofstream::binary);
			cout << "开始加密..." << endl;
			enc(password, inFileStream, outFileStream);
			cout << "加密完成，加密后的文件是：" << outFileName << endl;
		}
		else if (mode == "dec")//解密
		{
			outFileName = "dec_" + inFileName;
			outFileStream.open(outFileName, ofstream::out | ofstream::binary);
			cout << "开始解密..." << endl;
			dec(password, inFileStream, outFileStream);
			cout << "解密完成，解密后的文件是：" << outFileName << endl;
		}
		else
		{
			cout << "参数1输入不合法！" << endl;
		}

		inFileStream.close();
		outFileStream.close();

	}
	return 0;
}