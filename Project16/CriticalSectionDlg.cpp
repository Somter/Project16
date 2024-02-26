#include "CriticalSectionDlg.h"
#include <sstream>		

CriticalSectionDlg* CriticalSectionDlg::ptr = NULL;

CRITICAL_SECTION cs;

CriticalSectionDlg::CriticalSectionDlg(void)
{
	ptr = this;
}

CriticalSectionDlg::~CriticalSectionDlg(void)
{
	DeleteCriticalSection(&cs);
}

void CriticalSectionDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL CriticalSectionDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	InitializeCriticalSection(&cs);
	return TRUE;
}

void MessageAboutError(DWORD dwError)
{
	LPVOID lpMsgBuf = NULL;	
	TCHAR szBuf[300];	

	BOOL fOK = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	if (lpMsgBuf != NULL)
	{
		wsprintf(szBuf, TEXT("Ошибка %d: %s"), dwError, lpMsgBuf);
		MessageBox(0, szBuf, TEXT("Сообщение об ошибке"), MB_OK | MB_ICONSTOP);
		LocalFree(lpMsgBuf);
	}
}

DWORD WINAPI Write(LPVOID lp)
{
	srand(time(0));
	EnterCriticalSection(&cs); // захват	  
	ofstream out(TEXT("array.txt"));	
	if (!out.is_open())	
	{
		MessageAboutError(GetLastError());	
		LeaveCriticalSection(&cs); 
		return 1;
	}	
	out << "Hello world"; 
	out.close();	

	LeaveCriticalSection(&cs); // освобождение	

	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);	
	return 0;
}

DWORD WINAPI Read(LPVOID lp)
{
	EnterCriticalSection(&cs);

	ifstream in(TEXT("array.txt")); 
	if (!in.is_open())
	{
		MessageAboutError(GetLastError());
		LeaveCriticalSection(&cs);
		return 1;
	}
	string text;
	getline(in, text);
	in.close();

	for (int i = 0; i < 5; ++i) {
		string filename = "new_file" + to_string(i + 1) + ".txt";		
		ofstream out(filename);
		if (!out.is_open())
		{
			MessageAboutError(GetLastError());
			LeaveCriticalSection(&cs);
			return 1;
		}
		out << text;
		out.close();
	}

	LeaveCriticalSection(&cs);

	MessageBox(0, TEXT("Поток создал 5 новых файлов"), TEXT("Критическая секция"), MB_OK);

	return 0;
}

void CriticalSectionDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if (id == IDC_BUTTON1)			
	{
		HANDLE hThread = CreateThread(NULL, 0, Write, 0, 0, NULL);	
		CloseHandle(hThread);	
		hThread = CreateThread(NULL, 0, Read, 0, 0, NULL);	
		CloseHandle(hThread);	
	}	
}

INT_PTR CALLBACK CriticalSectionDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)	
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);	
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);	
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}