// HXTestNtfs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include "../HXNTFSPluginU/HXNTFSPluginU.h"
#include "../HXCommon/IFilePlugin.h"
#include "../HXCommon/HXExplorerCmd.h"


//#define HX_USECONSOLE

struct HXPrint
{
	HXPrint():m_i32Index(0), m_pwofstream(nullptr){}

	void SetOut(std::wofstream & wofstream)
	{
		m_pwofstream = &wofstream;
	}
	
	static std::wstring PrintTime(LPSYSTEMTIME pSysTime)
	{
		std::wstring wstr;
		wstr += std::to_wstring(pSysTime->wYear);
		wstr += L"-";
		wstr += std::to_wstring(pSysTime->wMonth);
		wstr += L"-";
		wstr += std::to_wstring(pSysTime->wDay);
		wstr += L" ";
		wstr += std::to_wstring(pSysTime->wHour);
		wstr += L":";
		wstr += std::to_wstring(pSysTime->wMinute);
		wstr += L":";
		wstr += std::to_wstring(pSysTime->wSecond);

		return wstr;
	}

	static void Print(INT32 m_i32Index, std::wofstream & wofstream, LPHXFileInfo pFileInfo)
	{
		std::wstring wstrLog;
		wstrLog += L"序号:";
		wstrLog += std::to_wstring(m_i32Index++);
		if (HX_DIRECTORYFLAG & pFileInfo->m_i32FileAttributes)
		{
			wstrLog += L"\nDir :";
		}
		else
		{
			wstrLog += L"\nFile:";
		}
		wstrLog += pFileInfo->m_szFileName;
		wstrLog += L"\nCreateTime:";
		SYSTEMTIME sysTime;
		FileTimeToSystemTime((FILETIME*)&(pFileInfo->m_u64CreateTime), &sysTime);
		//GetLocalTime(&sysTime);
		wstrLog += PrintTime(&sysTime);
		wstrLog += L"\nModifyTime:";
		FileTimeToSystemTime((FILETIME*)&(pFileInfo->m_u64ModifyTime), &sysTime);
		GetLocalTime(&sysTime);
		wstrLog += PrintTime(&sysTime);
		wstrLog += L"\nFile Real Size:";
		wstrLog += std::to_wstring(pFileInfo->m_u64FileRealSize);
		wstrLog += L"(Byte)";
		wstrLog += L"\nFile Size(Space Size):";
		wstrLog += std::to_wstring(pFileInfo->m_u64FileSpaceSize);
		wstrLog += L"(Byte)";
		if (wofstream.is_open())
		{
			wofstream << wstrLog << std::endl << std::endl;
		}
		else
		{
			std::wcout << wstrLog << std::endl << std::endl;
		}
	}

	void operator()(LPHXFileInfo pFileInfo) { 
		std::wstring wstrLog;
		wstrLog += L"序号:";
		wstrLog += std::to_wstring(m_i32Index++);
		if (HX_DIRECTORYFLAG & pFileInfo->m_i32FileAttributes)
		{
			wstrLog += L"\nDir :";
		}
		else
		{
			wstrLog += L"\nFile:";
		}
		wstrLog += pFileInfo->m_szFileName;
		wstrLog += L"\nCreateTime:";
		SYSTEMTIME sysTime;
		FileTimeToSystemTime((FILETIME*)&(pFileInfo->m_u64CreateTime), &sysTime);
		//GetLocalTime(&sysTime);
		wstrLog += PrintTime(&sysTime);
		wstrLog += L"\nModifyTime:";
		FileTimeToSystemTime((FILETIME*)&(pFileInfo->m_u64ModifyTime), &sysTime);
		GetLocalTime(&sysTime);
		wstrLog += PrintTime(&sysTime);
		wstrLog += L"\nFile Real Size:";
		wstrLog += std::to_wstring(pFileInfo->m_u64FileRealSize);
		wstrLog += L"(Byte)";
		wstrLog += L"\nFile Size(Space Size):";
		wstrLog += std::to_wstring(pFileInfo->m_u64FileSpaceSize);
		wstrLog += L"(Byte)";
		if (m_pwofstream && m_pwofstream->is_open())
		{
			(*m_pwofstream) << wstrLog << std::endl << std::endl;
		}
		else
		{
			std::wcout << wstrLog << std::endl << std::endl;
		}
	}

private:
	INT32	m_i32Index;
	std::wofstream * m_pwofstream;
};

void ClearVecFileInfo(VecFileInfo & listFileInfo)
{
	std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), [](LPHXFileInfo pFileInfo) {
		delete pFileInfo; pFileInfo = nullptr;	
	});

	listFileInfo.clear();
}
//
//int HXWriteFile(std::wstring strFile, LPBYTE pBuffer, UINT32 u32Size)
//{
//	HANDLE hFILE = CreateFile(strFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ| FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFILE == INVALID_HANDLE_VALUE)
//	{
//		printf("CreateFile error\n");
//		return 0;
//	}
//	if (SetFilePointer(hFILE, 0, NULL, FILE_END) == -1)
//	{
//		printf("SetFilePointer error\n");
//		return 0;
//	}
//	DWORD dwWrite;
//	if (!WriteFile(hFILE, &pBuffer, u32Size, &dwWrite, NULL))
//	{
//		printf("WriteFile error\n");
//		return 0;
//	}
//	//printf("write %d.\n", dwWrite);
//	//printf("done.\n");
//	CloseHandle(hFILE);
//}
//
//int HXReadFile(std::wstring strFile)
//{
//	HANDLE hFile = CreateFile(strFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//	if (hFile == INVALID_HANDLE_VALUE)
//	{
//		printf("CreateFile error.\n");
//		return 0;
//	}
//	int file_size = 0;
//	file_size = GetFileSize(hFile, NULL);
//	char* buff;
//	buff = (char*)malloc(file_size);
//	DWORD dwRead;
//	if (!ReadFile(hFile, buff, file_size, &dwRead, NULL))
//	{
//		printf("ReadFile error.\n");
//		return 0;
//	}
//	buff[file_size] = '\0';
//	CloseHandle(hFile);
//}

