#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include<time.h>

#include<ImageCompressLib.h>
#include"CompressTools.h"

LRESULT CAboutDialog::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);
    DoDataExchange(FALSE);

    return TRUE;
}

LRESULT CAboutDialog::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    EndDialog(wID);
    return 0;
}


LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);
    m_pngSlider = CTrackBarCtrl(GetDlgItem(IDC_PNG_QUALITY_SLIDER));
    m_pngSlider.SetRange(0, 100);
    m_pngSlider.SetTicFreq(10);
    m_pngSlider.SetPos(100);
    
    m_jpegSlider = CTrackBarCtrl(GetDlgItem(IDC_JPEG_QUALITY_SLIDER));
    m_jpegSlider.SetRange(0, 100);
    m_jpegSlider.SetTicFreq(10);
    m_jpegSlider.SetPos(75);
    
    m_bActive = false;

    TCHAR strPathFile[MAX_PATH] = { 0 };
    if (!GetModuleFileName(NULL, strPathFile, MAX_PATH))
    {
        return 0;
    }

    TCHAR drive[_MAX_DRIVE] = { 0 };
    TCHAR dir[_MAX_DIR] = { 0 };
    TCHAR fname[_MAX_FNAME] = { 0 };
    TCHAR ext[_MAX_EXT] = { 0 };

    _tsplitpath(strPathFile, drive, dir, fname, ext);

    DoDataExchange(FALSE);

    return TRUE;
}

LRESULT CMainDlg::OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HDROP hDrop = (HDROP)wParam;
    TCHAR strFileName[MAX_PATH];
    DragQueryFile(hDrop, 0, strFileName, MAX_PATH);
    DragFinish(hDrop);
    m_cs.Lock();
    m_strPath = strFileName;
    m_cs.Unlock();
    DoDataExchange(FALSE);
    return 0;
}

LRESULT CMainDlg::OnUpdateInfo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    m_cs.Lock();
    while (m_lstInfo.size())
    {
        CString strPath = m_lstInfo.front();
        m_lstInfo.pop();
        m_list.AppendText(strPath);
    }
    m_cs.Unlock();
    return 0;
}

LRESULT CMainDlg::OnFinished(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){
    CString resStr;
    resStr.Format(_T("\r\n文件数: %d\r\n成功数: %d\r\n不需要: %d\r\n失败数: %d\r\n耗时: %lld秒\r\n压缩前总体积: %llu字节\r\n压缩后总体积: %llu字节\r\n"),
        fileCount, succeedCount, noNeedCount, errorCount, (long long)(time(0) - startTime), sizeBefore, sizeAfter);
    m_cs.Lock();
    m_lstInfo.push(resStr);
    m_cs.Unlock();
    PostMessage(WM_UPDATE_INFO, 0, 0);
    SetStatus(TRUE);
    SetDlgItemText(IDC_STATIC_STATUS, _T("压缩完成"));
    return 0;
}

LRESULT CMainDlg::OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    Terminate();
    WaitForEnd();
    EndDialog(wID);
    return 0;
}

LRESULT CMainDlg::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CFolderDialog dlg;
    dlg.DoModal(this->m_hWnd);
    m_strPath = dlg.GetFolderPath();
    DoDataExchange(FALSE);
    return 0;
}

void CMainDlg::DoCompressDir(CString& strDir)
{
    CFindFile findFile;
    CString strPath = strDir + _T("\\*.*");
    BOOL bFound = findFile.FindFile(strPath);
    while (bFound && !m_bTerminated){
        bFound = findFile.FindNextFile();
        if (findFile.IsDots())
            continue;
        CString strPathName = findFile.GetFilePath();
        if (findFile.IsDirectory()){
            DoCompressDir(strPathName);
        }
        else{
            fileTobeProcess.push_back(strPathName);
        }
    }
}

void CMainDlg::DoCompress(CString& strPath){
    fileTobeProcess.clear();
    if (!strPath.IsEmpty() && strPath.Right(1) == _T("\\")){
        strPath.Delete(strPath.GetLength() - 1);
    }
    CFindFile findFile;
    if (findFile.FindFile(strPath)){
        if (findFile.IsDirectory()){
            DoCompressDir(strPath);
        }
        else{
            fileTobeProcess.push_back(strPath);
        }
    }
    int fileNum = fileTobeProcess.size();

//#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < fileNum; i++){
        if (m_bTerminated)
            break;
        else
            DoCompressFile(fileTobeProcess[i]);
    }

    fileTobeProcess.clear();
    PostMessage(WM_COMPRESS_FINISHED, 0, 0);
}

void CMainDlg::SetStatus(BOOL bStatus)
{
    GetDlgItem(IDC_BTNOPEN).EnableWindow(bStatus);
    GetDlgItem(IDC_EDITPATH).EnableWindow(bStatus);
    m_pngSlider.EnableWindow(bStatus);
    m_jpegSlider.EnableWindow(bStatus);
    if (bStatus)
    {
        SetDlgItemText(IDC_BTNCOMPRESS, _T("开始压缩"));
        m_bActive = false;
    }
    else
    {
        SetDlgItemText(IDC_BTNCOMPRESS, _T("停止压缩"));
        SetDlgItemText(IDC_STATIC_STATUS, _T("正在压缩..."));
        m_bActive = true;
    }
}

