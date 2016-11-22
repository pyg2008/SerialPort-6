
// AaronDemo1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AaronDemo1.h"
#include "AaronDemo1Dlg.h"
#include "afxdialogex.h"
#include "MyPtrList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_AUTO_SENT 	1
#define TIMER_SHOW_INPUT 	2
#define TIMERS_SHOW_INPUT 	200

CAaronDemo1Dlg *g_pMainWnd = NULL;
CMyPtrList		g_InPutList;
CMyPtrList		g_OutPutList;
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


// CAaronDemo1Dlg 对话框



CAaronDemo1Dlg::CAaronDemo1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AARONDEMO1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pCommThread = NULL;
	bPortOpened =FALSE;
	m_bIsShow = FALSE;
	m_bIsShowOX = FALSE;
	m_bIsShowTime = FALSE;
	m_bIsShowLen = FALSE;
	m_nListBoxLen = 0;

	g_pMainWnd = this;
	for (int i = 0; i < 20; ++i) {
		m_nComArray[i] = -1;
		m_nBoundRate[i] = -1;
	}
	m_nBoundRate[0] = CBR_110;
	m_nBoundRate[1] = CBR_300;
	m_nBoundRate[2] = CBR_600;
	m_nBoundRate[3] = CBR_1200;
	m_nBoundRate[4] = CBR_2400;
	m_nBoundRate[5] = CBR_4800;
	m_nBoundRate[6] = CBR_9600;
	m_nBoundRate[7] = CBR_14400;
	m_nBoundRate[8] = CBR_19200;
	m_nBoundRate[9] = CBR_38400;
	m_nBoundRate[10] = CBR_56000;
	m_nBoundRate[11] = CBR_57600;
	m_nBoundRate[12] = CBR_115200;
	m_nBoundRate[13] = CBR_128000;
	m_nBoundRate[14] = CBR_256000;
}

void CAaronDemo1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboSN);
	DDX_Control(pDX, IDC_STATIC_INFO, m_staticInfo);
	DDX_Control(pDX, IDC_LIST_READ, m_listboxRead);
	DDX_Control(pDX, IDC_COMBO2, m_comboBR);
	DDX_Control(pDX, IDC_BN_OPEN, m_bnOpen);
	DDX_Control(pDX, IDC_CHECK2, m_ck_time);
	DDX_Control(pDX, IDC_CHECK3, m_ck_len);
	DDX_Control(pDX, IDC_CHECK4, m_ck_ox_show);
	DDX_Control(pDX, IDC_CHECK5, m_ck_receive);
}

BEGIN_MESSAGE_MAP(CAaronDemo1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BN_OPEN, &CAaronDemo1Dlg::OnBnClickedBnOpen)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK2, &CAaronDemo1Dlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CAaronDemo1Dlg::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &CAaronDemo1Dlg::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &CAaronDemo1Dlg::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_BN_CLEAR, &CAaronDemo1Dlg::OnBnClickedBnClear)
END_MESSAGE_MAP()


// CAaronDemo1Dlg 消息处理程序

