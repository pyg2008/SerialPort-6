#if !defined(AFX_MYPTRLIST_H__82852E5D_833A_4426_9FDE_BD61AAEB345A__INCLUDED_)
#define AFX_MYPTRLIST_H__82852E5D_833A_4426_9FDE_BD61AAEB345A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyPtrList.h : header file
//
#include <afxmt.h>

/////////////////////////////////////////////////////////////////////////////
// CMyPtrList window

class CMyPtrList : public CPtrList
{
// Construction
public:
	CMyPtrList(int nBlockSize = 20);

// Attributes
public:


// Attributes	
private:
	CEvent m_EventEmpty;
	CMutex m_MutexModify;
	
// Operations
private:
	inline BOOL IsReadyModify() { return (WAIT_OBJECT_0 == WaitForSingleObject(m_MutexModify, INFINITE) );}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPtrList)
	//}}AFX_VIRTUAL

// Implementation
public:
	CEvent	*GetEvent();
	void	*RemoveHead();
	void	RemoveAll();
	BOOL	AddTail(void *pNewObject);
	virtual ~CMyPtrList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyPtrList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPTRLIST_H__82852E5D_833A_4426_9FDE_BD61AAEB345A__INCLUDED_)
