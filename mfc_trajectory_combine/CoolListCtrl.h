#pragma once

class CCoolListCtrl : public CListCtrl
{
public:
	CCoolListCtrl();
   virtual ~CCoolListCtrl();
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	void SetItemColor(DWORD item, COLORREF color);

	CMap<DWORD, DWORD&, COLORREF, COLORREF&> MapItemColor;
	DECLARE_MESSAGE_MAP()
};


