// CommCommunicateThread.cpp : implementation file
//

#include "stdafx.h"
#include "AaronDemo1.h"
#include "CommCommunicateThread.h"
int GetErrorString(PBYTE pBufferOut, int *nBufferLen, DWORD dwError);

#include "AaronDemo1Dlg.h"
#include "MyPtrList.h"

extern CAaronDemo1Dlg *g_pMainWnd;
extern CMyPtrList		g_InPutList;
extern CMyPtrList		g_OutPutList;

// CommCommunicateThread

IMPLEMENT_DYNCREATE(CommCommunicateThread, CWinThread)

CommCommunicateThread::CommCommunicateThread()
{
	memset(&osRead, 0, sizeof(osRead));
	memset(&osWrite, 0, sizeof(osWrite));
	memset(&ShareEvent, 0, sizeof(ShareEvent));
	COMFile = NULL;
	m_nBuffLen = 0; 
	memset(m_InPutBuff, 0, MAX_PATH);

	osRead.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (osRead.hEvent == NULL) {
		AfxMessageBox(_T("建立事件失败!")) ;
	}

	osWrite.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (osWrite.hEvent == NULL) {
		AfxMessageBox(_T("建立事件失败!")) ;
	}

	ShareEvent.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (ShareEvent.hEvent == NULL) {
		AfxMessageBox(_T("建立事件失败!")) ;
	}

}

CommCommunicateThread::~CommCommunicateThread()
{
}

BOOL CommCommunicateThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CommCommunicateThread::ExitInstance()
{
	CloseHandle(osRead.hEvent);
	memset(&osRead, 0, sizeof(osRead));;
	CloseHandle(osWrite.hEvent);
	memset(&osWrite, 0, sizeof(osWrite));
	CloseHandle(ShareEvent.hEvent);
	memset(&ShareEvent, 0, sizeof(ShareEvent));

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CommCommunicateThread, CWinThread)
END_MESSAGE_MAP()


// CommCommunicateThread message handlers


int CommCommunicateThread::Run()
{
	HANDLE hEventArr[2];

	hEventArr[0] = osRead.hEvent;
	hEventArr[1] = *g_OutPutList.GetEvent();

	while(1){
		//优先事件数据中的索引小的事件, 如果小索引事件一直有信号, 则大索引事件将很难触发
		DWORD nResutl = WaitForMultipleObjectsEx(2, hEventArr, FALSE, 200, TRUE/*INFINITE*/);
		if(0 == nResutl){
			if (g_OutPutList.GetCount() > 1000)
				g_OutPutList.RemoveAll();

			WORD nLen = (WORD)m_nBuffLen + 2;
			PBYTE pIn = new BYTE[nLen];
			pIn[0] = HIBYTE(nLen);
			pIn[1] = LOBYTE(nLen);
			memcpy(pIn + 2, m_InPutBuff, m_nBuffLen);
			g_InPutList.AddTail(pIn);
			m_nBuffLen = 0;
		}
		else if(1 == nResutl){
			PBYTE pOut = (PBYTE)g_OutPutList.RemoveHead();
			int nLen = pOut[0] * 0x100 + pOut[1] - 2;
			WriteCommBlock(pOut + 2, nLen);
			delete[] pOut;
		}

			DWORD dwEvtMask = 0 ; 
			WaitCommEvent( COMFile, &dwEvtMask, &ShareEvent);//等待串口事件
			if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) {
				ReadCommBlock( );
			
		}
	}

	//子线程结束
	CloseHandle( osRead.hEvent ) ;
	CloseHandle( osWrite.hEvent ) ;
	CloseHandle(ShareEvent.hEvent);
	return CWinThread::Run();
}


int CommCommunicateThread::ReadCommBlock(LPSTR lpszBlock)
{
	DWORD dwErrorFlags;
	COMSTAT ComStat ;
	DWORD dwLength;
	DWORD dwOutBytes = 0;
	ClearCommError( COMFile, &dwErrorFlags, &ComStat ) ;
	dwLength = ComStat.cbInQue;

	if (m_nBuffLen + dwLength > MAX_BUFF)
		m_nBuffLen = 0;
	if (dwLength > MAX_BUFF)
		dwLength = MAX_BUFF;
	
	if (dwLength > 0)
		ReadFile( COMFile, m_InPutBuff + m_nBuffLen/*lpszBlock*/, dwLength, &dwOutBytes, &/*ShareEvent*/osRead );


	m_nBuffLen/*dwLength*/ += dwLength;
	//m_nBuffLen = m_nBuffLen > MAX_BUFF ? MAX_BUFF : m_nBuffLen;
// 	if (WAIT_OBJECT_0 == WaitForSingleObject(osRead.hEvent, 200)) {
//  		ResetEvent(osRead.hEvent);
// 	else
// 		ResetEvent(ShareEvent.hEvent);
// 		m_nBuffLen/*dwLength*/ += dwLength;
// 
// 		if (m_nBuffLen > 50)
// 		{
// 			if (m_nBuffLen > 256)
// 			{
// 				m_nBuffLen = 256;
// 			}
// 			CStringA szTemp, szShow;
// 			for (DWORD i = 0; i < m_nBuffLen; ++i) {
// 				szTemp.Format((i + 1 != m_nBuffLen) ? _T("%02X ") :_T("%02X"), m_InPutBuff[i]);
// 				szShow += szTemp; szTemp.Empty();
// 			}
// 			g_pMainWnd->m_LsDataIn.InsertString(0, szShow);
// 
// 			memset(m_InPutBuff, 0, MAX_PATH);
// 			m_nBuffLen = 0;
// 		}
// 	}

	return ( dwLength ) ;
}