LRESULT CMainDlg::OnCompress(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    Terminate();
    WaitForEnd();
    Thread::Destroy();
    while (!m_lstInfo.empty()) m_lstInfo.pop();
    if (m_bActive){
        SetStatus(TRUE);
    }
    else
    {
        m_list.SetWindowText(_T(""));
        SetStatus(FALSE);
        m_cs.Lock();
        DoDataExchange(TRUE);
        m_cs.Unlock();
        Thread::Create(false);
    }
    return 0;
}

LRESULT CMainDlg::OnAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CAboutDialog aboutDlg;
    aboutDlg.DoModal();
    return 0;
}

UINT CMainDlg::Run(){
    m_cs.Lock();
    CString strPath = m_strPath;
    m_nOptionPNG = m_pngSlider.GetPos();
    m_nOptionJPEG = m_jpegSlider.GetPos();
    sizeAfter = sizeBefore = 0;
    fileCount = succeedCount = noNeedCount = errorCount = 0;
    startTime = time(0);
    m_cs.Unlock();
    DoCompress(strPath);
    return 0;
}

void CMainDlg::DoCompressFile(CString& strImg){
    CString strExt = strImg.Right(strImg.GetLength() - strImg.ReverseFind(_T('.')) - 1);
    strExt.MakeLower();
    if (strExt != _T("png") && strExt != _T("jpg") && strExt != _T("gif") && strExt != _T("gft") && strExt != _T("jpeg") && strExt != _T("bmp")){
        return;
    }
    CString strInfo;
    bool compressResult = false;
    bool containErrorStr = false;
    CString strImgOut = strImg + _T(".out");
    IMAGE_TYPE imageType = getImageType(CompressTools::WideCharToMultiBytes(strImg).c_str());
    RECT ninePatchInfo = { 0 };
    switch (imageType){
    case IMAGE_TYPE_JPEG:
        strInfo = _T("(JPEG)");
        compressResult = CompressTools::CompressJPEG(strImg, strImgOut, m_nOptionJPEG);
        break;
    case IMAGE_TYPE_APNG:
        strInfo = _T("(APNG)");
        compressResult = CompressTools::CompressAPNG(strImg, strImgOut, m_nOptionPNG);
        break;
    case IMAGE_TYPE_GFT:
        strInfo = _T("(GFT)");
        compressResult = CompressTools::CompressGFT(strImg, strImgOut, m_nOptionJPEG, m_nOptionPNG, strInfo);
        break;
    case IMAGE_TYPE_COMPLIED_9PNG:
        strInfo = _T("(complied 9 png)");
        compressResult = CompressTools::CompressPNG(strImg, strImgOut, m_nOptionPNG);
        break;
    case IMAGE_TYPE_UNCOMPLIED_9PNG:
        strInfo = _T("(Uncomplied 9 png)");
        compressResult = CompressTools::CompressUnComplied9Png(strImg, strImgOut, m_nOptionPNG, 2, ninePatchInfo);
        break;
    case IMAGE_TYPE_NORMAL_PNG:
        strInfo = _T("(PNG)");
        compressResult = CompressTools::CompressPNG(strImg, strImgOut, m_nOptionPNG);
        break;
    case IMAGE_TYPE_NOT_EXIST:
        strInfo = _T("(Not Exists)");
        break;
    case IMAGE_TYPE_UNKNOWN:
        strInfo = _T("(Unknown)");
        break;
    default:
        strInfo = _T("(Error)");
        break;
    }

    unsigned long long orgSize, outSize;
    orgSize = CompressTools::getFileSize(strImg);
    outSize = CompressTools::getFileSize(strImgOut);

    sizeBefore += orgSize;

    if (compressResult){
        if (orgSize > outSize){
            HANDLE hImgOut = CreateFile(strImgOut, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
            if (INVALID_HANDLE_VALUE != hImgOut){
                CloseHandle(hImgOut);
                DWORD dwAttr = GetFileAttributes(strImg);
                dwAttr = dwAttr & ~FILE_ATTRIBUTE_READONLY;
                SetFileAttributes(strImg, dwAttr);
                DeleteFile(strImg);
                MoveFile(strImgOut, strImg);
            }
            succeedCount++;
            strInfo += _T("(Success)");
            sizeAfter += outSize;
        }
        else{
            DeleteFile(strImgOut);
            noNeedCount++;
            strInfo += _T("(No Need)");
            sizeAfter += orgSize;
        }
    }
    else{
        DeleteFile(strImgOut);
        errorCount++;
        if (!containErrorStr) {
            strInfo += _T("(Error)");
        }
        sizeAfter += orgSize;
    }

    fileCount++;
    m_cs.Lock();
    m_lstInfo.push(strImg + _T(" - ") + strInfo + _T("\r\n"));
    m_cs.Unlock();
    PostMessage(WM_UPDATE_INFO, 0, 0);
    return;
}


LRESULT CMainDlg::OnNMCustomdrawPngQualitySlider(int /*idCtrl*/, LPNMHDR /*pNMHDR*/, BOOL& /*bHandled*/)
{
    //LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    CString text;
    text.Format(_T("%d"), m_pngSlider.GetPos());
    SetDlgItemText(IDC_STATIC_PNG_QUALITY, text);
    return 0;
}


LRESULT CMainDlg::OnNMCustomdrawJpegQualitySlider(int /*idCtrl*/, LPNMHDR /*pNMHDR*/, BOOL& /*bHandled*/)
{
    //LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    CString text;
    text.Format(_T("%d"), m_jpegSlider.GetPos());
    SetDlgItemText(IDC_STATIC_JPEG_QUALITY, text);
    return 0;
}
