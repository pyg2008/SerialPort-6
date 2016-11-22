#pragma once

//#include "CommunicateCommDlg.h"

// CommCommunicateThread

class CommCommunicateThread : public CWinThread
{
	DECLARE_DYNCREATE(CommCommunicateThread)


public:
	OVERLAPPED osRead;
	OVERLAPPED osWrite;

	OVERLAPPED ShareEvent;

	HANDLE COMFile;

	BYTE m_InPutBuff[MAX_BUFF] ;
	UINT m_nBuffLen;


	int ReadCommBlock(LPSTR lpszBlock = NULL);
protected:
	CommCommunicateThread();           // protected constructor used by dynamic creation
	virtual ~CommCommunicateThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount);
	BOOL ConnectComm(int nPort);
	BOOL CloseComm(int nPort);
	int WriteCommBlock(PBYTE pOutData, int nDataLen);

};


