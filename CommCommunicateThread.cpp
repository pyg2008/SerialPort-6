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
		AfxMessageBox(_T("�����¼�ʧ��!")) ;
	}

	osWrite.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (osWrite.hEvent == NULL) {
		AfxMessageBox(_T("�����¼�ʧ��!")) ;
	}

	ShareEvent.hEvent= CreateEvent( NULL, FALSE, FALSE, NULL ) ;
	if (ShareEvent.hEvent == NULL) {
		AfxMessageBox(_T("�����¼�ʧ��!")) ;
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
		//�����¼������е�����С���¼�, ���С�����¼�һֱ���ź�, ��������¼������Ѵ���
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
			WaitCommEvent( COMFile, &dwEvtMask, &ShareEvent);//�ȴ������¼�
			if ((dwEvtMask & EV_RXCHAR) == EV_RXCHAR) {
				ReadCommBlock( );
			
		}
	}

	//���߳̽���
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
	COMFile = CreateFile(szCom.GetBuffer(50), GENERIC_READ | GENERIC_WRITE,//�ɶ�����д
							FILE_SHARE_READ | FILE_SHARE_WRITE, // ������FILE_SHARE_READ,FILE_SHARE_WRITE,FILE_SHARE_DELETE
							NULL, // �ް�ȫ��
							OPEN_EXISTING, //���Ѵ����ļ�
							/*FILE_ATTRIBUTE_NORMAL | */FILE_FLAG_OVERLAPPED, // �ļ�����
							NULL//һ����Ч�ľ�����Ѵ��ڵ��豸���A valid handle to a template file with the GENERIC_READ access right
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
	// ���û�����,����/�����С(�ֽ���)
	SetupComm( /*COMFileTemp*/COMFile,4096,4096) ;
	// ָ�������¼�_�յ��ַ����뻺����
	SetCommMask(/*COMFileTemp*/COMFile, EV_RXCHAR ) ;


	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF ;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0 ;
	CommTimeOuts.ReadTotalTimeoutConstant = 1000 ;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 2*CBR_9600/9600 ;
	CommTimeOuts.WriteTotalTimeoutConstant = 0 ;
	//�������ڶ��������ʱ
	SetCommTimeouts(/*COMFileTemp*/COMFile, &CommTimeOuts ) ;

	//���ô��ڲ���:������=9600;ֹͣλ 1��;��У��;8λ
	dcb.DCBlength = sizeof( DCB ) ;
	GetCommState( /*COMFileTemp*/COMFile, &dcb ) ;
	dcb.BaudRate =CBR_9600;
	dcb.StopBits =ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.ByteSize=8;
	dcb.fBinary=TRUE;//������ͨ��, ���ַ�ͨ��
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

	//�����豸���ƿ�����ͨ���豸
	fRetVal = SetCommState(/*COMFileTemp*/COMFile, &dcb ) ;

	if(!fRetVal) return FALSE;

	//ˢ�建����
	PurgeComm( /*COMFileTemp*/COMFile, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;

	//ָ������ִ����չ����
	EscapeCommFunction( /*COMFileTemp*/COMFile, SETDTR ) ;



// 	//���ô��� ���յ��ַ����뻺�������¼�
// 	if (!SetCommMask(COMFile, EV_RXCHAR )) return ( FALSE );

	return TRUE ;
}


BOOL CommCommunicateThread::CloseComm(int nPort)
{
	//��ֹ���ж˿������¼�
	SetCommMask(COMFile, 0) ;

	//��������ն˾����ź�
	EscapeCommFunction( COMFile, CLRDTR ) ;

	//����ͨ����Դ����������뻺�����ַ�����ֹ��ͨ����Դ�Ϲ���Ķ���д��//���� 
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
		if (997 != nError) {//997�첽û���
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




