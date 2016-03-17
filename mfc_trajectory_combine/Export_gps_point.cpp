// Export_gps_point.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_trajectory_combine.h"
#include "Export_gps_point.h"
#include "afxdialogex.h"
#include "bdgps_api.h"
#include <fstream>
#pragma warning(disable:4996)
//   定义线程参数结构体
typedef struct
{
	const char *filename;   //轨迹文件名
	CCoolListCtrl *p_list_ctrl; //指向list ctrol控件
	int fraction;    //轨迹文件分段数
} PARAMETER;

// CExport_gps_point 对话框

IMPLEMENT_DYNAMIC(CExport_gps_point, CDialogEx)

CExport_gps_point::CExport_gps_point(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_GPS_POINT, pParent)
	, m_trajectory_file(_T(""))
	, m_edit_fraction(10)
{

}

CExport_gps_point::~CExport_gps_point()
{
}

void CExport_gps_point::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GPS_POINT, m_gps_point);
	DDX_Text(pDX, IDC_EDIT_TRAJ_FILE, m_trajectory_file);
	DDX_Text(pDX, IDC_EDIT_FRACTION, m_edit_fraction);
	DDV_MinMaxInt(pDX, m_edit_fraction, 0, 3600);
}


BEGIN_MESSAGE_MAP(CExport_gps_point, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GPS_FILE, &CExport_gps_point::OnBnClickedButtonGpsFile)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CExport_gps_point::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_TXT, &CExport_gps_point::OnBnClickedButtonTxt)
	ON_BN_CLICKED(IDC_BUTTON_MAP, &CExport_gps_point::OnBnClickedButtonMap)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CExport_gps_point 消息处理程序


BOOL CExport_gps_point::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_gps_point.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_gps_point.InsertColumn(0, "纬度", LVCFMT_LEFT, 150);
	m_gps_point.InsertColumn(1, "经度", LVCFMT_LEFT, 150);
	m_gps_point.InsertColumn(2, "时速(Km/H)", LVCFMT_LEFT, 120);
	m_gps_point.InsertColumn(3, "时间戳", LVCFMT_LEFT, 210);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CExport_gps_point::OnBnClickedButtonGpsFile()
{
	CFileDialog file_dlg(TRUE);  //创建选择文件对话框
	file_dlg.m_ofn.lpstrFilter = "gz or bin files(*.gz)|*.gz||";
	file_dlg.m_ofn.lpstrTitle = "选择轨迹文件";
	m_trajectory_file = "";
	UpdateData(FALSE);
	if (file_dlg.DoModal() == IDOK)
	{
		    m_trajectory_file = file_dlg.GetPathName();  //获取全路径
			UpdateData(FALSE);  //在Edit显示选中的文件全路径
			return;
	}
}

//////////////////////////////////////////////////////////////////////////
///       向列表控件插入GPS点
//////////////////////////////////////////////////////////////////////////
void InsertGPSPoint(CCoolListCtrl *ptr_list, GPS_POINT *ptr_point, int pos)
{
	if ((ptr_list == NULL) || (ptr_point == NULL))   return;
	CString cstr("");
	ptr_list->SetItemColor(pos, RGB(167, 253, 255));
	//插入经度
	cstr.Format("%.5f", (float)ptr_point->latitude / 100000);
	ptr_list->InsertItem(pos, cstr);

	//插入纬度
	cstr.Format("%.5f", (float)ptr_point->longitude / 100000);
	ptr_list->SetItemText(pos, 1, cstr);

	//插入时速
	cstr.Format("%.1f", (float)ptr_point->speed / 100 * 3600 / 1000);
	ptr_list->SetItemText(pos, 2, cstr);

	//插入时间戳
	CTime time(ptr_point->timestamp);
	ptr_list->SetItemText(pos, 3, time.Format("%Y-%m-%d %H:%M:%S"));
}

