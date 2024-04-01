#include "pch.h"
#include "CppUnitTest.h"
#include "../HXNTFSPluginU/HXNTFSPluginU.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HXNTFSPluginTest
{
	TEST_CLASS(HXNTFSPluginTest)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			std::wstring strFileName = L"";
			HX_FIND_DATA hxFindData;

			HXHANDLE hHanlde = HXFindFirstFile(strFileName, &hxFindData);
			
		}
	};
}
