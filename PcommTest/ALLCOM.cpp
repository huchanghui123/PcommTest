// ALLCOM.cpp: 实现文件
//

#include "pch.h"
#include "PcommTest.h"
#include "ALLCOM.h"
#include "afxdialogex.h"
#include "PCOMM.H"
#include "comm.h"
#include "MXTOOL.H"
#include <vector>
using std::vector;

// ALLCOM 对话框

IMPLEMENT_DYNAMIC(ALLCOM, CDialogEx)

ALLCOM::ALLCOM(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

ALLCOM::~ALLCOM()
{
}

void ALLCOM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL ALLCOM::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_SHOWWINDOW && wParam)
	{
		UpdateUI();
	}
	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

BEGIN_MESSAGE_MAP(ALLCOM, CDialogEx)
	ON_MESSAGE_VOID(WM_CLOSE, MyClose)
	ON_MESSAGE(WM_ALLCOM_DISPLAY_CHANGE, OnDisplayChange)
END_MESSAGE_MAP()

int COMCOUNT = 1;

// ALLCOM 消息处理程序
vector<CEdit*> editVector;
vector<CStatic*> staticVector;
vector<CMFCButton*> ctsVector;
vector<CMFCButton*> dsrVector;
vector<CMFCButton*> dcdVector;
vector<CMFCButton*> riVector;

void ALLCOM::ShowTitleInfo()
{
	TCHAR	szMessage[70];
	TCHAR	szbuf[20];

	lstrcpy(szMessage, GszAppName);
	lstrcat(szMessage, _T(","));

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
	
	SetWindowText(szMessage);
}

static UINT WriteCommThread(LPVOID lpParam)
{
	int port;
	char wbuf[] = "COMX";
	while (GbOpen)
	{
		for (int i = 0; i < COMCOUNT; i++)
		{
			port = i + 1;
			wbuf[3] = '0' + port;
			sio_write(port, wbuf, 5);
		}
		Sleep(20);
	}
	return 0;
}

static UINT ReadCommThread(LPVOID lpParam)
{
	ALLCOM* mdlg = (ALLCOM*)lpParam;
	while (GbOpen)
	{
		SendMessageTimeout(mdlg->m_hWnd, WM_ALLCOM_DISPLAY_CHANGE, FALSE,
			NULL, SMTO_BLOCK, 100, NULL);
		Sleep(20);
	}
	return 0;
}

