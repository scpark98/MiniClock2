#pragma once


// CShutdownTimeDlg 대화 상자

class CShutdownTimeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShutdownTimeDlg)

public:
	CShutdownTimeDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CShutdownTimeDlg();

	CString		m_shutdown_time;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHUTDOWN_TIME };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioMinutes(UINT index);
	afx_msg void OnBnClickedRadioAfterMinute();
	afx_msg void OnBnClickedRadioSpecifiedTime();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CEdit m_edit_after_minute;
	CEdit m_edit_specified_time;
	CButton m_radio_specified_time;
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditSpecifiedTime();
	afx_msg void OnEnChangeEditAfterMinute();
	afx_msg void OnEnUpdateEditAfterMinute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
