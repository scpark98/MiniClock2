// AddAlarmDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MiniClock2.h"
#include "afxdialogex.h"
#include "AddAlarmDlg.h"


// CAddAlarmDlg 대화 상자

IMPLEMENT_DYNAMIC(CAddAlarmDlg, CDialogEx)

CAddAlarmDlg::CAddAlarmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADD_ALARM, pParent)
{

}

CAddAlarmDlg::~CAddAlarmDlg()
{
}

void CAddAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NAME, m_edit_name);
	DDX_Control(pDX, IDC_EDIT_MINUTES, m_edit_minutes);
	DDX_Control(pDX, IDC_CHECK_ADD_FAVORITE, m_check_add_favorite);
	DDX_Control(pDX, IDC_CHECK_FLOATING, m_check_floating);
}


BEGIN_MESSAGE_MAP(CAddAlarmDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAddAlarmDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAddAlarmDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAddAlarmDlg 메시지 처리기

void CAddAlarmDlg::OnBnClickedOk()
{
	m_edit_name.GetWindowText(m_name);
	m_edit_minutes.GetWindowText(m_duration);
	m_add_favorite = m_check_add_favorite.GetCheck();
	m_as_floating = m_check_floating.GetCheck();

	CDialogEx::OnOK();
}

void CAddAlarmDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
