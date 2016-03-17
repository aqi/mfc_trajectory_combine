// Trajecory_combine.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_trajectory_combine.h"
#include "Trajecory_combine.h"
#include "afxdialogex.h"
#include "bdgps_api.h"
#include <map>  //使用map容器存放GPS点
#pragma warning(disable:4996)
//   定义线程参数结构体
typedef struct
{
	const char *dest_filename;   //轨迹文件名
	CListCtrl *p_list_ctrl;  //指向list ctrol控件
	CArray<CString, CString> src_filename;  //存有待处理文件名
} PARAMETER;

GPS_FILEHEAD  first_file_gps_filehead;  //GPS文件头
// CTrajecory_combine 对话框

IMPLEMENT_DYNAMIC(CTrajecory_combine, CDialogEx)

CTrajecory_combine::CTrajecory_combine(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_GPS_COMBINE, pParent)
{

}

CTrajecory_combine::~CTrajecory_combine()
{
}

void CTrajecory_combine::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, file_name_list);
}


BEGIN_MESSAGE_MAP(CTrajecory_combine, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_TRACK_FILE, &CTrajecory_combine::OnBnClickedButtonTrackFile)
	ON_BN_CLICKED(IDC_BUTTON_COMBINE, &CTrajecory_combine::OnBnClickedButtonCombine)
END_MESSAGE_MAP()


// CTrajecory_combine 消息处理程序


BOOL CTrajecory_combine::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	file_name_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	file_name_list.InsertColumn(0, "文件名", LVCFMT_LEFT, 450);
	file_name_list.InsertColumn(1, "文件处理状态", LVCFMT_LEFT, 178);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTrajecory_combine::OnPaint()
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


void CTrajecory_combine::OnBnClickedButtonTrackFile()
{
	//创建一个可以选择多个文件的对话框  "gz"参数表示默认文件后缀名
	CFileDialog file_dlg(TRUE, "gz", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT); 
	file_dlg.m_ofn.lpstrFilter = "gz or bin files(*.gz)|*.gz||";
	file_dlg.m_ofn.lpstrTitle = "选择轨迹文件";

	//做多可以打开500个文件
	file_dlg.m_ofn.nMaxFile = 500 * MAX_PATH;
	char *ch = new TCHAR[file_dlg.m_ofn.nMaxFile];
	file_dlg.m_ofn.lpstrFile = ch;

	//  内存块清零
	ZeroMemory(file_dlg.m_ofn.lpstrFile, sizeof(TCHAR) * file_dlg.m_ofn.nMaxFile);

	if (file_name_list.GetItemCount() != 0)  //列表不为空则先清空
	{
		file_name_list.DeleteAllItems();
		array_filename.RemoveAll();
	}
	
	if (file_dlg.DoModal() == IDOK)
	{
		POSITION file_pos = file_dlg.GetStartPosition();
		CString  list_item("");
		while (file_pos != NULL)
		{
			list_item = file_dlg.GetNextPathName(file_pos);
			array_filename.Add(list_item);
		}
	}

	int total_file = array_filename.GetSize();
	if (total_file == 0)
	{
		AfxMessageBox("没有选择要处理的文件!");
		return;
	}

	for (int i = 0; i < total_file; i++)
	{
		file_name_list.SetItemColor(i, RGB(167, 253, 255));
		file_name_list.InsertItem(i, array_filename.GetAt(i));
		file_name_list.SetItemText(i, 1, "待处理......");
	}
	delete [] ch;
}