BOOL CAaronDemo1Dlg::OnInitDialog()
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

	//自动查询串口
	HKEY hTestKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hTestKey)) {
		QueryKey(hTestKey);
	}
	RegCloseKey(hTestKey);

	if (-1 == m_nComArray[0]) {
		AfxMessageBox(_T("本机没有串口....."));
	}

	int i = 0;
	while (i < 20 && -1 != m_nComArray[i])
	{
		CString szCom;
		szCom.Format("COM %d", m_nComArray[i]);
		m_comboSN.InsertString(i, szCom.GetBuffer(5));
		++i;
	}
	m_comboSN.SetCurSel(0);

	//m_comboSN.InsertString(0,"1");
	//m_comboSN.InsertString(1, "2");
	//m_comboSN.InsertString(2, "3");
	//m_comboSN.InsertString(3, "4");

	m_comboBR.InsertString(0, "9600");
	m_comboBR.InsertString(1, "19200");
	m_comboBR.InsertString(2, "38400");
	m_comboBR.InsertString(3, "57600");
	m_comboBR.InsertString(4, "115200");
	m_comboBR.InsertString(5, "128000");
	m_comboBR.InsertString(6, "256000");
	m_comboBR.SetCurSel(4);

	SetTimer(TIMER_SHOW_INPUT, TIMERS_SHOW_INPUT, NULL);
	m_pCommThread = (CommCommunicateThread *)AfxBeginThread(RUNTIME_CLASS(CommCommunicateThread), 0, 0, CREATE_SUSPENDED);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAaronDemo1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAaronDemo1Dlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAaronDemo1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAaronDemo1Dlg::OnBnClickedBnOpen()
{
	int nCurSel = m_comboSN.GetCurSel();
	CString strMessage;
	if (nCurSel > -1)
	{
		int nCom = m_nComArray[nCurSel];
		if (bPortOpened)
		{
			if (m_pCommThread->CloseComm(nCom))
			{
				m_pCommThread->SuspendThread();
				m_bnOpen.SetWindowText("连接串口");
				strMessage = "serial port closed";
				//	AfxMessageBox("打开串口失败....");
				m_staticInfo.SetWindowText(strMessage);
				bPortOpened = FALSE; 
	
			}
		}

		else
		{
			if (m_pCommThread->ConnectComm(nCom)) 
			{
				m_pCommThread->ResumeThread();
				m_bnOpen.SetWindowText("关闭串口");
				strMessage = "open successfully";
				bPortOpened = TRUE;
				m_staticInfo.SetWindowText(strMessage);
			}
			else {
				strMessage = "open failed";
			//	AfxMessageBox("打开串口失败....");
				m_staticInfo.SetWindowText(strMessage);
			}
		}
	}
}

void CAaronDemo1Dlg::QueryKey(HKEY hKey)
{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
	//	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	//	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

								 // Enumerate the subkeys, until RegEnumKeyEx fails.

								 // 	if (cSubKeys)
								 // 	{
								 // 		printf( "\nNumber of subkeys: %d\n", cSubKeys);
								 // 
								 // 		for (i=0; i<cSubKeys; i++) 
								 // 		{ 
								 // 			cbName = MAX_KEY_LENGTH;
								 // 			retCode = RegEnumKeyEx(hKey, i,
								 // 				achKey, 
								 // 				&cbName, 
								 // 				NULL, 
								 // 				NULL, 
								 // 				NULL, 
								 // 				&ftLastWriteTime); 
								 // 			if (retCode == ERROR_SUCCESS) 
								 // 			{
								 // 				_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
								 // 			}
								 // 		}
								 // 	} 

								 // Enumerate the key values. 
	if (cValues > 0) {
		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++) {
			cchValue = MAX_VALUE_NAME;  achValue[0] = '\0';
			if (ERROR_SUCCESS == RegEnumValue(hKey, i, achValue, &cchValue, NULL, NULL, NULL, NULL)) {
				CString szName(achValue);
				if (-1 != szName.Find(_T("Serial")) || -1 != szName.Find(_T("VCom"))) {
					BYTE strDSName[10]; memset(strDSName, 0, 10);
					DWORD nValueType = 0, nBuffLen = 10;
					if (ERROR_SUCCESS == RegQueryValueEx(hKey, (LPCTSTR)achValue, NULL, &nValueType, strDSName, &nBuffLen)) {
						int nIndex = -1;
						while (++nIndex < 20) {
							if (-1 == m_nComArray[nIndex]) {
								m_nComArray[nIndex] = atoi((char*)(strDSName + 3));
								break;
							}
						}
					}
				}
			}
		}
	}
	else {
		AfxMessageBox(_T("本机没有串口....."));
	}

}

