// MyPtrList.cpp : implementation file
//

#include "stdafx.h"
#include "MyPtrList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPtrList

CMyPtrList::CMyPtrList(int nBlockSize)
{
	CPtrList::CPtrList(nBlockSize);
}

CMyPtrList::~CMyPtrList()
{
	RemoveAll();
	//???CloseHandle(m_EventEmpty);
	//???CloseHandle(m_MutexModify);	
}


BEGIN_MESSAGE_MAP(CMyPtrList, CMyPtrList)
	//{{AFX_MSG_MAP(CMyPtrList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPtrList message handlers

BOOL CMyPtrList::AddTail(void *pNewObject)
{
	if (IsReadyModify())
	{
		CPtrList::AddTail(pNewObject);
		m_EventEmpty.SetEvent();
		ReleaseMutex(m_MutexModify);
		return TRUE;
	}
	return FALSE;
}

void CMyPtrList::RemoveAll()
{
	PVOID pObj = NULL;
	if (IsReadyModify()) {
		POSITION pos = GetHeadPosition();
		while (pos != NULL){
			delete GetNext(pos);
		}
		CPtrList::RemoveAll();
		m_EventEmpty.ResetEvent();
	}
	m_EventEmpty.ResetEvent();
	ReleaseMutex(m_MutexModify);
}
void * CMyPtrList::RemoveHead()
{
	void *pBuff = NULL;
	if (IsReadyModify())
	{
		if (IsEmpty())
		{
			m_EventEmpty.ResetEvent();
		}
		else
		{
			pBuff = CPtrList::RemoveHead();
			if (IsEmpty())
			{
				m_EventEmpty.ResetEvent();
			}
			else
			{
				m_EventEmpty.SetEvent();
			}
		}
		ReleaseMutex(m_MutexModify);
	}
	return pBuff;
}

CEvent *CMyPtrList::GetEvent()
{
	return &m_EventEmpty;
}

