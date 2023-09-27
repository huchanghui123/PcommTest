
// PcommTestDlg.h: 头文件
//

#pragma once
#include "ALLCOM.h"

// CPcommTestDlg 对话框
class CPcommTestDlg : public CDialogEx
{
// 构造
public:
	CPcommTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PCOMMTEST_DIALOG };
#endif
	ALLCOM *allComDlg;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void initUI();
	int GetMaxCom();
	CComboBox portComboBox;
	CComboBox baudComboBox;
	CComboBox parityComboBox;
	CComboBox dataComboBox;
	CComboBox stopComboBox;
	afx_msg void MyClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
};
