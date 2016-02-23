#include "stdafx.h"
#include "CompressTools.h"
#include "GftUtil.h"
#include <ImageCompressLib.h>
#include <locale>
#include <codecvt>

using namespace std;

namespace
{
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> conv;
}

string CompressTools::WideCharToMultiBytes(const wchar_t* str)
{
    return conv.to_bytes(str);
    return conv.to_bytes(str);
}

std::string CompressTools::WideCharToMultiBytes(const std::wstring &str)
{
    return conv.to_bytes(str);
}

wstring CompressTools::WideCharFromMultiBytes(const char* str)
{
    return conv.from_bytes(str);
}

std::wstring CompressTools::WideCharFromMultiBytes(const std::string &str)
{
    return conv.from_bytes(str);
}

void CompressTools::CopyData(HANDLE hDst, HANDLE hSrc)
{
    BYTE pBuffer[512];
    DWORD dwRead = 0;
    ReadFile(hSrc, pBuffer, 512, &dwRead, NULL);
    while (dwRead)
    {
        WriteFile(hDst, pBuffer, dwRead, &dwRead, NULL);
        ReadFile(hSrc, pBuffer, 512, &dwRead, NULL);
    }
}

bool CompressTools::WriteGFT(const CString& strFile, const CString& strImgOut, const DWORD offset){
    DWORD dwAttr = GetFileAttributes(strFile);
    dwAttr = dwAttr & ~FILE_ATTRIBUTE_READONLY;
    SetFileAttributes(strFile, dwAttr);
    HandleHelper<> hFile = CreateFile(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);
    if (!hFile.IsValid()){
        return false;
    }
    HandleHelper<> hImgOut = CreateFile(strImgOut, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    SetFilePointer(hFile, offset, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);
    CopyData(hFile, hImgOut);
    hImgOut.CloseHandle();
    hFile.CloseHandle();
    return true;
}

bool CompressTools::CompressPNG(CString& strImg, CString& strImgOut, int nOption){
    string strInput = WideCharToMultiBytes(strImg);
    string strOutput = WideCharToMultiBytes(strImgOut);
    bool result = false;
    if (PngQuantFile(strInput.c_str(), strOutput.c_str(), nOption)){
        strInput = strOutput;
    }
    result = PngZopfliCompress(strInput.c_str(), strOutput.c_str());
    return result;
}

void CompressTools::DeleteDir(CString str){
    CFindFile finder;
    CString strdel, strdir;
    strdir.Format(_T("%s//*.*"), str);
    BOOL b_finded = (BOOL)finder.FindFile(strdir);
    while (b_finded){
        b_finded = (BOOL)finder.FindNextFile();
        if (finder.IsDots())
            continue;
        strdel = finder.GetFileName();
        if (finder.IsDirectory()){
            strdel = str + "//" + strdel;
            DeleteDir(strdel);
        }
        else{
            strdel = str + "//" + strdel;
            if (finder.IsReadOnly()){
                SetFileAttributes(strdel, GetFileAttributes(strdel)&(~FILE_ATTRIBUTE_READONLY));
            }
            DeleteFile(strdel);
        }

    }
    finder.Close();
    RemoveDirectory(str);
}

unsigned long long CompressTools::getFileSize(CString& fileName){
    HandleHelper<> hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    unsigned long long sizeOfFile = GetFileSize(hFile, NULL);
    hFile.CloseHandle();
    return sizeOfFile;
}

bool CompressTools::CompressAPNG(CString& strImg, CString& strImgOut, int /*nOption*/){
    bool result = false;
    string strInput = WideCharToMultiBytes(strImg);
    string strOutput = WideCharToMultiBytes(strImgOut);
    result = ApngCompress(strInput.c_str(), strOutput.c_str());
    return result;
}

bool CompressTools::CompressJPEG(CString& strImg, CString& strImgOut, int nOption){
    bool result = false;
    string strInput = WideCharToMultiBytes(strImg);
    string strOutput = WideCharToMultiBytes(strImgOut);
    result = CJpegFile(strInput.c_str(), strOutput.c_str(), nOption) != 0;
    return result;
}

bool CompressTools::CompressUnComplied9Png(CString &strImg, CString &strImgOut, int nOption, int outputType, RECT &ninePatch, bool *haveNinePatch){

    bool result = false;
    string strInput = WideCharToMultiBytes(strImg);
    string strOutput = WideCharToMultiBytes(strImgOut);
    int nine[4] = { ninePatch.left, ninePatch.top, ninePatch.right, ninePatch.bottom };
    if (NinePatchOpt(strInput.c_str(), strOutput.c_str(), outputType, nine)){
        ninePatch.left = nine[0];
        ninePatch.top = nine[1];
        ninePatch.right = nine[2];
        ninePatch.bottom = nine[3];
        if (haveNinePatch != NULL)
            *haveNinePatch = true;
        result = PngQuantFile(strOutput.c_str(), strOutput.c_str(), nOption) != 0;
    }
    else{
        result = PngQuantFile(strInput.c_str(), strOutput.c_str(), nOption) != 0;
    }
    if (result && getImageType(strOutput.c_str()) == IMAGE_TYPE_UNCOMPLIED_9PNG){
        result = PngZopfliCompress(strOutput.c_str(), strOutput.c_str());
    }
    else{
        result = PngZopfliCompress(strInput.c_str(), strOutput.c_str());
    }
    return result;
}

bool CompressTools::CompressGFT(const CString &strFile, const CString &strFileOut, int nOptionJPEG, int nOptionPNG, CString &strInfo){
    CopyFile(strFile, strFileOut, false);
    HandleHelper<> hFile = CreateFile(strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (!hFile.IsValid()){
        strInfo += _T("(Not Exist)");
        return false;
    }
    BYTE pHeader[8] = { 0 };
    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    GFTFILEHEADER head;
    DWORD dwRead = 0;
    ReadFile(hFile, &head, sizeof(head), &dwRead, NULL);
    if (!dwRead || head.i32Type != 0x00464754){
        hFile.CloseHandle();
        return false;
    }
    SetFilePointer(hFile, head.i32DataOffset, NULL, FILE_BEGIN);
    ReadFile(hFile, pHeader, 8, &dwRead, NULL);
    if (dwRead < 8){
        hFile.CloseHandle();
        return false;
    }

    RECT ninePath = { 0 };
    bool haveNinePatchInfo = GftUtil::GetNinePathInfo(hFile, ninePath);

    CString strImg = strFile, strImgOut = strFile;
    strImg += _T(".gftorg");
    strImgOut += _T(".gftout");
    HANDLE hImg = CreateFile(strImg, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, 0);
    SetFilePointer(hFile, head.i32DataOffset, NULL, FILE_BEGIN);
    CopyData(hImg, hFile);
    CloseHandle(hImg);
    hFile.CloseHandle();

    bool processResult = false;
    bool ninePatchResult = false;

    if (pHeader[0] == 0xFF && pHeader[1] == 0xD8 && pHeader[2] == 0xFF){
        strInfo += _T("(jpeg)");
        processResult = CompressJPEG(strImg, strImgOut, nOptionJPEG);
    }
    else if (pHeader[0] == 0x89 && pHeader[1] == 0x50 &&
        pHeader[2] == 0x4E && pHeader[3] == 0x47 &&
        pHeader[4] == 0x0D && pHeader[5] == 0x0A &&
        pHeader[6] == 0x1A && pHeader[7] == 0x0A){
        if (haveNinePatchInfo){
            strInfo += _T("(9png)");
            processResult = CompressUnComplied9Png(strImg, strImgOut, nOptionPNG, 1, ninePath, &ninePatchResult);
        }
        else{
            strInfo += _T("(png)");
            processResult = CompressPNG(strImg, strImgOut, nOptionPNG);
        }
    }
    else{
        strInfo += _T("(Unknow)");
        processResult = false;
    }
    if (processResult){
        processResult = WriteGFT(strFileOut, strImgOut, head.i32DataOffset);
        if (ninePatchResult){//save new ninePatch info, or the Gft will be destroyed
            HandleHelper<> hFile = CreateFile(strFileOut, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, 0);
            GftUtil::setNinePathInfo(hFile, ninePath);
            hFile.CloseHandle();
        }
    }

    DeleteFile(strImg);
    DeleteFile(strImgOut);
    return processResult;
}