//////////////////////////////////////////////////////////////////////////
///  保存所选文件GPS点到map容器
///  para1:filename-----所选文件名
///  para2:n------------list控件行索引号
///  para3:list_ctrl----list控件
///  parq4:map_gps_point---GPS点map容器
//////////////////////////////////////////////////////////////////////////
void SaveGPSPointToMap(const char* filename, const int n, CCoolListCtrl &list_ctrl, std::map<time_t, GPS_POINT>& map_gps_point)
{
	FILE* pFile = fopen(filename, "rb");
	if (!pFile)
	{
		list_ctrl.SetItemColor(n, RGB(255, 0, 0));
		list_ctrl.SetItemText(n, 1, "此文件无法打开,已忽略...");
		return;
	}

	size_t data_size = get_fileSize(filename);
	int32_t gz_header_3byte = GZ_HEADER_3BYTE;
	fread(&gz_header_3byte, 1, sizeof(gz_header_3byte), pFile);
	rewind(pFile);

	// 判断是否是gz文件
	bool is_gzfile = (GZ_HEADER_3BYTE == (gz_header_3byte & 0xFFFFFF));

	char* buffer = NULL;
	if (is_gzfile)
	{
		// 读取gz文件，解压bdgps轨迹文件到内存
		data_size = get_gzbinSize(filename);

		gzFile gzf = gzopen(filename, "rb");
		buffer = new char[data_size + 1];
		buffer[data_size] = 0;

		if (gzread(gzf, buffer, data_size) < 0)
			return;
		gzclose(gzf);
	}
	else
	{
		buffer = new char[data_size];
		fread(buffer, 1, data_size, pFile);
	}

	// 分析内存中的数据
	char *gps_buffer = buffer;
	GPS_FILEHEAD* gps_filehead = (GPS_FILEHEAD*)gps_buffer;   // bin文件头
	if (!((gps_filehead->empty_1 == 0x00) && (gps_filehead->data_pos == 0x18)))  //判断是否是bin轨迹文件
	{
		list_ctrl.SetItemColor(n, RGB(255, 0, 0));
		list_ctrl.SetItemText(n, 1, "此文件不是轨迹文件,已忽略...");
		return;
	}

	if (n == 0)  // 获取第一个文件的文件头
	{
		memcpy(&first_file_gps_filehead, gps_filehead, sizeof(GPS_FILEHEAD));
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

	GPS_POINT* gps_point = (GPS_POINT*)(gps_buffer + gps_filehead->data_pos);  // 第一条GPS记录
	int gps_point_total = (data_size - gps_filehead->data_pos) / (sizeof(GPS_POINT) - ver_offset); // GPS记录条目数

	int fraction = 10;    // 默认输出10个GPS点间距
	int count = fraction;
	while (gps_point_total--)
	{
		if (count % fraction == 0)
		{
			map_gps_point.insert(std::make_pair(gps_point->timestamp, *gps_point));
			count--;
		}
		else if ((--count) == 0)
		{
			count = fraction;
		}
			
		gps_point++;
		gps_point = (GPS_POINT*)((char*)gps_point - ver_offset);    // 兼容旧版本 02 04 05 当作 06版本数据读，读好来个指针回退
	}
	gps_point = (GPS_POINT*)((char*)gps_point + ver_offset);
	--gps_point;
	map_gps_point.insert(std::make_pair(gps_point->timestamp, *gps_point));
	list_ctrl.SetItemText(n, 1, "此文件合并完成!!!");
	
	fclose(pFile);
	delete[] buffer;
	return;
}

//////////////////////////////////////////////////////////////////////////
///     合并所有轨迹文件:将所有选择轨迹文件的GPS点保存到.bin.gz文件
///      para1:bin_file:临时文件
///      para2:filename:合并后文件名
//////////////////////////////////////////////////////////////////////////
bool MakeGpsFile(FILE* bin_file, const char* filename)
{
	// 获得bin数据大小
	rewind(bin_file);
	fseek(bin_file, 0, SEEK_END);
	size_t bin_size = ftell(bin_file);
	rewind(bin_file);

	// 建立gps文件头
	GPS_FILEHEAD* gps_filehead = new GPS_FILEHEAD;
	size_t gps_head_size = sizeof(GPS_FILEHEAD);

	memcpy(gps_filehead, &first_file_gps_filehead, sizeof(GPS_FILEHEAD));
	gps_filehead->data_pos = 0x18;
	gps_filehead->data_ver = 0x06;    // 06 是06版轨迹文件，之前是04  05

	// 分配gps文件头 加  bin数据大小 内存
	size_t buf_size = gps_head_size + bin_size;
	char* buffer = new char[buf_size];
	memset(buffer, 0, buf_size);
	// 加载到缓存
	memcpy(buffer, gps_filehead, gps_head_size);
	fread(buffer + gps_head_size, bin_size, 1, bin_file);

	// 写入gz压缩的百度导航轨迹文件
	gzFile gzf = gzopen(filename, "rw");
	if (gzwrite(gzf, buffer, buf_size) < 0)
	{
		return false;
	}
	
	gzclose(gzf);
	delete [] buffer;
	delete gps_filehead;
	return true;
}

//////////////////////////////////////////////////////////////////////////
///                      线程处理函数:
///      由于在给list控件插入的条目太多时,造成程序界面卡死,为了解决这个问题,在此
///      使用线程处理来函数来向list控件中插入获取到的VIP和密码,这样主线程窗口
///      就不会卡死了.
//////////////////////////////////////////////////////////////////////////
//void ThreadCombine(LPVOID pParam)
//{
//	PARAMETER *p_parameter = (PARAMETER *)pParam;
//	std::map<time_t, GPS_POINT> map_gps_point;    //使用map容器存放GPS点
//	size_t file_num = p_parameter->src_filename.GetSize();
//	CString file_name("");
//	for (size_t i = 0; i < file_num; i++)
//	{
//		//   保存所有GPS点到map容器
//		file_name = p_parameter->src_filename.GetAt(i);
//		SaveGPSPointToMap(file_name.GetBuffer(file_name.GetLength()), i, *(p_parameter->p_list_ctrl), map_gps_point);
//	}
//
//	FILE* bin_file = tmpfile();
//	for (auto it = map_gps_point.begin(); it != map_gps_point.end(); ++it)
//	{
//		fwrite(&it->second, sizeof(GPS_POINT), 1, bin_file);   // 写容器里的gps节点到bin文件
//	}
//	if (MakeGpsFile(bin_file, p_parameter->dest_filename) == false)
//	{
//		AfxMessageBox("未知错误:合并轨迹文件失败!");
//	}
//	else
//	{
//		AfxMessageBox("合并轨迹文件成功!");
//	}
//	fclose(bin_file);
//}

void CTrajecory_combine::OnBnClickedButtonCombine()
{
	if (array_filename.GetSize() == 0)
	{
		AfxMessageBox("还没有选择轨迹文件,请选中后再合并");
		return;
	}

	CFileDialog combine_dlg(FALSE, "bin.gz", "combined_trajectory");  //创建保存文件对话框  "bin.gz"参数表示默认文件后缀名
	combine_dlg.m_ofn.lpstrFilter = "html files(*.bin.gz)|*.bin.gz||";
	combine_dlg.m_ofn.lpstrTitle = "合并选择的轨迹文件";
	std::map<time_t, GPS_POINT> map_gps_point;    //使用map容器存放GPS点
	if (combine_dlg.DoModal() == IDOK)
	{
		CString file_path = combine_dlg.GetPathName();

		size_t file_num = array_filename.GetSize();
		CString file_name("");
		for (size_t i = 0; i < file_num; i++)
		{
			//   保存所有GPS点到map容器
			file_name = array_filename.GetAt(i);
			char *file = file_name.GetBuffer(file_name.GetLength());
			SaveGPSPointToMap(file, i, file_name_list, map_gps_point);
		}

		FILE* bin_file = tmpfile();
		for (auto it = map_gps_point.begin(); it != map_gps_point.end(); ++it)
		{
			fwrite(&it->second, sizeof(GPS_POINT), 1, bin_file);   // 写容器里的gps节点到bin文件
		}

		if (map_gps_point.size() == 0)
		{
			AfxMessageBox("所选文件都不是轨迹文件，无法合并!");
			return;
		}
		if (MakeGpsFile(bin_file, file_path.GetBuffer(file_path.GetLength())))
		{
			AfxMessageBox("合并轨迹文件成功!");
		}
		else
		{
			AfxMessageBox("合并轨迹文件失败!");
		}
	}
}