//////////////////////////////////////////////////////////////////////////
///                      线程处理函数:
///      由于在给list控件插入的条目太多时,造成程序界面卡死,为了解决这个问题,在此
///      使用线程处理来函数来向list控件中插入获取到的VIP和密码,这样主线程窗口
///      就不会卡死了.
//////////////////////////////////////////////////////////////////////////
void ThreadFunc(LPVOID pParam)
{
	PARAMETER *p_parameter = (PARAMETER *)pParam;
	FILE* ptr_file = fopen(p_parameter->filename, "rb"); // 数据输入文件
	if (NULL == ptr_file)
	{
		AfxMessageBox("文件不存在,打开文件失败!");
		return;
	}

	int32_t gz_header_3byte = GZ_HEADER_3BYTE;  //gz文件头
	fread(&gz_header_3byte, 1, sizeof(gz_header_3byte), ptr_file);
	rewind(ptr_file);

	// 判断是否是gz文件
	bool is_gzfile = (GZ_HEADER_3BYTE == (gz_header_3byte & 0xFFFFFF));

	char* buffer = NULL;
	long data_size = get_fileSize(p_parameter->filename);
	if (is_gzfile)
	{
		// 读取gz文件到内存
		data_size = get_gzbinSize(p_parameter->filename); //gz文件解压后的大小

		gzFile gzf = gzopen(p_parameter->filename, "rb");
		buffer = new char[data_size + 1];
		buffer[data_size] = 0;

		if (gzread(gzf, buffer, data_size) < 0)
		{
			AfxMessageBox("读取gz文件出现错误!");
			return;
		}
		gzclose(gzf);
	}
	else
	{
		// 读已经解压开bin文件到内存
		buffer = new char[data_size];
		fread(buffer, 1, data_size, ptr_file);
	}

	// 分析内存中的数据
	char* gps_buffer = buffer;
	GPS_FILEHEAD* gps_filehead = (GPS_FILEHEAD*)gps_buffer;   // bin文件头
	if (!((gps_filehead->empty_1 == 0x00) && (gps_filehead->data_pos == 0x18)))
	{
		AfxMessageBox("错误:所选文不是轨迹文件!");
		return;
	}

    // 兼容旧版本 02 04 05 和当前 06版本数据，使用指针回退，兼容数据结构不一样长
	int ver_offset = 0;
	if (gps_filehead->data_ver == 5)
	{
		ver_offset = sizeof(int32_t);
	}
	else if (gps_filehead->data_ver <= 4)
	{
		ver_offset = 2 * sizeof(int32_t);
	}

	size_t GPS_POINT_SIZE = sizeof(GPS_POINT);
	GPS_POINT* gps_point = (GPS_POINT*)(gps_buffer + gps_filehead->data_pos);  // 第一条GPS点

	int gps_point_total = (data_size - gps_filehead->data_pos) / (GPS_POINT_SIZE - ver_offset); // GPS点个数
	if (gps_point_total == 0)  //判断轨迹文件中是否有GPS点
	{
		AfxMessageBox("此文件没有GPS点!");
		return;
	}

	int item_count = p_parameter->p_list_ctrl->GetItemCount();
	if (item_count)  //如果列表非空,则先删除list所有条目,然后插入新的条目
	{
		p_parameter->p_list_ctrl->DeleteAllItems();
		item_count = 0;
	}

   try
   {
	  const int tmp = gps_point_total;  //零时变量保存GPS点总数
	  if (p_parameter->fraction == 0)  //如果点间距为0,则表示打印所有GPS点
	  {
		 while (gps_point_total--)
		 {
			InsertGPSPoint(p_parameter->p_list_ctrl, gps_point++, item_count++);
			gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);    // 兼容旧版本 02 04 05 当作 06版本数据读，读好来个指针回退
		 }
	  }
	  else
	  {
		 int count = tmp / p_parameter->fraction;  //隔fraction个点取点
		 if (count == 0)   //判断是否分段数过多
		 {
			 CString err_str;
			 err_str.Format("%d", tmp);
			 AfxMessageBox("分段数过多,请输入分段数少于" + err_str + "的数字!");
			 return;
		 }
		 while (gps_point_total--)
		 {
			if ((count % (tmp / p_parameter->fraction)) == 0)
			{
				InsertGPSPoint(p_parameter->p_list_ctrl, gps_point, item_count++);
				count--;
			}
			else
			{
				if (--count == 0)
				{
					count = tmp / p_parameter->fraction;
				}
			}

			gps_point++;
			gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);
		  }

		  gps_point = (GPS_POINT*)((char*)gps_point + ver_offset);
		  InsertGPSPoint(p_parameter->p_list_ctrl, --gps_point, item_count++); // 终点节点
	  }
    }
	catch (...)  //    捕获未知异常
	{
		AfxMessageBox("未知错误:可能文件损坏!");
	}
	fclose(ptr_file);
	delete [] buffer;
}

void CExport_gps_point::OnBnClickedButtonExport()
{
	UpdateData(TRUE); //数据传送到控件变量
	if (0 == m_trajectory_file.GetLength())
	{
		AfxMessageBox("请先选择轨迹文件!");
		return;
	}
	PARAMETER *p_parameter = new PARAMETER;  //自定义线程参数结构
	p_parameter->filename = m_trajectory_file.GetBuffer(m_trajectory_file.GetLength());
	p_parameter->p_list_ctrl = &m_gps_point;
	p_parameter->fraction = m_edit_fraction;

	//开启线程处理导出gps点
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, p_parameter, 0, NULL);
}


