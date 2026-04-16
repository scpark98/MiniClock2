// TimeListDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MiniClock2.h"
#include "afxdialogex.h"
#include "TimeListDlg.h"

#include "Common/MemoryDC.h"
#include "Common/messagebox/XMessageBox/XMessageBox.h"

// CTimeListDlg 대화 상자

IMPLEMENT_DYNAMIC(CTimeListDlg, CDialogEx)

CTimeListDlg::CTimeListDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TIME_LIST, pParent)
{

}

CTimeListDlg::~CTimeListDlg()
{
}

void CTimeListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ALARM_LIST, m_static_alarm_list);
	DDX_Control(pDX, IDC_CHECK_AUTO_HIDE, m_check_autohide);
	DDX_Control(pDX, IDC_LIST_TIME, m_list);
}


BEGIN_MESSAGE_MAP(CTimeListDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDOK, &CTimeListDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTimeListDlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	//ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	//ON_WM_NCACTIVATE()
END_MESSAGE_MAP()


// CTimeListDlg 메시지 처리기

BOOL CTimeListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LONG_PTR style = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);

	// 캡션 + 모든 테두리 제거
	style &= ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER | WS_DLGFRAME);
	style |= WS_THICKFRAME; // resize는 가능하도록 테두리는 남긴다.

	::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

	// 반드시 필요 (프레임 다시 계산)
	::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOACTIVATE | SWP_FRAMECHANGED);

	init_list();

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(452, 112));
	m_resize.Add(IDC_CHECK_AUTO_HIDE, 100, 0, 0, 0);
	m_resize.Add(IDC_LIST_TIME, 0, 0, 100, 100);

	m_static_alarm_list.set_color(GRAY128, GRAY32);
	m_static_alarm_list.set_font_weight();

	m_check_autohide.set_back_color(GRAY(32), false);
	m_check_autohide.set_text_color(GRAY128, lightblue, GRAY(212), GRAY(212));
	m_check_autohide.set_font_weight(FW_BOLD);
	m_check_autohide.SetCheck(theApp.GetProfileInt(_T("TimeListDlg"), _T("auto hide"), false));

	RestoreWindowPosition(&theApp, this, _T("TimeListDlg"));// , false, true, true);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CTimeListDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CTimeListDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
	return CDialogEx::OnEraseBkgnd(pDC);
}

void CTimeListDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this, _T("TimeListDlg"));
}

void CTimeListDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CTimeListDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

void CTimeListDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CTimeListDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DefWindowProc(WM_NCLBUTTONDOWN, HTCAPTION, MAKEWORD(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CTimeListDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CTimeListDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CTimeListDlg::init_list()
{
	m_list.SetExtendedStyle(/*LVS_EX_GRIDLINES | */LVS_EX_FLATSB | /*LVS_EX_CHECKBOXES |*/ LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_list.set_color_theme(CSCColorTheme::color_theme_dark_gray);//, false);
	m_list.set_headings(_T("이름,120;등록 시각,80;간격,44;알람 시각,80;남은 시각,80;등록 날짜,88"));

	m_list.allow_edit();
	m_list.set_font_name(theApp.GetProfileString(_T("TimeListDlg\\list\\setting"), _T("font name"), _T("맑은 고딕")));
	m_list.set_font_size(theApp.GetProfileInt(_T("TimeListDlg\\list\\setting"), _T("font size"), 8));
	m_list.set_line_height(18);// theApp.GetProfileInt(_T("TimeListDlg\\list\\setting"), _T("line height"), 20));
	m_list.restore_column_width(&theApp, _T("TimeListDlg\\list"));

	m_list.set_header_height(20);

	for (int i = col_start; i <= col_date; i++)
		m_list.set_column_text_align(i, LVCFMT_CENTER);

	m_list.set_column_data_type(col_duration, CVtListCtrlEx::column_data_type_numeric);

	m_list.set_draw_selected_border(false);
}
void CTimeListDlg::OnPaint()
{
	CPaintDC dc1(this);
	CRect rc;
	GetClientRect(rc);

	CMemoryDC dc(&dc1, &rc);
	dc.FillSolidRect(rc, RGB(32, 32, 32));
}

void CTimeListDlg::OnNcPaint()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CDialogEx::OnNcPaint()을(를) 호출하지 마십시오.
	CPaintDC dc(this);
	CRect rc;

	GetClientRect(rc);
	dc.FillSolidRect(rc, red);
}

void CTimeListDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (bCalcValidRects)// && m_caption_removed)
	{
		//NCCALCSIZE_PARAMS* pParams = (NCCALCSIZE_PARAMS*)lParam;

		// 👉 여기서 위쪽 잘라내기
		//pParams->rgrc[0].top += 1; // 또는 0~8 정도 조절
		lpncsp->rgrc[0].top -= 6;
		//lpncsp->rgrc[0].bottom += 6;
	}

	CDialogEx::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CTimeListDlg::OnNcActivate(BOOL bActive)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return TRUE;
	return CDialogEx::OnNcActivate(bActive);
}
