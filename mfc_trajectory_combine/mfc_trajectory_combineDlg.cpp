
// mfc_trajectory_combineDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_trajectory_combine.h"
#include "mfc_trajectory_combineDlg.h"
#include "afxdialogex.h"
#include "Export_gps_point.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cmfc_trajectory_combineDlg 对话框



Cmfc_trajectory_combineDlg::Cmfc_trajectory_combineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_TRAJECTORY_COMBINE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc_trajectory_combineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FUN, m_tab);
}

BEGIN_MESSAGE_MAP(Cmfc_trajectory_combineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FUN, &Cmfc_trajectory_combineDlg::OnSelchangeTabFun)
END_MESSAGE_MAP()


// Cmfc_trajectory_combineDlg 消息处理程序

BOOL Cmfc_trajectory_combineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	//ShowWindow(SW_MINIMIZE);---->这句代码好坑人,无意中加上去的
	//  导致程序一运行,就自动最小化到了任务栏,为了解决这个问题感觉屏蔽
	//掉吧......

	// TODO: 在此添加额外的初始化代码
	//tab控件添加选项卡
	m_tab.InsertItem(0, "百度地图GPS点导出工具");
	m_tab.InsertItem(1, "百度地图轨迹文件合并工具");
	m_tab.InsertItem(2, "关于工具");
	//选项卡关联对话框
	exp_gps_dlg.Create(IDD_DIALOG_GPS_POINT, &m_tab);
	traje_combine_dlg.Create(IDD_DIALOG_GPS_COMBINE, &m_tab);
	about_tool_dlg.Create(IDD_DIALOG_ABOUT_TOOL, &m_tab);
	CRect rc;
	m_tab.GetClientRect(&rc);
	//  调整子对话框在父窗口中的位置
	rc.top += 21;
	rc.bottom += 20;
	rc.left += 1;
	rc.right += 20;

	//  移动对话框到tab客户区
	exp_gps_dlg.MoveWindow(&rc);
	traje_combine_dlg.MoveWindow(&rc);
	about_tool_dlg.MoveWindow(&rc);
	//设置隐藏和显示
	exp_gps_dlg.ShowWindow(true);
	traje_combine_dlg.ShowWindow(false);
	about_tool_dlg.ShowWindow(false);
	//设置默认选项卡
	m_tab.SetCurSel(0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cmfc_trajectory_combineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cmfc_trajectory_combineDlg::OnPaint()
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
		CPaintDC  dc(this);
		CRect  rect;
		GetClientRect(&rect);
		CDC  dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap  bmpBackground;
		bmpBackground.LoadBitmap(IDB_BITMAP_BACK);
		BITMAP  bitmap;
		bmpBackground.GetBitmap(&bitmap);
		CBitmap  *pbmpOld = dcMem.SelectObject(&bmpBackground); dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0,
		bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cmfc_trajectory_combineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmfc_trajectory_combineDlg::OnSelchangeTabFun(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	int cur_sel = m_tab.GetCurSel();
	switch (cur_sel)
	{
	case 0:
		exp_gps_dlg.ShowWindow(true);
		traje_combine_dlg.ShowWindow(false);
		about_tool_dlg.ShowWindow(false);
		break;
	case 1:
		traje_combine_dlg.ShowWindow(true);
		exp_gps_dlg.ShowWindow(false);
		about_tool_dlg.ShowWindow(false);
		break;
	case 2:
		about_tool_dlg.ShowWindow(true);
		traje_combine_dlg.ShowWindow(false);
		exp_gps_dlg.ShowWindow(false);
		break;
	default:
		break;
	}
}
