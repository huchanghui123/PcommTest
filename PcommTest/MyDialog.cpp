// MyDialog.cpp: 实现文件
//

#include "pch.h"
#include "PcommTest.h"
#include "MyDialog.h"
#include "afxdialogex.h"
#include <atlconv.h>
#include "comm.h"
#include "MXTOOL.h"
#include "PCOMM.H"
#pragma comment(lib, "pcomm.lib")
// MyDialog 对话框

IMPLEMENT_DYNAMIC(MyDialog, CDialogEx)

MyDialog::MyDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

MyDialog::~MyDialog()
{
	
}

void MyDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, sendEdit);
	DDX_Control(pDX, IDC_EDIT2, recevieEdit);
	DDX_Control(pDX, IDC_AUTO, autoCheck);
	CString com;
	com.Format(_T("COM%d"), GCommData.Port);
	SetDlgItemText(IDC_EDIT1, com);
	ShowStatus();
	DDX_Control(pDX, IDC_BUTTON1, ctsState);
	DDX_Control(pDX, IDC_BUTTON3, dsrState);
	DDX_Control(pDX, IDC_BUTTON4, riState);
	DDX_Control(pDX, IDC_BUTTON2, dcdState);
}


BEGIN_MESSAGE_MAP(MyDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &MyDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &MyDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_AUTO, &MyDialog::OnBnClickedAuto)
	//自定义消息映射
	ON_MESSAGE(WM_DISPLAY_CHANGE, OnDisplayChange)
END_MESSAGE_MAP()

void MyDialog::ShowStatus()
{
	TCHAR	szMessage[70];
	TCHAR	szbuf[20];

	lstrcpy(szMessage, GszAppName);
	if (GbOpen) 
	{
		wsprintf(szbuf, _T(" -- COM%d,"), GCommData.Port);
		lstrcat(szMessage, szbuf);
	
		lstrcpy(szbuf, GBaudTableStr[GCommData.ibaudrate]);
		lstrcat(szMessage, szbuf);
		lstrcat(szMessage, _T(","));

		lstrcpy(szbuf, GParityTableStr[GCommData.iparity]);
		lstrcat(szMessage, szbuf);
		lstrcat(szMessage, _T(","));

		lstrcpy(szbuf, GDataBitsTableStr[GCommData.ibytesize]);
		lstrcat(szMessage, szbuf);
		lstrcat(szMessage, _T(","));

		lstrcpy(szbuf, GStopBitsTableStr[GCommData.istopbits]);
		lstrcat(szMessage, szbuf);

		if (GCommData.Hw)
			lstrcat(szMessage, _T(",RTS/CTS"));

		if (GCommData.Sw)
			lstrcat(szMessage, _T(",XON/XOFF"));
	}
	SetWindowText(szMessage);

	ctsState.m_bGrayDisabled = FALSE;
	ctsState.m_bDontUseWinXPTheme = TRUE;
	ctsState.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	ctsState.SetTextColor(RGB(0, 0, 0));

	dsrState.m_bGrayDisabled = FALSE;
	dsrState.m_bDontUseWinXPTheme = TRUE;
	dsrState.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	dsrState.SetTextColor(RGB(0, 0, 0));

	riState.m_bGrayDisabled = FALSE;
	riState.m_bDontUseWinXPTheme = TRUE;
	riState.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	riState.SetTextColor(RGB(0, 0, 0));

	dcdState.m_bGrayDisabled = FALSE;
	dcdState.m_bDontUseWinXPTheme = TRUE;
	dcdState.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
	dcdState.SetTextColor(RGB(0, 0, 0));

	ShowLStatus(GCommData.Port);
}

void MyDialog::ShowLStatus(int port)
{
	int ret = sio_lstatus(port);
	if (ret >= 0)
	{
		if (ret&S_CTS)
		{
			ctsState.SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			ctsState.SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_DSR)
		{
			dsrState.SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			dsrState.SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_RI)
		{
			riState.SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			riState.SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_CD)
		{
			dcdState.SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			dcdState.SetFaceColor(RGB(204, 204, 204), TRUE);
		}
	}
}

// MyDialog 消息处理程序
void MyDialog::OnBnClickedOk()
{
	CString pintStr;
	sendEdit.GetWindowTextW(pintStr);

	char wbuf[30];
	USES_CONVERSION;
	strncpy(wbuf, W2A(pintStr), pintStr.GetLength()+1);

	sio_write(GCommData.Port, wbuf, 30);

	CString text;
	char rbuf[30];

	int len;
	len = sio_read(GCommData.Port, rbuf, 30);
	if (len > 0)
	{
		//USES_CONVERSION;
		//text = A2W(rbuf);
		text = rbuf;
		int nlength = recevieEdit.GetWindowTextLengthW();
		recevieEdit.SetSel(nlength, nlength);
		recevieEdit.ReplaceSel(text);
	}
	ShowLStatus(GCommData.Port);
}

static BOOL autoThread = FALSE;

void MyDialog::OnBnClickedCancel()
{
	autoThread = FALSE;
	GbOpen = FALSE;
	sio_close(GCommData.Port);
	
	CDialogEx::OnCancel();
}

static UINT CommThread(LPVOID lpParam)
{
	MyDialog* mdlg = (MyDialog*)lpParam;
	while (autoThread)
	{
		SendMessageTimeout(mdlg->m_hWnd, WM_DISPLAY_CHANGE, FALSE, 
			NULL, SMTO_BLOCK, 100, NULL);
		Sleep(20);
	}
	//Sleep(20);
	return 0;
}

void MyDialog::OnBnClickedAuto()
{
	int ret = autoCheck.GetCheck();
	if (ret)
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(FALSE);
		CWinThread *hThread = AfxBeginThread(CommThread, (LPVOID)this);
		autoThread = TRUE;
	}
	else
	{
		((CButton*)GetDlgItem(IDOK))->EnableWindow(TRUE);
		autoThread = FALSE;
	}
}


LRESULT MyDialog::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	CString str;
	sendEdit.GetWindowTextW(str);

	char wbuf[30];
	USES_CONVERSION;
	int wlength = str.GetLength() + 1;
	strncpy(wbuf, W2A(str), wlength);

	sio_write(GCommData.Port, wbuf, 30);

	char rbuf[30];
	CString text;
	int rlength = sizeof(rbuf);
	int len;
	len = sio_read(GCommData.Port, rbuf, rlength);
	if (len > 0)
	{
		USES_CONVERSION;
		text = A2W(rbuf);

		int nlength = recevieEdit.GetWindowTextLengthW();
		recevieEdit.SetSel(nlength, nlength);
		recevieEdit.ReplaceSel(text);
	}
	ShowLStatus(GCommData.Port);
	return 0;
}


