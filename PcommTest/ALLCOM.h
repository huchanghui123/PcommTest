#pragma once

#define WM_ALLCOM_DISPLAY_CHANGE WM_USER + 200

// ALLCOM 对话框

class ALLCOM : public CDialogEx
{
	DECLARE_DYNAMIC(ALLCOM)

public:
	ALLCOM(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ALLCOM();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg  LRESULT  OnDisplayChange(WPARAM wParam, LPARAM lParam);
	afx_msg void MyClose(void);
	void UpdateUI(void);
	void ShowTitleInfo(void);
	void ShowLStatus(int);
	BOOL OpenPort(void);

};
