
// PcommTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "PcommTest.h"
#include "PcommTestDlg.h"
#include "afxdialogex.h"
#include "comm.h"
#include "PCOMM.H"
#include "MXTOOL.H"
#include "MyDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TCHAR GszAppName[] = _T("Pcomm Demo");

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPcommTestDlg 对话框

CPcommTestDlg::CPcommTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCOMMTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	allComDlg = NULL;
}

void CPcommTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, portComboBox);
	DDX_Control(pDX, IDC_COMBO2, baudComboBox);
	DDX_Control(pDX, IDC_COMBO3, parityComboBox);
	DDX_Control(pDX, IDC_COMBO4, dataComboBox);
	DDX_Control(pDX, IDC_COMBO5, stopComboBox);
}

BEGIN_MESSAGE_MAP(CPcommTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE_VOID(WM_CLOSE, MyClose)
	ON_BN_CLICKED(IDC_BUTTON1, &CPcommTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CPcommTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK1, &CPcommTestDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CPcommTestDlg 消息处理程序

BOOL CPcommTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetWindowText(GszAppName);
	initUI();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPcommTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
		ShellAbout(this->m_hWnd, _T("PcommLib Test"), _T("For bugs, please contact 168380359@qq.com"), NULL);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPcommTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPcommTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define	BAUDCOUNT	10
#define	DATABITCOUNT	4
#define	PARITYCOUNT	5
#define	STOPBITCOUNT	2

CString GBaudTableStr[BAUDCOUNT] = {
	_T("4800"),_T("7200"),_T("9600"),_T("19200"),_T("38400"),_T("57600"),
	_T("115200"),_T("230400"),_T("460800"),_T("921600")
};

CString GDataBitsTableStr[DATABITCOUNT] = {
	_T("5"),_T("6"),_T("7"),_T("8")
};

CString GParityTableStr[PARITYCOUNT] = {
	_T("None"),_T("Even"),_T("Odd"),_T("Mark"),_T("Space")
};

CString GStopBitsTableStr[STOPBITCOUNT] = {
	_T("1"),_T("2")
};

int GBaudTable[BAUDCOUNT] = {
	B4800,B7200,B9600,B19200,B38400,B57600,
	B115200,B230400,B460800,B921600
};

int GDataBitsTable[DATABITCOUNT] = {
	BIT_5,BIT_6,BIT_7,BIT_8
};

int GParityTable[PARITYCOUNT] = {
	P_NONE, P_EVEN, P_ODD, P_MRK, P_SPC
};

int GStopBitsTable[STOPBITCOUNT] = {
	STOP_1,STOP_2
};

BOOL GbOpen = FALSE;
int MAXCOM = 8;

int CPcommTestDlg::GetMaxCom()
{
	CUIntArray arrComPortNo;
	TCHAR szComPort[10];
	HANDLE hCom = NULL;

	for (int i = 1; i <= 10; ++i)
	{
		/*if (i < 10)
			wsprintf(szComPort, _T("COM%d"), i);
		else
			wsprintf(szComPort, _T("\\\\.\\COM%d"), i);*/

		wsprintf(szComPort, _T("COM%d"), i);

		hCom = CreateFile(szComPort,
			GENERIC_READ | GENERIC_WRITE, // desired access should be read&write  
			0,                          // COM port must be opened in non-sharing mode  
			NULL,                       // don't care about the security  
			OPEN_EXISTING,              // IMPORTANT: must use OPEN_EXISTING for a COM port  
			0,                          // usually overlapped but non-overlapped for existance test  
			NULL);                      // always NULL for a general purpose COM port  

		if (INVALID_HANDLE_VALUE == hCom)
		{
			if (ERROR_ACCESS_DENIED == (LONG)hCom)
			{   // then it exists and currently opened  
				arrComPortNo.Add(i);
			}
		}
		else
		{   // COM port exist  
			arrComPortNo.Add(i);
			CloseHandle(hCom);
		}
	}

	int nComportsAvailable = arrComPortNo.GetSize();

	return nComportsAvailable;
}

void CPcommTestDlg::initUI()
{
	GCommData.MAXCOM = GetMaxCom();
	MAXCOM = GCommData.MAXCOM;
	//CString msg;
	//msg.Format(_T("%d"), MAXCOM);
	//AfxMessageBox(msg);
	TCHAR buf[10];
	int com;
	for (com=1;com<=MAXCOM;com++)
	{
		wsprintf(buf, _T("COM%d"), com);
		portComboBox.AddString(buf);
	}
	portComboBox.SetCurSel(0);

	for (int i=0;i< BAUDCOUNT;i++)
	{
		baudComboBox.InsertString(i, GBaudTableStr[i]);
	}
	baudComboBox.SetCurSel(6);
	
	for (int i=0;i<PARITYCOUNT;i++)
	{
		parityComboBox.InsertString(i, GParityTableStr[i]);
	}
	parityComboBox.SetCurSel(0);

	for (int i=0; i<DATABITCOUNT;i++)
	{
		dataComboBox.InsertString(i, GDataBitsTableStr[i]);
	}
	dataComboBox.SetCurSel(DATABITCOUNT-1);
	
	for (int i=0; i<STOPBITCOUNT;i++)
	{
		stopComboBox.InsertString(i, GStopBitsTableStr[i]);
	}
	stopComboBox.SetCurSel(0);

	((CButton*)GetDlgItem(IDC_CHECK3))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(TRUE);
}


COMMDATA GCommData = {};

BOOL PortSet(void)
{
	int	port = GCommData.Port;
	int	mode = GCommData.Parity | GCommData.ByteSize | GCommData.StopBits;
	int	hw = GCommData.Hw ? 3 : 0;	/* bit0 and bit1 */
	int	sw = GCommData.Sw ? 12 : 0;     /* bit2 and bit3 */
	int	ret;
	DWORD	tout;
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
	sio_SetReadTimeouts(port, tout, 0);
	return TRUE;
}

BOOL OpenPort()
{
	int ret;
	if ((ret = sio_open(GCommData.Port)) != SIO_OK)
	{
		MxShowError(_T("sio_open"), ret);
		return FALSE;
	}
	GbOpen = TRUE;
	if (!PortSet()) {
		sio_close(GCommData.Port);
		return FALSE;
	}
	return TRUE;
}


void CPcommTestDlg::OnBnClickedButton1()
{
	GCommData.Port = portComboBox.GetCurSel() + 1;
	GCommData.iparity = parityComboBox.GetCurSel();
	GCommData.Parity = GParityTable[GCommData.iparity];
	GCommData.ibaudrate = baudComboBox.GetCurSel();
	GCommData.BaudRate = GBaudTable[GCommData.ibaudrate];
	GCommData.ibytesize = dataComboBox.GetCurSel();
	GCommData.ByteSize = GDataBitsTable[GCommData.ibytesize];
	GCommData.istopbits = stopComboBox.GetCurSel();
	GCommData.StopBits = GStopBitsTable[GCommData.istopbits];

	GCommData.Hw = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED;
	GCommData.Sw = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED;
	GCommData.Dtr = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() == BST_CHECKED;
	GCommData.Rts = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == BST_CHECKED;
	if (OpenPort())
	{
		MyDialog mydlg;
		mydlg.DoModal();
	}
}

void CPcommTestDlg::OnBnClickedButton2()
{
	GCommData.Port = portComboBox.GetCurSel() + 1;
	GCommData.iparity = parityComboBox.GetCurSel();
	GCommData.Parity = GParityTable[GCommData.iparity];
	GCommData.ibaudrate = baudComboBox.GetCurSel();
	GCommData.BaudRate = GBaudTable[GCommData.ibaudrate];
	GCommData.ibytesize = dataComboBox.GetCurSel();
	GCommData.ByteSize = GDataBitsTable[GCommData.ibytesize];
	GCommData.istopbits = stopComboBox.GetCurSel();
	GCommData.StopBits = GStopBitsTable[GCommData.istopbits];

	GCommData.Hw = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck() == BST_CHECKED;
	GCommData.Sw = ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck() == BST_CHECKED;
	GCommData.Dtr = ((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck() == BST_CHECKED;
	GCommData.Rts = ((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck() == BST_CHECKED;

	if (allComDlg == NULL)
	{
		allComDlg = new ALLCOM();
		allComDlg->Create(IDD_DIALOG2, this);
	}
	allComDlg->ShowWindow(SW_SHOW);

}

void CPcommTestDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	int ret = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
	if (ret)
	{
		((CButton*)GetDlgItem(IDC_CHECK4))->EnableWindow(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_CHECK4))->EnableWindow(TRUE);
	}
}

void CPcommTestDlg::MyClose()
{
	if (allComDlg)
	{
		allComDlg->DestroyWindow();
		delete allComDlg;
		allComDlg = NULL;
	}
	GbOpen = FALSE;
	CDialogEx::OnDestroy();
}

