// HXFindCatalog.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <Windows.h>

/*

HX_FIND_DATA


typedef void * HXHANDLE;


HXHANDLE
WINAPI
HXCreateFile(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess, //无效 默认读
	_In_ DWORD dwShareMode,		//无效 默认共享读
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,//无效
	_In_ DWORD dwCreationDisposition,//无效
	_In_ DWORD dwFlagsAndAttributes,//无效
	_In_opt_ HXHANDLE hTemplateFile//无效
	);


BOOL
WINAPI
HXReadFile(
	_In_ HXHANDLE hFile,
	_Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToRead,
	_Out_opt_ LPDWORD lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped//无效
	);

HXHANDLE
WINAPI
HXFindFirstFileW(
	_In_ LPCWSTR lpFileName,
	_Out_ LPHX_FIND_DATA lpFindFileData
	);
	   	 
BOOL
WINAPI
HXFindNextFile(
	_In_ HXHANDLE hFindFile,
	_Out_ LPWIN32_FIND_DATAW lpFindFileData
	);
*/

//
//void HXFindCatalog()
//{
//	std::list<std::wstring>  fileStringList;
//	WIN32_FIND_DATA fileData;//文件的数据结构，里面保存着文件名
//				//查找第一个文件
//	HANDLE hFileHandle = FindFirstFile(L"F:\\*.*",//待查找的路径文件夹名字,*.*表示通配符，什么文件都可以
//		&fileData);
//
//	fileStringList.push_back(fileData.cFileName);
//	while (FindNextFile(hFileHandle, &fileData))
//	{
//		fileStringList.push_back(fileData.cFileName);
//	}
//
//	FindClose(hFileHandle);
//}
//
//void HXOpenFile()
//{
//	//CreateFileW(
//	//	_In_ LPCWSTR lpFileName,
//	//	_In_ DWORD dwDesiredAccess,
//	//	_In_ DWORD dwShareMode,
//	//	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//	//	_In_ DWORD dwCreationDisposition,
//	//	_In_ DWORD dwFlagsAndAttributes,
//	//	_In_opt_ HANDLE hTemplateFile
//	//);
//	HANDLE hFile = CreateFile(L"\\\\.\\F:",
//		GENERIC_READ,
//		FILE_SHARE_READ,
//		NULL,
//		OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//
//
//	if (INVALID_HANDLE_VALUE == hFile)
//	{
//		int x = GetLastError();
//		return;
//	}
//
//	int file_size = 0;
//	file_size = GetFileSize(hFile, NULL);
//	char *buff;
//	buff = (char*)malloc(file_size);
//	DWORD dwRead;
//	if (!ReadFile(hFile, buff, file_size, &dwRead, NULL))
//	{
//		int x = GetLastError();
//		return;
//	}
//	buff[file_size] = '\0';
//	printf("content:%s\n", buff);
//	CloseHandle(hFile);
//	return;
//}
#include <string>

void HXCreatTxt(std::wstring str, UINT64 i32Size)
{
	std::ofstream fout;
	char szTmp[] = "qwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiopqwertyuiop";
	int iSize = sizeof(szTmp);
	for (size_t j = 0; j < i32Size; j++)
	{
		std::wstring strFile = str;
		strFile += std::to_wstring(j);
		strFile += L".txt";
		fout.open(strFile);
		fout.write(szTmp, iSize);
		fout.close();
		//fout.open(L"G:\\1w\\1.txt");
		//for (size_t i = 0; i < 10000; i++)
		//{
		//	fout.write(szTmp, iSize);
		//}
		//fout.close();
		//fout.open(L"G:\\1w\\2.txt");
		//for (size_t i = 0; i < 10000; i++)
		//{
		//	fout.write(szTmp, iSize);
		//}
		//fout.close();
	}


	std::cout << "Hello World!\n";

}
int main()
{
	std::wstring str;
	//str = L"E:\\1w\\";
	//HXCreatTxt(str, 10000);


	//str = L"E:\\10w\\";
	//HXCreatTxt(str, 10*10000);

	str = L"E:\\30w\\";
	HXCreatTxt(str, 300000);
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
