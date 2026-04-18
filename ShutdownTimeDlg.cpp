// ShutdownTimeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MiniClock2.h"
#include "ShutdownTimeDlg.h"
#include "afxdialogex.h"

#include "Common/Functions.h"


// CShutdownTimeDlg 대화 상자

IMPLEMENT_DYNAMIC(CShutdownTimeDlg, CDialogEx)

CShutdownTimeDlg::CShutdownTimeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHUTDOWN_TIME, pParent)
{

}

CShutdownTimeDlg::~CShutdownTimeDlg()
{}

void CShutdownTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_AFTER_MINUTE, m_edit_after_minute);
	DDX_Control(pDX, IDC_EDIT_SPECIFIED_TIME, m_edit_specified_time);
	DDX_Control(pDX, IDC_RADIO_SPECIFIED_TIME, m_radio_specified_time);
}


BEGIN_MESSAGE_MAP(CShutdownTimeDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_05, IDC_RADIO_60, &CShutdownTimeDlg::OnBnClickedRadioMinutes)
	ON_BN_CLICKED(IDC_RADIO_SPECIFIED_TIME, &CShutdownTimeDlg::OnBnClickedRadioSpecifiedTime)
	ON_BN_CLICKED(IDOK, &CShutdownTimeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CShutdownTimeDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CShutdownTimeDlg::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT_SPECIFIED_TIME, &CShutdownTimeDlg::OnEnChangeEditSpecifiedTime)
	ON_EN_CHANGE(IDC_EDIT_AFTER_MINUTE, &CShutdownTimeDlg::OnEnChangeEditAfterMinute)
	ON_EN_UPDATE(IDC_EDIT_AFTER_MINUTE, &CShutdownTimeDlg::OnEnUpdateEditAfterMinute)
END_MESSAGE_MAP()


// CShutdownTimeDlg 메시지 처리기

BOOL CShutdownTimeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	int minutes = theApp.GetProfileInt(_T("setting"), _T("shutdown after minutes"), 0);
	CString hhmm = theApp.GetProfileString(_T("setting"), _T("shutdown specified time"), _T(""));
	bool use_specified_time = theApp.GetProfileInt(_T("setting"), _T("use specified time"), false);

	if (!use_specified_time && minutes > 0)
	{
		if (minutes % 5 == 0 && minutes >= 5 && minutes <= 60)
			CheckDlgButton(IDC_RADIO_05 + (minutes / 5 - 1), BST_CHECKED);

		m_edit_after_minute.SetWindowText(i2S(minutes));
	}
	else if (use_specified_time && !hhmm.IsEmpty())
	{
		m_edit_specified_time.SetWindowText(hhmm);
		m_radio_specified_time.SetCheck(BST_CHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CShutdownTimeDlg::OnBnClickedRadioMinutes(UINT id)
{
	CString str;
	GetDlgItemText(id, str);

	str = str.Left(str.Find(_T("분")));
	m_edit_after_minute.SetWindowText(str);
	m_radio_specified_time.SetCheck(BST_UNCHECKED);
}


void CShutdownTimeDlg::OnBnClickedRadioSpecifiedTime()
{
	CheckRadioButtons(this, IDC_RADIO_05, IDC_RADIO_60, -1, BST_UNCHECKED);
	m_edit_after_minute.SetWindowText(_T(""));
}


void CShutdownTimeDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_radio_specified_time.GetCheck() == BST_CHECKED)
	{
		m_edit_specified_time.GetWindowText(m_shutdown_time);
		theApp.WriteProfileString(_T("setting"), _T("shutdown specified time"), m_shutdown_time);
		theApp.WriteProfileInt(_T("setting"), _T("use specified time"), true);
	}
	else
	{
		CString minutes;

		m_edit_after_minute.GetWindowText(minutes);
		if (_ttoi(minutes) <= 0)
			return;

		theApp.WriteProfileInt(_T("setting"), _T("shutdown after minutes"), _ttoi(minutes));
		theApp.WriteProfileInt(_T("setting"), _T("use specified time"), false);

		CTime t = CTime::GetCurrentTime();
		CTimeSpan ts(0, 0, _ttoi(minutes), 0);
		t += ts;
		m_shutdown_time.Format(_T("%02d%02d"), t.GetHour(), t.GetMinute());
	}

	CDialogEx::OnOK();
}


void CShutdownTimeDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CShutdownTimeDlg::OnBnClickedButtonClear()
{
	m_shutdown_time.Empty();
	CDialogEx::OnOK();
}


void CShutdownTimeDlg::OnEnChangeEditAfterMinute()
{
	m_radio_specified_time.SetCheck(BST_UNCHECKED);
}


void CShutdownTimeDlg::OnEnChangeEditSpecifiedTime()
{
	CheckRadioButtons(this, IDC_RADIO_05, IDC_RADIO_60, -1, BST_UNCHECKED);
	m_edit_after_minute.SetWindowText(_T(""));
	m_radio_specified_time.SetCheck(BST_CHECKED);
}


void CShutdownTimeDlg::OnEnUpdateEditAfterMinute()
{}


BOOL CShutdownTimeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT_AFTER_MINUTE))
		{
			CheckRadioButtons(this, IDC_RADIO_05, IDC_RADIO_60, -1, BST_UNCHECKED);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
