#pragma once


// CAboutToolDlg 对话框

class CAboutToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutToolDlg)

public:
	CAboutToolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAboutToolDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ABOUT_TOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
