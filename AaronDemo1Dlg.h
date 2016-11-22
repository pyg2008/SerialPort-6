
// AaronDemo1Dlg.h : ͷ�ļ�
//

#pragma once
//#include "SerialPort.h"
#include "afxwin.h"
#include "CommCommunicateThread.h"

// CAaronDemo1Dlg �Ի���
class CAaronDemo1Dlg : public CDialogEx
{
public:
	int m_nComArray[20];
	int m_nBoundRate[20];
	BOOL bPortOpened;
	CString  strPortName;
	CComboBox m_comboSN;
	CStatic m_staticInfo;
	CListBox m_listboxRead;
	CComboBox m_comboBR;
	CButton m_bnOpen;
	CButton m_ck_time;
	CButton m_ck_len;
	CButton m_ck_ox_show;
	CButton m_ck_receive;
	CommCommunicateThread *m_pCommThread;
// ����
public:
	CAaronDemo1Dlg(CWnd* pParent = NULL);	// ��׼���캯��
	//CSerialPort mySerialPort;
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AARONDEMO1_DIALOG };
	
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//int LayerFrm(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBnOpen();
	afx_msg void OnBnClickedBnClear();

	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();

	BYTE m_btOut1Sent[MAX_BUFF];
	BYTE m_btOut2AutoSent[MAX_BUFF];
	BOOL m_bIsShow;
	BOOL m_bIsShowOX;
	BOOL m_bIsShowTime;
	BOOL m_bIsShowLen;

	UINT m_nListBoxLen;

public:
	void QueryKey(HKEY hKey);
	int ShowInPutInfo(void);
	int SetHorizontalExtent(void);
};
