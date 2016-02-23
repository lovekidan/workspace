// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlctrls.h>
#include <atlddx.h>
#include <atlmisc.h>
#include <queue>
#include "thread.hpp"
#include "cs.hpp"

using namespace WTL;

#define WM_UPDATE_INFO WM_USER + 1
#define WM_COMPRESS_FINISHED WM_USER + 2


class CAboutDialog :
    public CDialogImpl<CAboutDialog>,
    public CWinDataExchange<CAboutDialog>
{
public:
    enum { IDD = IDD_DIALOG_ABOUT };

    BEGIN_MSG_MAP(CAboutDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCancel)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CAboutDialog)
    END_DDX_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


class CMainDlg :
    public CDialogImpl<CMainDlg>,
    public CWinDataExchange<CMainDlg>,
    public System::Thread::Thread
{
public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
        MESSAGE_HANDLER(WM_UPDATE_INFO, OnUpdateInfo)
        MESSAGE_HANDLER(WM_COMPRESS_FINISHED, OnFinished)
        COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		COMMAND_ID_HANDLER(IDC_BTNOPEN, OnOpen)
		COMMAND_ID_HANDLER(IDC_BTNCOMPRESS, OnCompress)
        COMMAND_ID_HANDLER(IDC_BTNABOUT, OnAbout)
        NOTIFY_HANDLER(IDC_PNG_QUALITY_SLIDER, NM_CUSTOMDRAW, OnNMCustomdrawPngQualitySlider)
        NOTIFY_HANDLER(IDC_JPEG_QUALITY_SLIDER, NM_CUSTOMDRAW, OnNMCustomdrawJpegQualitySlider)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CMainDlg)
        DDX_CONTROL_HANDLE(IDC_LIST, m_list)
        DDX_CONTROL_HANDLE(IDC_PNG_QUALITY_SLIDER, m_pngSlider)
        DDX_CONTROL_HANDLE(IDC_JPEG_QUALITY_SLIDER, m_jpegSlider);
        DDX_TEXT(IDC_EDITPATH, m_strPath);
    END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnUpdateInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnFinished(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	LRESULT OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void DoCompressFile(CString& strFile);

    void DoCompressDir(CString& strDir);

    void DoCompress(CString& strPath);

    void SetStatus(BOOL bStatus);

	LRESULT OnCompress(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    UINT Run();

private:
    WTL::CRichEditCtrl m_list;
    WTL::CTrackBarCtrl m_pngSlider;
    WTL::CTrackBarCtrl m_jpegSlider;
    CString m_strPath;
    System::Thread::CriticalSection m_cs;
    std::queue<CString> m_lstInfo;
    int m_nOptionJPEG;
    int m_nOptionPNG;
    bool m_bActive;
    unsigned long fileCount;
    unsigned long succeedCount;
    unsigned long noNeedCount;
    unsigned long errorCount;
    unsigned long long sizeBefore;
    unsigned long long sizeAfter;
    time_t startTime;

    std::vector<CString> fileTobeProcess;
public:
    LRESULT OnNMCustomdrawPngQualitySlider(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
    LRESULT OnNMCustomdrawJpegQualitySlider(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
};
