// CoolListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "mfc_trajectory_combine.h"
#include "CoolListCtrl.h"

BEGIN_MESSAGE_MAP(CCoolListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	//    ON_WM_SIZE()  
END_MESSAGE_MAP()
CCoolListCtrl::CCoolListCtrl(void)
{
}

CCoolListCtrl::~CCoolListCtrl(void)
{
}

void CCoolListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = CDRF_DODEFAULT;
	NMLVCUSTOMDRAW * lplvdr = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW &nmcd = lplvdr->nmcd;
	switch (lplvdr->nmcd.dwDrawStage)//判断状态  
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		COLORREF ItemColor;
		//根据在SetItemColor(DWORD iItem, COLORREFcolor) 设置的//ITEM号和COLORREF 在摸板中查找，然后进行颜色赋值。  
		if (MapItemColor.Lookup(nmcd.dwItemSpec, ItemColor))
		{
			//lplvdr->clrText = RGB(0,0,0)//ItemColor  
			lplvdr->clrTextBk = ItemColor;
			*pResult = CDRF_DODEFAULT;
		}
		break;
	}
}

void CCoolListCtrl::SetItemColor(DWORD item, COLORREF color)
{
	// m_iCurrentItem = iItem;  
	// m_CurrentColor = color;  
	MapItemColor.SetAt(item, color);//设置某行的颜色  
	this->RedrawItems(item, item);//重新染色  

								  //this->SetCheck(item,1);  
	this->SetFocus();//设置焦点  
	UpdateWindow();
}


