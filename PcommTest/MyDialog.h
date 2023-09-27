#pragma once

//1.自定义消息ID号
#define WM_DISPLAY_CHANGE WM_USER + 100

// MyDialog 对话框

class MyDialog : public CDialogEx
{
	DECLARE_DYNAMIC(MyDialog)

public:
	MyDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MyDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	void ShowStatus(void);
	void ShowLStatus(int);
	CEdit sendEdit;
	CEdit recevieEdit;
	afx_msg void OnBnClickedAuto();
	CButton autoCheck;
	//2. 声明自定义的消息函数
	afx_msg  LRESULT  OnDisplayChange(WPARAM wParam, LPARAM lParam);
	CMFCButton ctsState;
	CMFCButton dsrState;
	CMFCButton riState;
	CMFCButton dcdState;
};
