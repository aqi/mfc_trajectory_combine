#pragma once
#include "afxcmn.h"
#include "afxtempl.h"
#include "CoolListCtrl.h"  //自定义可以更改条目颜色的类

// CTrajecory_combine 对话框

class CTrajecory_combine : public CDialogEx
{
	DECLARE_DYNAMIC(CTrajecory_combine)

public:
	CTrajecory_combine(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTrajecory_combine();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GPS_COMBINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CCoolListCtrl file_name_list;
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonTrackFile();
	CArray<CString, CString> array_filename;  //保存选中的多个文件的文件名
	afx_msg void OnBnClickedButtonCombine();
};
