#pragma once

//#define HX_FILEPLUGIN				0x0400
//#define HX_FILEPLUGIN_READCATALOG	(HX_FILEPLUGIN+1)
//#define HX_FILEPLUGIN_READFILE		(HX_FILEPLUGIN+2)
//#define HX_FILEPLUGIN_READClose		(HX_FILEPLUGIN+3)

#define _HX_DELETE_PTR_ARRAY_(_Ptr)	{if(nullptr!=_Ptr) delete[] _Ptr;_Ptr=nullptr;}


//	先不做差异处理  使用HXExplorerCmd中HXExplorerPluginCmd
enum class HXFileSystemCmd : uint32_t
{
	HXFileSystemCmd_UnKonwn = 0,
	HXFileSystemCmd_Initialize,
	HXFileSystemCmd_Release,
	HXFileSystemCmd_ReadDirectory,
	HXFileSystemCmd_ReadFile,
	HXFileSystemCmd_ReadClose,
};