int main()
{
	std::locale::global(std::locale(""));
	IFilePlugin * pFile = Instance();

	//HXReadFile(L"G:\\1.txt");
#ifndef HX_USECONSOLE

	CHXFindDirectory dir;

	/* 目前遍历  还未使用按页读取
	dir.m_u64CurrentPos = 0;
	dir.i32PageSize = 100;
	*/
	/*传入目录只能为G:、G:\\文件夹、G:\\文件夹\\文件夹 不能为G:\\、G:\\文件夹\\ */
	VecFileInfo listFileInfo;
	HXPrint hxprint;
	std::wofstream ofstreamLog;
	std::wstring str;
	str += L"./hx";
	str += L".txt";
	ofstreamLog.open(str);
	if (ofstreamLog.is_open())
	{
		hxprint.SetOut(ofstreamLog);
		dir.m_wstrDirectory = L"G:\\";
		pFile->OnCommand(HX_FILEPLUGIN_READCATALOG, &dir, &listFileInfo);
		ofstreamLog << dir.m_wstrDirectory << std::endl;
		std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), hxprint);
		ofstreamLog << L"======================================================\n" << std::endl;
		ClearVecFileInfo(listFileInfo);
		ofstreamLog.close();
	

		//dir.m_wstrDirectory = L"G:\\索引根";
		//pFile->OnCmd(HX_FILEPLUGIN_READCATALOG, &dir, &listFileInfo);
		//ofstreamLog << dir.m_wstrDirectory << std::endl;
		//std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), hxprint);
		//ofstreamLog << L"======================================================\n" << std::endl;
		//ClearVecFileInfo(listFileInfo);

		//dir.m_wstrDirectory = L"G:\\索引根\\文件夹";
		//pFile->OnCmd(HX_FILEPLUGIN_READCATALOG, &dir, &listFileInfo);
		//ofstreamLog << dir.m_wstrDirectory << std::endl;
		//std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), hxprint);
		//ofstreamLog << L"======================================================\n" << std::endl;
		//ClearVecFileInfo(listFileInfo);

		LPHXReadFileInfo pReadFileInfo = new CHXReadFileInfo();
		pReadFileInfo->m_wstrDirectory = L"G:\\Everything.exe";
		pReadFileInfo->m_u64CurPos = 0;
		// 需为一个扇区的整数倍
		pReadFileInfo->m_u64ReadSize = 4096;
		pReadFileInfo->m_u64RealReadSize = 0;
		pReadFileInfo->m_pBuffer = new BYTE[pReadFileInfo->m_u64ReadSize];

		std::ofstream ofBackFile;
		ofBackFile.open(L"Everything.exe");
		pFile->OnCommand(HX_FILEPLUGIN_READFILE, pReadFileInfo, nullptr);
		int  i32 = 0;
		while (0 != pReadFileInfo->m_u64RealReadSize)
		{
			i32++;
			ofBackFile.write((char*)pReadFileInfo->m_pBuffer, pReadFileInfo->m_u64RealReadSize);
			pReadFileInfo->m_u64CurPos += pReadFileInfo->m_u64RealReadSize;
			//pReadFileInfo->m_u64ReadSize = 1024;
			pReadFileInfo->m_u64RealReadSize = 0;
			pFile->OnCommand(HX_FILEPLUGIN_READFILE, pReadFileInfo, nullptr);
		}
		ofBackFile.close();

		std::wcout << L"输出结束 输出到：" << L"hx2.txt" << std::endl;
	}


#else 
	std::wcout << L"请输入盘符:\n" << std::endl;
	std::wstring strDir;
	std::wcin >> strDir;
	CHXFindDirectory dir;
	dir.wstrDirectory = strDir;
	VecFileInfo listFileInfo;
	pFile->OnCommand(HX_FILEPLUGIN_READCATALOG, &dir, &listFileInfo);
	std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), HXPrint());
	std::wcout << L"======================================================\n" << std::endl;
	INT32	i32Index = 0;

	while (1)
	{
		std::wcout << L"请选择文件夹:\n" << std::endl;
		std::wcin >> i32Index;
		if (-1 == i32Index)
		{
			break;
		}
		if (i32Index > (listFileInfo.size() - 1) || i32Index < -1)
		{
			std::wcout << L"序号错误！！！" << std::endl;
			std::wcout << L"======================================================\n" << std::endl;
			continue;
		}

		if (!(listFileInfo[i32Index]->m_i32FileAttributes & HX_DIRECTORYFLAG))
		{
			std::wcout << L"非文件夹！！！" << std::endl;
			std::wcout << L"======================================================\n" << std::endl;
			continue;
		}

		dir.wstrDirectory += L"\\";
		dir.wstrDirectory += listFileInfo[i32Index]->m_szFileName;
		ClearVecFileInfo(listFileInfo);
		pFile->OnCommand(HX_FILEPLUGIN_READCATALOG, &dir, &listFileInfo);
		std::for_each(listFileInfo.cbegin(), listFileInfo.cend(), HXPrint());
		std::wcout << L"======================================================\n" << std::endl;

	}
#endif // !HX_USECONSOLE

	{
		std::wstring strDir;
		std::wcin >> strDir;
	}
}

