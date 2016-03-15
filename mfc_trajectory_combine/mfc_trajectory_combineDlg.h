
// mfc_trajectory_combineDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "Export_gps_point.h"
#include "Trajecory_combine.h"
#include "AboutToolDlg.h"

// Cmfc_trajectory_combineDlg 对话框
class Cmfc_trajectory_combineDlg : public CDialogEx
{
// 构造
public:
	Cmfc_trajectory_combineDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_TRAJECTORY_COMBINE_DIALOG };
#endif
	CExport_gps_point exp_gps_dlg;  //导出gps点对话框
	CTrajecory_combine traje_combine_dlg;  //轨迹合并对话框
	CAboutToolDlg about_tool_dlg;   //关于软件对话框
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
	CTabCtrl m_tab;  //tab控件变量
	afx_msg void OnSelchangeTabFun(NMHDR *pNMHDR, LRESULT *pResult);
};