void ALLCOM::UpdateUI(void)
{
	COMCOUNT = GCommData.MAXCOM;

	CRect cr;
	GetClientRect(&cr); //获取对话框客户区域大小
	ClientToScreen(&cr); //转换为荧幕坐标
	int x = GetSystemMetrics(SM_CXSCREEN); 
	int y = GetSystemMetrics(SM_CYSCREEN); 
	//设置Diglog宽高和位置
	int cwidth = COMCOUNT * (COMWIDTH + 10);
	MoveWindow((x - cwidth) / 2, cr.top, cwidth - 15, 300);
	//标题栏信息
	ShowTitleInfo();
	//字体设置
	CFont *myFont = new CFont();
	myFont->CreateFont(18, 0, 0, 0, FW_HEAVY, true, false,
		0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		FIXED_PITCH | FF_MODERN, _T("Courier New"));
	//创建数据接收区
	for (int i=0;i<COMCOUNT;i++)
	{
		//文本区
		CEdit* pEdit = new CEdit;
		pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL,
			CRect(i * COMWIDTH + 10, 30, i * COMWIDTH + COMWIDTH, 210), this, i + 100);
		pEdit->SetFont(myFont);
		editVector.push_back(pEdit);

		CStatic* pCStatic = new CStatic;
		TCHAR buf[10];
		wsprintf(buf, _T("COM%d"), i + 1);
		pCStatic->Create(buf, WS_CHILD | WS_VISIBLE,
			CRect(i * COMWIDTH + 10, 10, i * COMWIDTH + COMWIDTH, 30), this, i + 200);
		staticVector.push_back(pCStatic);
		//CTS状态区
		CMFCButton* ctsBtn = new CMFCButton;
		ctsBtn->Create(_T("CTS"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(i * COMWIDTH + 10, 215, i * COMWIDTH + 45, 235), this, i + 300);
		ctsBtn->EnableWindow(FALSE);
		ctsBtn->m_bGrayDisabled = FALSE;
		ctsBtn->m_bDontUseWinXPTheme = TRUE;
		ctsBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		ctsBtn->SetTextColor(RGB(0, 0, 0));
		ctsVector.push_back(ctsBtn);
		//DTR状态区
		CMFCButton* dsrBtn = new CMFCButton;
		dsrBtn->Create(_T("DTR"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(i * COMWIDTH + 50, 215, i * COMWIDTH + 85, 235), this, i + 400);
		dsrBtn->EnableWindow(FALSE);
		dsrBtn->m_bGrayDisabled = FALSE;
		dsrBtn->m_bDontUseWinXPTheme = TRUE;
		dsrBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		dsrBtn->SetTextColor(RGB(0, 0, 0));
		dsrVector.push_back(dsrBtn);
		//DCD状态区
		CMFCButton* dcdBtn = new CMFCButton;
		dcdBtn->Create(_T("DCD"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(i * COMWIDTH + 90, 215, i * COMWIDTH + 125, 235), this, i + 500);
		dcdBtn->EnableWindow(FALSE);
		dcdBtn->m_bGrayDisabled = FALSE;
		dcdBtn->m_bDontUseWinXPTheme = TRUE;
		dcdBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		dcdBtn->SetTextColor(RGB(0, 0, 0));
		dcdVector.push_back(dcdBtn);
		//RI状态区
		CMFCButton* riBtn = new CMFCButton;
		riBtn->Create(_T("RI"), WS_CHILD | WS_VISIBLE | WS_BORDER,
			CRect(i * COMWIDTH + 130, 215, i * COMWIDTH + 165, 235), this, i + 600);
		riBtn->EnableWindow(FALSE);
		riBtn->m_bGrayDisabled = FALSE;
		riBtn->m_bDontUseWinXPTheme = TRUE;
		riBtn->m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
		riBtn->SetTextColor(RGB(0, 0, 0));
		riVector.push_back(riBtn);
	}
	delete myFont;

	GbOpen = OpenPort();
	if (GbOpen)
	{
		CWinThread *writeThread = AfxBeginThread(WriteCommThread, (LPVOID)this,
			THREAD_PRIORITY_NORMAL, 0, 0, NULL);

		CWinThread *readThread = AfxBeginThread(ReadCommThread, (LPVOID)this,
			THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		
	}
}

BOOL PortSet(int mPort)
{
	int	port = mPort;
	int	mode = GCommData.Parity | GCommData.ByteSize | GCommData.StopBits;
	int	hw = GCommData.Hw ? 3 : 0;	/* bit0 and bit1 */
	int	sw = GCommData.Sw ? 12 : 0;     /* bit2 and bit3 */
	int	ret;
	DWORD tout;
	if ((ret = sio_ioctl(port, GCommData.BaudRate, mode)) != SIO_OK) {
		MxShowError(_T("sio_ioctl"), ret);
		return FALSE;
	}
	if ((ret = sio_flowctrl(port, hw | sw)) != SIO_OK) {
		MxShowError(_T("sio_flowctrl"), ret);
		return FALSE;
	}

	if ((ret = sio_DTR(port, (GCommData.Dtr ? 1 : 0))) != SIO_OK) {
		MxShowError(_T("sio_DTR"), ret);
		return FALSE;
	}
	if (!GCommData.Hw) {
		if ((ret = sio_RTS(port, (GCommData.Rts ? 1 : 0))) != SIO_OK) {
			MxShowError(_T("sio_RTS"), ret);
			return FALSE;
		}
	}
	tout = 1000 / sio_getbaud(port);  /* ms/byte */
	if (tout < 1)
		tout = 1;
	tout = tout * 1 * 3;             /* 1 byte; '*3' is for delay */
	if (tout < 100)
		tout = 100;
	sio_SetWriteTimeouts(port, tout);
	//sio_SetReadTimeouts(port, tout, 0);
	return TRUE;
}

BOOL ALLCOM::OpenPort(void)
{
	for (int i=0;i<COMCOUNT;i++)
	{
		int port = i + 1;
		int ret;
		if ((ret = sio_open(port)) != SIO_OK)
		{
			MxShowError(_T("sio_open"), ret);
			return FALSE;
		}
		if (!(PortSet(port)))
		{
			sio_close(port);
			return FALSE;
		}
	}
	
	return TRUE;
}

LRESULT ALLCOM::OnDisplayChange(WPARAM wParam, LPARAM lParam)
{
	int port;
	//char wbuf[]="COMX";
	char rbuf[5] = {0};
	CString text;

	for (int i = 0; i < COMCOUNT; i++)
	{
		port = i + 1;
		//sprintf(wbuf, "COM%d", port);
		//wbuf[3] = '0' + port;
		//sio_write(port, wbuf, 5);
		
		int len = 0;
		len = sio_read(port, rbuf, 5);
		if (len > 0)
		{
			text = rbuf;
			if (editVector[i])
			{
				int length = editVector[i]->GetWindowTextLengthW();
				editVector[i]->SetSel(length, length);
				editVector[i]->ReplaceSel(text);
			}
		}
		ShowLStatus(i);
	}
	return 0;
}

void ALLCOM::ShowLStatus(int i)
{
	int ret = sio_lstatus(i + 1);
	if (ret >= 0)
	{
		if (ret&S_CTS)
		{
			ctsVector[i]->SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			ctsVector[i]->SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_DSR)
		{
			dsrVector[i]->SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			dsrVector[i]->SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_RI)
		{
			riVector[i]->SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			riVector[i]->SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		if (ret&S_CD)
		{
			dcdVector[i]->SetFaceColor(RGB(117, 249, 77), TRUE);
		}
		else
		{
			dcdVector[i]->SetFaceColor(RGB(204, 204, 204), TRUE);
		}
		
	}
}

static UINT CloseCOMThread(LPVOID lpParam)
{
	for (int i = 0; i < COMCOUNT; i++)
	{
		sio_close(i + 1);
	}
	return 0;
}

void ALLCOM::MyClose()
{
	GbOpen = FALSE;
	
	CWinThread *chThread = AfxBeginThread(CloseCOMThread, (LPVOID)this, 
		THREAD_PRIORITY_NORMAL, 0, 0, NULL);

	for (int i = 0; i < COMCOUNT; i++)
	{
		if (editVector[i])
		{
			delete editVector[i];
			editVector[i] = NULL;
		}
		if (staticVector[i])
		{
			delete staticVector[i];
			staticVector[i] = NULL;
		}
		if (ctsVector[i])
		{
			delete ctsVector[i];
			ctsVector[i] = NULL;
		}
		if (dsrVector[i])
		{
			delete dsrVector[i];
			dsrVector[i] = NULL;
		}
		if (dcdVector[i])
		{
			delete dcdVector[i];
			dcdVector[i] = NULL;
		}
		if (riVector[i])
		{
			delete riVector[i];
			riVector[i] = NULL;
		}
	}
	editVector.swap(vector<CEdit*>());
	staticVector.swap(vector<CStatic*>());
	ctsVector.swap(vector<CMFCButton*>());
	dsrVector.swap(vector<CMFCButton*>());
	dcdVector.swap(vector<CMFCButton*>());
	riVector.swap(vector<CMFCButton*>());
	this->OnClose();
}

