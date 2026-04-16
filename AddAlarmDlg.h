#pragma once
#include "afxdialogex.h"


// CAddAlarmDlg 대화 상자

class CAddAlarmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAddAlarmDlg)

public:
	CAddAlarmDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAddAlarmDlg();

	CString		m_name;
	CString		m_duration;
	bool		m_add_favorite;
	bool		m_as_floating;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADD_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit_name;
	CEdit m_edit_minutes;
	CButton m_check_add_favorite;
	CButton m_check_floating;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

};