void CExport_gps_point::OnBnClickedButtonTxt()
{
	size_t item_count = m_gps_point.GetItemCount();
	if (item_count == 0)
	{
		AfxMessageBox("列表为空,请导出GPS点后再保存");
		return;
	}
	
	CString def_filename = m_trajectory_file;
	int n = def_filename.Find('.');
	def_filename = def_filename.Left(n) + ".gps";  //获取默认文件名

	CFileDialog export_dlg(FALSE, "txt", def_filename);  //创建保存文件对话框  "txt"参数表示默认文件后缀名
	export_dlg.m_ofn.lpstrFilter = "txt files(*.txt)|*.txt||";
	export_dlg.m_ofn.lpstrTitle = "导出GPS到文本文件";
	if (export_dlg.DoModal() == IDOK)
	{
		std::ofstream gps_file(export_dlg.GetPathName(), std::ios::out);
		if (!gps_file.is_open())
		{
			AfxMessageBox("保存文件失败,未知错误!");
			return;
		}
		// 导出文件标题
		gps_file << "经度" << "\t" << "纬度" << "\t" << "时速(Km/H)" << "\t" << "时间戳" << std::endl;

		for (size_t i = 0; i < item_count; i++)
		{
			gps_file << m_gps_point.GetItemText(i, 0) << "\t"       //写入经度
				     << m_gps_point.GetItemText(i, 1) << "\t"       //写入纬度
				     << m_gps_point.GetItemText(i, 2) << "\t"       //写入时速
				     << m_gps_point.GetItemText(i, 3) << std::endl; //写入时间戳
		}
		gps_file.close();
		AfxMessageBox("生成文本文件成功!");
	}
}


void CExport_gps_point::OnBnClickedButtonMap()
{
	size_t item_count = m_gps_point.GetItemCount();
	if (0 == item_count)
	{
		AfxMessageBox("列表为空,请导出GPS点后再保存");
		return;
	}

	CString def_filename = m_trajectory_file;
	int n = def_filename.Find('.');
	def_filename = def_filename.Left(n);  //获取默认文件名

	CFileDialog export_dlg(FALSE, "html", def_filename);  //创建保存文件对话框  "html"参数表示默认文件后缀名
	export_dlg.m_ofn.lpstrFilter = "html files(*.html)|*.html||";
	export_dlg.m_ofn.lpstrTitle = "生成地图文件";

	if (export_dlg.DoModal() == IDOK)
	{
		std::ofstream map_file(export_dlg.GetPathName(), std::ios::out);
		if (!map_file.is_open())
		{
			AfxMessageBox("生成地图文件失败,未知错误!");
			return;
		}

		//   生成html地图文件
		map_file << "<html><head>\n<meta http-equiv=\"refresh\" content=\"2; url=http://www.google.cn/maps/dir/";
		for (size_t i = 0; i < item_count; i++)
		{
			map_file << m_gps_point.GetItemText(i, 0) << "%09"       //写入经度
				     << m_gps_point.GetItemText(i, 1) << "/";         //写入纬度
		}
		map_file << "\" />\n</head><body>\n自动跳转到 到google地图 <p>\n http://www.google.cn/maps/dir/ gps坐标/ gps坐标 / gps坐标 <p>\n";
		map_file << "当前轨迹数据文件:" << m_trajectory_file << "<p>\n</body></html>";
		map_file.close();
		//AfxMessageBox("生成文地图件成功!");
		
		ShellExecute(NULL, "open", export_dlg.GetPathName(), "", "", SW_SHOWNORMAL);
	}
}


void CExport_gps_point::OnPaint()
{
	CPaintDC  dc(this);
	CRect  rect;
	GetClientRect(&rect);
	CDC  dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap  bmpBackground;
	bmpBackground.LoadBitmap(IDB_BITMAP_GPS_BACK);
	BITMAP  bitmap;
	bmpBackground.GetBitmap(&bitmap);
	CBitmap  *pbmpOld = dcMem.SelectObject(&bmpBackground); dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0,
	bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
}


HBRUSH CExport_gps_point::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	//让所有的静态文本都透明好显示背景色
	if (nCtlColor == CTLCOLOR_DLG)
	{
		//返回对话框的颜色   
		hbr = (HBRUSH)::CreateSolidBrush(RGB(146, 161, 202));
	}
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		//让静态文本框透明   
		pDC->SetBkMode(TRANSPARENT);
		//pDC->SetTextColor(RGB(0, 0, 255));   //设置字体颜色白色   
		hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	return hbr;
}