BOOL CommCommunicateThread::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinThread::OnIdle(lCount);
}


BOOL CommCommunicateThread::ConnectComm(int nPort)
{
//	BYTE bSet;
	DCB dcb ;
	BOOL fRetVal ;
	COMMTIMEOUTS CommTimeOuts;
	CString szCom; 
	szCom.Format(_T("\\\\.\\COM%d"), nPort);
	COMFile = CreateFile(szCom.GetBuffer(50), GENERIC_READ | GENERIC_WRITE,//可读、可写
							FILE_SHARE_READ | FILE_SHARE_WRITE, // 不共享FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
							NULL, // 无安全描
							OPEN_EXISTING, //打开已存在文件
							/*FILE_ATTRIBUTE_NORMAL | */FILE_FLAG_OVERLAPPED, // 文件属性
							NULL//一个有效的句柄，已存在的设备句柄A valid handle to a template file with the GENERIC_READ access right
							);

	int nError = GetLastError();
	if (INVALID_HANDLE_VALUE == COMFile/*COMFileTemp*/ ){
		char chError[256];memset(chError, 0, 256);
		int nBuffLen = 256;
		if (GetErrorString((PBYTE)chError, &nBuffLen, GetLastError())) {
			AfxMessageBox(chError);
		}
		return ( FALSE ) ;
	}
	// 设置缓冲区,输入/输出大小(字节数)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// 指定监视事件_收到字符放入缓冲区
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/9600 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	//给定串口读与操作限时
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//设置串口参数:波特率=9600;停止位 1个;无校验;8位
	dcb.DCBlength = sizeof( DCB ) ;
	GetCommState( /*COMFileTemp*/COMFile, &dcb ) ;
	dcb.BaudRate =CBR_9600;
	dcb.StopBits =ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ByteSize=8;
	dcb.fBinary=TRUE;//二进制通信, 非字符通信
	dcb.fOutxDsrFlow = 0 ;
	dcb.fDtrControl = DTR_CONTROL_ENABLE ;
	dcb.fOutxCtsFlow = 0 ;
	dcb.fRtsControl = RTS_CONTROL_ENABLE ;
	dcb.fInX = dcb.fOutX = 1 ;
	dcb.XonChar = 0X11 ;
	dcb.XoffChar = 0X13 ;
	dcb.XonLim = 100 ;
	dcb.XoffLim = 100 ;
	dcb.fParity = TRUE ;

	//根据设备控制块配置通信设备
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;

	if(!fRetVal) return FALSE;

	//刷清缓冲区
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//指定串口执行扩展功能
	EscapeCommFunction( /*COMFileTemp*/COMFile, SETDTR ) ;



// 	//设置串口 “收到字符放入缓冲区”事件
// 	if (!SetCommMask(COMFile, EV_RXCHAR )) return ( FALSE );

	return TRUE ;
}


BOOL CommCommunicateThread::CloseComm(int nPort)
{
	//禁止串行端口所有事件
	SetCommMask(COMFile, 0) ;

	//清除数据终端就绪信号
	EscapeCommFunction( COMFile, CLRDTR ) ;

	//丢弃通信资源的输出或输入缓冲区字符并终止在通信资源上挂起的读、写操//场作 
	PurgeComm( COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	CloseHandle( COMFile );
	COMFile = NULL;
// 	CloseHandle(osRead.hEvent);
// 	memset(&osRead, 0, sizeof(osRead));;
// 	CloseHandle(osWrite.hEvent);
// 	memset(&osWrite, 0, sizeof(osWrite));
// 	CloseHandle(ShareEvent.hEvent);
// 	memset(&ShareEvent, 0, sizeof(ShareEvent));

	return TRUE;
}


int CommCommunicateThread::WriteCommBlock(PBYTE pOutData, int nDataLen)
{
	if (NULL == pOutData || nDataLen < 1)
		return FALSE;
	//while(nDataLen > 0){
	DWORD nLen = 0;
	if (! WriteFile(COMFile, (LPCVOID)pOutData, nDataLen, &nLen, &/*ShareEvent*/osWrite)) {

		if (WAIT_OBJECT_0 == WaitForSingleObject(osWrite.hEvent, 0xFFFFFF)) 
			ResetEvent(osWrite.hEvent);
		else
			ResetEvent(osWrite.hEvent);


		DWORD nError = GetLastError();
		if (997 != nError) {//997异步没完成
			char chError[256];memset(chError, 0, 256);
			int nBuffLen = 256;
			if (GetErrorString((PBYTE)chError, &nBuffLen, nError)) {
				AfxMessageBox(chError);
			}
		}
	}
	//}

	return TRUE;
}