void CAaronDemo1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_AUTO_SENT == nIDEvent) {
		WORD nLen = m_btOut2AutoSent[0] * 0x100 + m_btOut2AutoSent[1];
		if (nLen > 2) {
			PBYTE pOut = new BYTE[nLen];
			memcpy(pOut, m_btOut2AutoSent, nLen);
			g_OutPutList.AddTail(pOut);
		}
		if (g_OutPutList.GetCount() > 1000)
			g_OutPutList.RemoveAll();
	}
	else if (TIMER_SHOW_INPUT == nIDEvent) {
		if (m_bIsShow)
			ShowInPutInfo();
	}

	CDialogEx::OnTimer(nIDEvent);
}


int CAaronDemo1Dlg::ShowInPutInfo(void)
{
	if (m_listboxRead.GetCount() > 1000)
		m_listboxRead.ResetContent();

	if (WAIT_OBJECT_0 == WaitForSingleObject(*g_InPutList.GetEvent(), 0))
	{
		CString szTemp, szShow;

		PBYTE pIn = (PBYTE)g_InPutList.RemoveHead();
		if (NULL != pIn) {
			WORD nLen = pIn[0] * 0x100 + pIn[1];
			CTime tNow = CTime::GetCurrentTime();
            
			//Take out the data    2016//11/21
			PBYTE p = &(pIn[5]);
			INT data = (INT32)*p;
			/*PBYTE p = pIn + 5;
			int t = (int)(*p);*/
			
			if (m_bIsShowTime)
				szShow = tNow.Format("%H:%M:%S");
			if (m_bIsShowLen) {
				szTemp.Format("[%d]", nLen);
				szShow += szTemp;
			}
			if (!szShow.IsEmpty())
				szShow += ":";

			for (int i = 2; i < nLen; ++i) 
			{
				if (m_bIsShowOX) {
					if (i == nLen - 1)
						szTemp.Format("%02X", pIn[i]);
					else
						szTemp.Format("%02X ", pIn[i]);

					szShow += szTemp;
				}
				else {
					szShow += pIn[i];
				}

			}
			m_listboxRead.InsertString(0, szShow);

			if ((int)m_nListBoxLen < szShow.GetLength()) 
			{
				m_nListBoxLen = szShow.GetLength();
				SetHorizontalExtent();
			}

			delete[] pIn;
		}
	}

	return 0;
}

int CAaronDemo1Dlg::SetHorizontalExtent(void)
{
	CString    str;
	CSize      sz;
	int        dx = 0;
	TEXTMETRIC tm;
	CDC*       pDC = m_listboxRead.GetDC();
	CFont*     pFont = m_listboxRead.GetFont();

	CFont* pOldFont = pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tm);
	m_listboxRead.GetText(0, str);
	sz = pDC->GetTextExtent(str);
	sz.cx += tm.tmAveCharWidth;

	pDC->SelectObject(pOldFont);
	m_listboxRead.ReleaseDC(pDC);
	m_listboxRead.SetHorizontalExtent(sz.cx);

	return 0;
}

void CAaronDemo1Dlg::OnBnClickedCheck2()
{
	if (m_ck_time.GetCheck())
		m_bIsShowTime = TRUE;
	else
		m_bIsShowTime = FALSE;
}


void CAaronDemo1Dlg::OnBnClickedCheck3()
{
	if (m_ck_len.GetCheck())
		m_bIsShowLen = TRUE;
	else
		m_bIsShowLen = FALSE;
}


void CAaronDemo1Dlg::OnBnClickedCheck4()
{
	if (m_ck_ox_show.GetCheck())
		m_bIsShowOX = TRUE;
	else
		m_bIsShowOX = FALSE;
}


void CAaronDemo1Dlg::OnBnClickedCheck5()
{
	if (m_ck_receive.GetCheck())
		m_bIsShow = TRUE;
	else
		m_bIsShow = FALSE;
}


void CAaronDemo1Dlg::OnBnClickedBnClear()
{
	m_listboxRead.ResetContent();
}
