#pragma once
#include "header.h"

class CriticalSectionDlg
{
public:
	CriticalSectionDlg(void);
public:
	~CriticalSectionDlg(void);
	static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	
	static CriticalSectionDlg* ptr;
	void Cls_OnClose(HWND hwnd);
	BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};
