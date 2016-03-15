#pragma once
#include "afxcmn.h"
#include "CoolListCtrl.h"  //自定义可以更改颜色的CListCtrl控件

// CExport_gps_point 对话框

class CExport_gps_point : public CDialogEx
{
	DECLARE_DYNAMIC(CExport_gps_point)

public:
	CExport_gps_point(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExport_gps_point();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GPS_POINT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CCoolListCtrl m_gps_point;
	CString m_trajectory_file;  // 轨迹文件
	int m_edit_fraction;  // 轨迹文件分段数

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonGpsFile();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnBnClickedButtonTxt();
	afx_msg void OnBnClickedButtonMap();
	
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
