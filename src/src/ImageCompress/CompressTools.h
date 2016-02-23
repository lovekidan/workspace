#pragma once

#include <atlstr.h>
#include <string>

namespace CompressTools{
	template<HANDLE hInvalidValue = INVALID_HANDLE_VALUE>
	class HandleHelper{
	public:
		HandleHelper(HANDLE handle)
			: handle_(handle)
		{ }

		~HandleHelper(){
			CloseHandle();
		}

		void CloseHandle()throw(){
			if (IsValid()){
				::CloseHandle(handle_);
				handle_ = hInvalidValue;
			}
		}

		operator HANDLE()const{
			return handle_;
		}

		bool IsValid()const{
			return handle_ != hInvalidValue;
		}


	private:
		HANDLE handle_;
	};


	typedef struct tagGFTFILEHEADER {
		DWORD   i32Type;
		WORD    i16Version;
		WORD    i16Reserved;
		DWORD   i32Reserved;
		WORD    i16TextureMode;
		WORD    i16BlendMode;
		DWORD   i32DataOffset;
	} GFTFILEHEADER;

    std::string WideCharToMultiBytes(const wchar_t* str);
    std::string WideCharToMultiBytes(const std::wstring &str);
    std::wstring WideCharFromMultiBytes(const char* str);
    std::wstring WideCharFromMultiBytes(const std::string &str);

	void CopyData(HANDLE hDst, HANDLE hSrc);
	bool WriteGFT(const CString& strFile, const CString& strImgOut, const DWORD offset);
    bool CompressPNG(CString& strImg, CString& strImgOut, int nOption = 100);
	void DeleteDir(CString str);
	bool CompressAPNG(CString& strImg, CString& strImgOut, int nOption);
    bool CompressJPEG(CString& strImg, CString& strImgOut, int nOption = 75);
    bool CompressUnComplied9Png(CString &strImg, CString &strImgOut, int nOption, int outputType, RECT &ninePatch, bool *haveNinePatch = NULL);
    bool CompressGFT(const CString &strFile, const CString &strFileOut, int nOptionJPEG, int nOptionPNG, CString &strInfo);
    unsigned long long getFileSize(CString& fileName);
}