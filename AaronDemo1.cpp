
// AaronDemo1.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "AaronDemo1.h"
#include "AaronDemo1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAaronDemo1App

BEGIN_MESSAGE_MAP(CAaronDemo1App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAaronDemo1App 构造

CAaronDemo1App::CAaronDemo1App()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CAaronDemo1App 对象

CAaronDemo1App theApp;


// CAaronDemo1App 初始化

BOOL CAaronDemo1App::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CAaronDemo1Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int GetErrorString(PBYTE pBufferOut, int *nBufferLen, DWORD dwError) {
	HLOCAL hlocal = NULL;   // Buffer that gets the error message string
							// Get the error code's textual description
	BOOL fOk = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, dwError, MAKELANGID(/*LANG_ENGLISH*/LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED/*SUBLANG_ENGLISH_US*/),
		(PTSTR)&hlocal, 0, NULL);

	if (!fOk) { // Is it a network-related error?
		HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL, DONT_RESOLVE_DLL_REFERENCES);
		if (hDll != NULL) {
			FormatMessage(
				FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_FROM_SYSTEM,
				hDll, dwError, MAKELANGID(/*LANG_ENGLISH*/LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED/*SUBLANG_ENGLISH_US*/),
				(PTSTR)&hlocal, 0, NULL);
			FreeLibrary(hDll);
		}
	}
	int nLen = 0;
	if (fOk && hlocal != NULL) {
		nLen = strlen((PTSTR)LocalLock(hlocal));
		*nBufferLen = (nLen < *nBufferLen) ? nLen : *nBufferLen;
		memcpy(pBufferOut, (void*)LocalLock(hlocal), *nBufferLen);
		LocalFree(hlocal);
	}
	return nLen;
}