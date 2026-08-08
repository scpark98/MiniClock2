// TimeListDlg.cpp: 구현 파일
//

#include "pch.h"
#include "MiniClock2.h"
#include "MiniClock2Dlg.h"
#include "afxdialogex.h"
#include "TimeListDlg.h"

#include "Common/MemoryDC.h"
//#include "Common/messagebox/XMessageBox/XMessageBox.h"
#include "Common/log/SCLog/SCLog.h"		//20260801 by claude. [진단] alarm beep 원인 추적.

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
	ON_COMMAND_RANGE(menu_favorite_start, menu_favorite_start + 100, on_menu_favorites)
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	//ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_WM_ACTIVATEAPP()
	ON_BN_CLICKED(IDC_CHECK_AUTO_HIDE, &CTimeListDlg::OnBnClickedCheckAutoHide)
	ON_COMMAND(ID_MENU_DELETE, &CTimeListDlg::OnMenuDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TIME, &CTimeListDlg::OnNMDblclkListTime)
	ON_COMMAND(ID_MENU_RESET_START_TIME, &CTimeListDlg::OnMenuResetStartTime)
	ON_COMMAND(ID_MENU_FLOATING, &CTimeListDlg::OnMenuFloating)
	ON_COMMAND(ID_MENU_COPY_TO_CLIPBOARD, &CTimeListDlg::OnMenuCopyToClipboard)
	ON_COMMAND(ID_MENU_LOCK_LISTITEM, &CTimeListDlg::OnMenuLockListitem)
	ON_REGISTERED_MESSAGE(Message_CSCShapeDlg, &CTimeListDlg::on_message_CSCShapeDlg)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_TIME, &CTimeListDlg::OnLvnEndLabelEditListTime)
	ON_WM_ENTERSIZEMOVE()
	ON_WM_EXITSIZEMOVE()
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
	//OnPaint 가 client 전체를 GRAY(32) 로 칠하는데 WS_CLIPCHILDREN 이 없으면
	//자식(체크박스/리스트) 영역까지 덮어써버린다. 자식은 부모 invalidate 로는 자동 재그리지 않아
	//OnNcActivate 의 Invalidate() 후 체크박스 V/박스가 사라진 상태로 남는다.
	style |= WS_CLIPCHILDREN;

	::SetWindowLongPtr(m_hWnd, GWL_STYLE, style);

	//WS_EX_COMPOSITED ? OS-level 더블버퍼링. hide→show 시 OS 가 backbuffer 에 paint 완료 후 한 번에
	//compose 해서 흰색→dark theme flash 가 사라진다. WS_EX_TOOLWINDOW/TOPMOST 등과 병행 가능.
	ModifyStyleEx(0, WS_EX_COMPOSITED);

	// 반드시 필요 (프레임 다시 계산)
	::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
		SWP_NOACTIVATE | SWP_FRAMECHANGED);

	init_list();

	m_resize.Create(this);
	m_resize.SetMinimumTrackingSize(CSize(400, 100));
	m_resize.Add(IDC_CHECK_AUTO_HIDE, 100, 0, 0, 0);
	m_resize.Add(IDC_LIST_TIME, 0, 0, 100, 100);

	m_static_alarm_list.set_color(GRAY128, GRAY32);
	m_static_alarm_list.set_font_weight();

	//RGB()/GRAY() 매크로는 COLORREF (alpha=0). Gdiplus::Color 인자로 넘기면
	//ARGB 0x00xxxxxx → 알파 0 → GDI+ Pen/Brush 로 그리면 완전 투명. V 자 체크가 안 그려졌던 원인.
	//Pen/Brush 로 그려지는 항목은 반드시 gGRAY()/gRGB() (alpha=255) 를 써야 한다.
	m_check_autohide.set_back_color(gGRAY(32), false);
	m_check_autohide.set_text_color(gGRAY(128), false);// Gdiplus::Color::LightBlue, gGRAY(192), gGRAY(192));
	m_check_autohide.set_font_weight(FW_BOLD);
	m_check_autohide.SetCheck(theApp.GetProfileInt(_T("TimeListDlg"), _T("auto hide"), false));

	m_floating.set_text(this, _T(" "), 12, Gdiplus::FontStyle::FontStyleBold, 0.0f, 1.6f, _T("DSEG7 Classic"),//_T("맑은 고딕")),
		Gdiplus::Color(255, 128, 128, 192),
		Gdiplus::Color(255, 0, 0, 0),
		Gdiplus::Color(255, 64, 64, 64),
		Gdiplus::Color(1, 0, 0, 0));
	RestoreWindowPosition(&theApp, &m_floating, _T("TimeListDlg\\m_floating"), false, false, false);

	RestoreWindowPosition(&theApp, this, _T("TimeListDlg"));// , false, true, true);

	m_msgbox.create(this, _T("MiniClock2"), IDR_MAINFRAME);
	m_msgbox.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_msgbox.set_show_on_parent_center(false);

	load_timelist();

	SetTimer(timer_time, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CTimeListDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
			case VK_F2:
				::PostMessage(GetParent()->GetSafeHwnd(), pMsg->message, pMsg->wParam, pMsg->lParam);
				return true;
			case VK_DELETE:
				if (m_list.is_in_editing())
					return CDialogEx::PreTranslateMessage(pMsg);
				else
					OnMenuDelete();
				return true;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CTimeListDlg::OnEraseBkgnd(CDC* pDC)
{
	//OnPaint 와 동일 색으로 직접 erase. 기존 `return FALSE` 는 MFC 관례상 "erase 안 했음" 이라
	//DefWindowProc 가 dialog class brush (시스템 회색) 로 fill 해버려 hide→show 시점에 회색 flash
	//후 OnPaint 가 dark theme 으로 덮는 깜빡임이 생긴다.
	CRect rc;
	GetClientRect(rc);
	pDC->FillSolidRect(rc, RGB(32, 32, 32));
	return TRUE;
}

//프레임 드래그로 사용자가 resize 를 시작할 때 WS_EX_COMPOSITED 를 잠시 끈다.
//COMPOSITED 가 켜진 상태로 resize 하면 자식 컨트롤들(리스트 텍스트 등) 의 매 paint 마다 전체 윈도우
//compose 가 트리거돼 staggered flicker 가 보인다. show/hide 시점엔 다시 켜야 flash 가 안 생기므로
//resize 모달 루프 동안만 임시로 끔.
void CTimeListDlg::OnEnterSizeMove()
{
	ModifyStyleEx(WS_EX_COMPOSITED, 0);
	CDialogEx::OnEnterSizeMove();
}

void CTimeListDlg::OnExitSizeMove()
{
	ModifyStyleEx(0, WS_EX_COMPOSITED);
	CDialogEx::OnExitSizeMove();
}

void CTimeListDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	if (lpwndpos && (lpwndpos->flags & SWP_HIDEWINDOW))
		return;

	//부모(CMiniClock2Dlg)가 모니터 off 등으로 위치 저장을 lock 한 상태면 저장 skip.
	//OS 가 visible 모니터로 자동 reposition 한 좌표가 레지스트리에 굳어지는 것을 막는다.
	CMiniClock2Dlg* main = (CMiniClock2Dlg*)GetParent();
	if (main && main->is_position_save_locked())
		return;

	//현재 위치가 어떤 visible 모니터의 작업 영역에 충분히 들어가 있을 때만 저장.
	CRect rc;
	GetWindowRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0)
		return;

	HMONITOR hMon = MonitorFromRect(&rc, MONITOR_DEFAULTTONULL);
	if (hMon == NULL)
		return;

	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hMon, &mi))
		return;

	CRect inter;
	if (!inter.IntersectRect(rc, CRect(mi.rcWork)) || inter.Width() < 20 || inter.Height() < 20)
		return;

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
	// 안전망: 변경 시점마다 저장하지만 누락 경로가 있을 수 있으므로 종료 시 한 번 더.
	// 아래 delete 루프가 item 들을 해제하므로 반드시 그 전에 호출.
	save_timelist();

	m_list.save_column_width(&theApp, _T("TimeListDlg\\list"));

	//이미 m_floating는 파괴된 상태이므로 여기서 저장해선 안된다.
	//main dlg가 파괴될 때 m_floating도 같이 파괴되도록 설정되어 있으므로 main dlg의 OnDestroy()에서 저장하도록 한다.
	//또는 CSCShapeDlg에서 이동 시 메시지를 전달해서 여기서 저장할수도 있다.
	//SaveWindowPosition(&theApp, &m_floating, _T("TimeListDlg\\m_floating"));

	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		auto item = (CAlarmItem*)m_list.GetItemData(i);
		if (item)
			delete item;
	}

	CDialogEx::OnDestroy();
}

void CTimeListDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DefWindowProc(WM_NCLBUTTONDOWN, HTCAPTION, MAKEWORD(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CTimeListDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	CMenu* pMenu;

	menu.LoadMenu(IDR_MENU_TIME_LIST);
	pMenu = (CMenu*)menu.GetSubMenu(0);

	int count = theApp.GetProfileInt(_T("favorite"), _T("count"), 0);
	if (count > 0)
	{
		pMenu->AppendMenuW(MF_SEPARATOR);

		CString str;
		CString caption;

		for (int i = 0; i < count; i++)
		{
			str = theApp.GetProfileString(_T("favorite"), i2S(i, false, true, 3), _T(""));

			std::deque<CString> token;
			get_token_str(str, token, _T("|"));

			if (token.size() < 2)
				continue;

			caption.Format(_T("%s (%s%s) (&%d)"), token[0], token[1], (token[1].Find(_T(":")) > 0) ? _T("") : _T("분"), i + 1);
			pMenu->AppendMenu(MF_STRING, menu_favorite_start + i, caption);
		}
	}

	int index = m_list.get_selected_index();

	if (index >= 0)
	{
		auto item = (CAlarmItem*)m_list.GetItemData(index);

		if (item->is_floating)
			pMenu->CheckMenuItem(ID_MENU_FLOATING, MF_CHECKED);

		if (item->is_locked)
		{
			pMenu->CheckMenuItem(ID_MENU_LOCK_LISTITEM, MF_CHECKED);
			pMenu->EnableMenuItem(ID_MENU_RESET_START_TIME, MF_DISABLED);
		}
		else
		{
			pMenu->CheckMenuItem(ID_MENU_LOCK_LISTITEM, MF_UNCHECKED);
		}
	}
	else
	{
		pMenu->EnableMenuItem(ID_MENU_FLOATING, MF_DISABLED);
		pMenu->EnableMenuItem(ID_MENU_RESET_START_TIME, MF_DISABLED);
		pMenu->EnableMenuItem(ID_MENU_LOCK_LISTITEM, MF_DISABLED);
		pMenu->EnableMenuItem(ID_MENU_COPY_TO_CLIPBOARD, MF_DISABLED);
		pMenu->EnableMenuItem(ID_MENU_DELETE, MF_DISABLED);
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
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

	//m_list.set_column_data_type(col_duration, CSCListCtrl::column_data_type_numeric);
	//남은 시각은 "HH:MM:SS" 또는 "-HH:MM:SS" 형식으로 _ttof 가 선두 시각값을 파싱.
	//텍스트 정렬이면 "100:00:00" 이 "23:00:00" 보다 앞으로 오는 등 어긋난다.
	//m_list.set_column_data_type(col_remain, CSCListCtrl::column_data_type_numeric);

	m_list.set_draw_selected_border(false);
	m_list.set_use_own_context_menu(false);
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

		// ?? 여기서 위쪽 잘라내기
		//pParams->rgrc[0].top += 1; // 또는 0~8 정도 조절
		lpncsp->rgrc[0].top -= 6;
		//lpncsp->rgrc[0].bottom += 6;
	}

	CDialogEx::OnNcCalcSize(bCalcValidRects, lpncsp);
}

BOOL CTimeListDlg::OnNcActivate(BOOL bActive)
{
	Invalidate();
	UpdateWindow();

	//return TRUE;를 할 경우 비활성화 될 때 여전히 상단 바가 남는다. FALSE로 하니 남는 버그가 사라짐.
	//return FALSE;를 할 경우 흰색바는 사라지지만 다른 dlg가 입력이벤트를 전혀 처리하지 못하는 현상이 발생함.
	return TRUE;// FALSE;
	return CDialogEx::OnNcActivate(bActive);
}

void CTimeListDlg::add(CString title, CString duration, bool add_favorite, bool floating, bool save_list)
{
	CTime tStart;
	CTimeSpan ts_duration;
	CString	sDate;
	CString sEnd;
	CString str;

	tStart = CTime::GetCurrentTime();

	//title이 비어있다면 "제목없음"으로 설정한다.
	if (title.IsEmpty())
	{
		int no_title_count = 0;
		for (int i = 0; i < m_list.size(); i++)
		{
			if (m_list.get_text(i, col_title).Find(_T("제목없음")) >= 0)
				no_title_count++;
		}

		title.Format(_T("제목없음%d"), no_title_count);
	}

	//hhmm 형식으로 입력되면 이는 분 단위가 아니라 hh:mm의 의미로 입력했다고 보고 ':'을 추가해준다.
	if (duration.GetLength() == 4 && IsNatural(duration))
	{
		duration.Insert(2, _T(":"));
	}

	//만약 알람시간을 표시하는 형식인 hh:mm라면 (hh:mm:ss로 입력해도 초는 무시된다)
	//hh:mm, h:mm, hh:m, h:m 모두 가능하다.
	if (get_char_count(duration, ':') >= 1)
	{
		//h:m, h:mm, hh:m, hh:mm 형식으로 입력된 경우라면
		std::deque<CString> token;
		int hour = 0;
		int minute = 0;

		get_token_str(duration, token, _T(":"), false);

		//20260802 by claude. get_token_str 은 allowEmpty=false 라 빈 토큰을 버린다. ":" 만 입력하면
		//토큰이 0개가 되어 token[0] 이 범위 밖 접근이 된다(Debug 어서션 / Release 접근 위반).
		if (token.empty())
		{
			m_msgbox.DoModal(_T("간격을 인식할 수 없습니다.\n\n분 단위(30), hh:mm(9:30), 1d 2h 3m 형식으로 입력하세요."));
			return;
		}

		hour = _ttoi(token[0]);
		if (token.size() > 1)
			minute = _ttoi(token[1]);

		sEnd.Format(_T("%02d:%02d:00"), hour, minute);
		CTime tEnd = get_CTime_from_datetime_str(_T(""), sEnd);
		ts_duration = tEnd - tStart;

		//지난 시간이라도 즐겨찾기 등록일 경우는 등록되어야 하므로 여기서 자르면 안된다. 일단 추가한다.
		//어짜피 리스트에서 걸러진다.
		/*
		if (ts_duration.GetTotalSeconds() < 0)
		{
			str.Format(_T("%s는 이미 지난 시각입니다."), sEnd);
			m_msgbox.DoModal(str, MB_OK, 3);
			return;
		}
		*/

		//if (ts_duration.GetTotalMinutes() > 60)
		//	duration.Format(_T("%dh %dm"), ts.GetHours(), ts.GetMinutes());
		//else
		//	duration.Format(_T("%dm"), ts.GetMinutes());
	}
	//알람시간이 아닌 알람시간까지의 남은 시간을 "1h 23m" 또는 "1.23"과 같은 형식으로 입력한 경우라면
	else
	{
		int minutes = get_minutes_from_duration_string(duration);

		//20260802 by claude. 파싱 실패(-1) 를 검사하지 않아 1분 전에 만료된 알람이 조용히 등록됐다.
		if (minutes < 0)
		{
			m_msgbox.DoModal(_T("간격을 인식할 수 없습니다.\n\n분 단위(30), hh:mm(9:30), 1d 2h 3m 형식으로 입력하세요."));
			return;
		}

		ts_duration = CTimeSpan(0, 0, minutes, 0);
		sEnd = get_time_str(tStart + ts_duration);
	}

	if (add_favorite)
	{
		CString value;
		value.Format(_T("%s|%s"), title, duration);

		int count = theApp.GetProfileInt(_T("favorite"), _T("count"), 0);
		theApp.WriteProfileString(_T("favorite"), i2S(count, false, true, 3), value);
		theApp.WriteProfileInt(_T("favorite"), _T("count"), count + 1);
	}

	//알람 시간이 명시됐다면 sEnd가 채워져 있으므로 여기서는 스킵되고
	//남은 시간이 명시됐다면 sEnd를 계산해준다.
	sDate = get_date_str(tStart);

	//기존 항목 중 floating 인 것이 없다면 추가되는 항목을 자동으로 floating 으로 설정한다.
	//(빈 리스트도 자동 포함됨)
	bool any_floating = false;
	for (int i = 0; i < m_list.size(); i++)
	{
		auto item = (CAlarmItem*)m_list.GetItemData(i);
		if (item && item->is_floating)
		{
			any_floating = true;
			break;
		}
	}
	if (!any_floating)
		floating = true;

	int index = m_list.insert_item(-1, 0, title, get_time_str(tStart), get_time_str(ts_duration.GetTotalSeconds()), sEnd, _T(""), sDate);

	//floating은 단 1 항목만 가능하다.
	if (floating)
	{
		for (int i = 0; i < m_list.size(); i++)
		{
			auto item = (CAlarmItem*)m_list.GetItemData(i);
			if (item)
				item->is_floating = false;
		}
	}

	CAlarmItem* data = new CAlarmItem(title, tStart, ts_duration, add_favorite, floating);
	m_list.SetItemData(index, reinterpret_cast<DWORD_PTR>(data));

	if (save_list)
		save_timelist();

	//전 항목 col_remain 동기화 후 정렬. (load 직후 등 stale 인 경우도 안전)
	refresh_remain_and_sort();
}

//1:13(=1h 13m), 1d 20m(=1일 20분) 등의 문자열을 총 minutes로 변환한다.
int	CTimeListDlg::get_minutes_from_duration_string(CString& duration)
{
	int minutes = 0;

	duration.Replace(_T(","), _T(":"));
	duration.Replace(_T(";"), _T(":"));

	//우선 "1.23" 또는 "1 23"과 같이 입력되면 "1h 23m"과 같은 정형화 된 형식으로 변환시켜준다.
	//그래야만 이 함수를 호출한 곳에서 리스트에 추가할 때도 그 형식으로 추가된다.
	if (get_char_count(duration, '.') >= 1 || get_char_count(duration, ' ') >= 1)
	{
		CString sep = _T(".");

		if (get_char_count(duration, ' ') >= 1)
			sep = _T(" ");

		std::deque<CString> token;
		get_token_str(duration, token, sep, false, 2);
		if (token.size() == 2)
		{
			duration.Format(_T("%dh %dm"), _ttoi(token[0]), _ttoi(token[1]));
		}
	}

	//1d 2h 3m과 같은 형식으로 입력한 경우
	if (get_char_count(duration, 'd') == 1 ||
		get_char_count(duration, 'h') == 1 ||
		get_char_count(duration, 'm') == 1)
	{
		minutes = get_total_minutes_from_dhm(duration);
	}
	//1:23 또는 1:23:45 과 같은 입력은 분까지만 잘라준다.
	else if (get_char_count(duration, ':') >= 1)
	{
		std::deque<CString> token;
		get_token_str(duration, token, _T(":"), false, 2);

		//20260802 by claude. 위에서 ',' / ';' 를 ':' 로 치환하므로 "," "5," 같은 입력이 여기로 온다.
		//빈 토큰은 버려지므로 토큰이 0~1개일 수 있고, 무검사로 token[1] 을 읽으면 범위 밖 접근.
		if (token.empty())
			return -1;

		minutes = _ttoi(token[0]) * 60 + (token.size() > 1 ? _ttoi(token[1]) : 0);
	}
	//분 단위로만 입력한 경우
	else
	{
		if (!IsNatural(duration))
		{
			//AfxMessageBox(_T("분 단위 또는 hh:mm 또는 1d 2h 3m과 같은 형식으로 입력하세요."), MB_ICONEXCLAMATION);
			return -1;
		}

		minutes = _ttoi(duration);
	}

	return minutes;
}

//20260802 by claude. 구버전은 CAlarmItem 구조체를 통째로 REG_BINARY 로 저장했다. 그 방식은
//(a) 멤버가 하나만 추가돼도 sizeof 가 바뀌어 기존 항목이 전부 버려지고
//(b) CString 같은 non-POD 멤버를 넣는 순간 blob 을 캐스팅해 읽는 코드가 쓰레기 포인터를 역참조하며
//(c) GetProfileBinary 가 new BYTE[] 로 준 버퍼를 delete 로 해제하는 불일치를 남겼다.
//문자열 직렬화로 바꿔 셋 다 제거한다.
CString CAlarmItem::to_string() const
{
	CString str;
	str.Format(_T("%I64d|%I64d|%d|%d|%d|%d|%s"),
		(__int64)start.GetTime(),
		(__int64)ts_duration.GetTotalSeconds(),
		(int)is_locked,
		(int)is_floating,
		(int)is_paused,
		(int)fired,
		title.GetString());

	return str;
}

bool CAlarmItem::from_string(const CString& str)
{
	//title 은 마지막 필드이므로 앞쪽 구분자 6개만 찾고 나머지는 통째로 title 로 둔다.
	//이렇게 해야 title 에 '|' 가 들어가도 파싱이 깨지지 않는다.
	int pos[6] = { 0, };
	int from = 0;

	for (int i = 0; i < _countof(pos); i++)
	{
		pos[i] = str.Find(_T('|'), from);
		if (pos[i] < 0)
			return false;

		from = pos[i] + 1;
	}

	start = CTime((__time64_t)_ttoi64(str.Left(pos[0])));
	ts_duration = CTimeSpan((__time64_t)_ttoi64(str.Mid(pos[0] + 1, pos[1] - pos[0] - 1)));
	is_locked = (_ttoi(str.Mid(pos[1] + 1, pos[2] - pos[1] - 1)) != 0);
	is_floating = (_ttoi(str.Mid(pos[2] + 1, pos[3] - pos[2] - 1)) != 0);
	is_paused = (_ttoi(str.Mid(pos[3] + 1, pos[4] - pos[3] - 1)) != 0);
	fired = (_ttoi(str.Mid(pos[4] + 1, pos[5] - pos[4] - 1)) != 0);
	title = str.Mid(pos[5] + 1);

	return true;
}

void CTimeListDlg::migrate_timelist_from_binary()
{
	//구버전 레이아웃 재현. 현재 CAlarmItem 은 CString 을 가지므로 blob 을 그대로 캐스팅할 수 없다.
	//당시의 멤버 순서·타입을 그대로 두어야 sizeof 와 오프셋이 맞는다.
	struct alarm_item_binary
	{
		TCHAR		title[16];
		CTime		start;
		CTimeSpan	ts_duration;
		bool		is_locked;
		bool		is_floating;
		bool		is_paused;
	};

	int count = theApp.GetProfileInt(_T("TimeListDlg"), _T("count"), 0);
	int migrated = 0;

	//count 범위 밖에도 과거에 항목 수가 줄면서 남은 잔재 키가 있다(실제로 count=7 인데 item10 까지 존재).
	//구버전 save 가 지우지 않았던 것들이라 여기서 같이 정리한다. 키 형식이 item%02d 라 상한은 100.
	for (int i = 0; i < 100; i++)
	{
		CString key;
		key.Format(_T("item%02d"), i);

		LPBYTE blob = nullptr;
		UINT sz = 0;
		if (!theApp.GetProfileBinary(_T("TimeListDlg"), key, &blob, &sz))
			continue;

		if (i >= count)
		{
			//리스트에 올라오지 않는 잔재. 변환하지 않고 삭제만 한다.
			logWrite(_T("[timelist] migrate item[%02d]: count(%d) 밖의 잔재 — 삭제"), i, count);
		}
		else if (sz == sizeof(alarm_item_binary))
		{
			alarm_item_binary* old = (alarm_item_binary*)blob;
			//blob 이 종단되지 않은 상태일 수 있다. CString 으로 옮기기 전에 강제 종단.
			old->title[_countof(old->title) - 1] = 0;

			CAlarmItem item(old->title, old->start, old->ts_duration, old->is_locked, old->is_floating);
			item.is_paused = old->is_paused;

			CString new_key;
			new_key.Format(_T("alarm%02d"), i);
			theApp.WriteProfileString(_T("TimeListDlg"), new_key, item.to_string());
			migrated++;
		}
		else
		{
			logWrite(_T("[timelist] migrate item[%02d]: size mismatch (got=%u expected=%u) — 버림"),
				i, sz, (UINT)sizeof(alarm_item_binary));
		}

		delete[] blob;
		theApp.WriteProfileString(_T("TimeListDlg"), key, NULL);
	}

	theApp.WriteProfileInt(_T("schema"), _T("timelist_version"), 1);
	logWrite(_T("[timelist] migrate: count=%d migrated=%d"), count, migrated);
}

void CTimeListDlg::load_timelist()
{
	if (theApp.GetProfileInt(_T("schema"), _T("timelist_version"), 0) < 1)
		migrate_timelist_from_binary();

	int count = theApp.GetProfileInt(_T("TimeListDlg"), _T("count"), 0);

	//20260801 by claude. [진단] 시작 시 registry 에서 불러온 원본 count.
	logWrite(_T("[timelist] load_timelist begin: registry count=%d"), count);

	for (int i = 0; i < count; i++)
	{
		CString key;
		key.Format(_T("alarm%02d"), i);

		CString value = theApp.GetProfileString(_T("TimeListDlg"), key, _T(""));
		if (value.IsEmpty())
		{
			//20260801 by claude. [진단] count 는 있는데 해당 key 가 없거나 read 실패.
			logWrite(_T("[timelist] load item[%02d]: 값 없음"), i);
			continue;
		}

		CAlarmItem* item = new CAlarmItem;
		if (!item->from_string(value))
		{
			//20260802 by claude. [진단] 손상된 값. 이 항목만 버리고 나머지는 계속 읽는다.
			logWrite(_T("[timelist] load item[%02d]: 파싱 실패 — %s"), i, value.GetString());
			delete item;
			continue;
		}

		int index = m_list.insert_item(-1, 0, item->title,
										get_time_str(item->start),
										get_time_str(item->ts_duration),
										get_time_str(item->start + item->ts_duration),
										_T(""),
										get_date_str(item->start));
		m_list.SetItemData(index, reinterpret_cast<DWORD_PTR>(item));

		//20260801 by claude. [진단] 각 아이템의 title / start / end / locked / floating.
		CTime end = item->start + item->ts_duration;
		logWrite(_T("[timelist] load item[%02d]: title=%s start=%s end=%s locked=%d floating=%d fired=%d insert_index=%d"),
			i, item->title.GetString(),
			get_time_str(item->start).GetString(),
			get_time_str(end).GetString(),
			(int)item->is_locked, (int)item->is_floating, (int)item->fired, index);
	}

	//20260801 by claude. [진단] 최종 m_list.size() vs registry count 일치 여부.
	logWrite(_T("[timelist] load_timelist end: m_list.size()=%d"), (int)m_list.size());

	ensure_floating();
}

void CTimeListDlg::ensure_floating()
{
	//남은시간이 가장 짧은 *양의* 항목 1개만 floating, 나머지는 모두 floating 해제.
	//양의 remain 항목이 하나도 없으면 어떤 항목도 floating 되지 않는다.
	if (m_list.size() == 0)
		return;

	CTime now = CTime::GetCurrentTime();
	int target = -1;
	LONGLONG min_remain = 0;

	for (int i = 0; i < m_list.size(); i++)
	{
		auto* it = (CAlarmItem*)m_list.GetItemData(i);
		if (!it)
			continue;
		LONGLONG s = ((it->start + it->ts_duration) - now).GetTotalSeconds();
		if (s < 0)
			continue;
		if (target < 0 || s < min_remain)
		{
			target = i;
			min_remain = s;
		}
	}

	bool changed = false;
	for (int i = 0; i < m_list.size(); i++)
	{
		auto* it = (CAlarmItem*)m_list.GetItemData(i);
		if (!it)
			continue;
		bool want = (i == target);
		if (it->is_floating != want)
		{
			it->is_floating = want;
			changed = true;
		}
	}

	if (changed)
		save_timelist();
}

void CTimeListDlg::refresh_remain_and_sort()
{
	//col_remain 은 default text 컬럼이라 CSCListCtrl::sort 의 _ttof / 사전식 비교로는
	//"HH:MM:SS" 형식을 정확히 줄세울 수 없다. 따라서 직접 std::sort + lambda 로 정렬한다.
	//키는 (start + ts_duration - now) 의 total seconds. 가장 가까운 알람이 위로 (오름차순).
	int n = m_list.size();
	if (n == 0)
		return;

	CTime now = CTime::GetCurrentTime();

	//20260802 by claude. 남은 시간이 다시 양수가 된 항목(현재 시각으로 다시 시작, 시작·간격·종료 시각
	//편집)은 아직 울리지 않은 상태로 되돌린다. fired 가 남아 있으면 그 알람은 영영 울리지 않는다.
	bool fired_cleared = false;
	for (int i = 0; i < n; i++)
	{
		auto* it = (CAlarmItem*)m_list.GetItemData(i);
		if (!it || !it->fired)
			continue;

		if (((it->start + it->ts_duration) - now).GetTotalSeconds() > 0)
		{
			it->fired = false;
			fired_cleared = true;
		}
	}

	if (fired_cleared)
		save_timelist();

	//항목이 1개뿐이면 정렬은 의미 없지만 floating 보장은 필요.
	if (n == 1)
	{
		ensure_floating();
		return;
	}

	struct Row
	{
		LONGLONG		remain_secs;
		CAlarmItem*		item;
	};
	std::vector<Row> rows;
	rows.reserve(n);

	for (int i = 0; i < n; i++)
	{
		auto* it = (CAlarmItem*)m_list.GetItemData(i);
		if (!it)
			continue;
		CTimeSpan remain = (it->start + it->ts_duration) - now;
		rows.push_back({ remain.GetTotalSeconds(), it });
	}

	std::sort(rows.begin(), rows.end(),
		[](const Row& a, const Row& b)
		{
			return a.remain_secs < b.remain_secs;
		});

	m_list.SetRedraw(FALSE);

	//정렬된 순서대로 리스트 재구성. delete_all_items 는 m_list_db 만 비우고
	//각 라인의 data(=CAlarmItem*) 는 해제하지 않으므로 포인터 그대로 재삽입 가능.
	m_list.delete_all_items();
	for (const auto& r : rows)
	{
		int idx = m_list.insert_item(-1, 0,
			r.item->title,
			get_time_str(r.item->start),
			get_time_str(r.item->ts_duration),
			get_time_str(r.item->start + r.item->ts_duration),
			get_time_str(r.remain_secs),
			get_date_str(r.item->start));
		m_list.SetItemData(idx, reinterpret_cast<DWORD_PTR>(r.item));
	}

	m_list.SetRedraw(TRUE);

	ensure_floating();
}

void CTimeListDlg::save_timelist()
{
	int prev_count = theApp.GetProfileInt(_T("TimeListDlg"), _T("count"), 0);
	int count = m_list.size();

	theApp.WriteProfileInt(_T("TimeListDlg"), _T("count"), count);

	//20260801 by claude. [진단] save 시점의 count.
	logWrite(_T("[timelist] save_timelist: count=%d"), count);

	for (int i = 0; i < count; i++)
	{
		CString key;
		key.Format(_T("alarm%02d"), i);

		CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(i);
		if (!item)
		{
			//20260801 by claude. [진단] m_list 에 있지만 item 데이터가 NULL — 이런 상태로 write 되지 않음.
			logWrite(_T("[timelist] save item[%02d]: NULL data (not written)"), i);
			continue;
		}

		theApp.WriteProfileString(_T("TimeListDlg"), key, item->to_string());

		//20260801 by claude. [진단] 저장된 각 아이템의 title / end / locked.
		CTime end = item->start + item->ts_duration;
		logWrite(_T("[timelist] save item[%02d]: title=%s end=%s locked=%d floating=%d fired=%d"),
			i, item->title.GetString(),
			get_time_str(end).GetString(),
			(int)item->is_locked, (int)item->is_floating, (int)item->fired);
	}

	//20260802 by claude. 항목이 줄었을 때 남는 잔재 키를 지운다. 남겨두면 이후 count 가 다시 늘었을 때
	//그 자리의 쓰기가 실패하면 옛 항목이 되살아난다.
	for (int i = count; i < prev_count; i++)
	{
		CString key;
		key.Format(_T("alarm%02d"), i);
		theApp.WriteProfileString(_T("TimeListDlg"), key, NULL);
	}
}

LRESULT CTimeListDlg::OnNcHitTest(CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	LRESULT result = CDialogEx::OnNcHitTest(point);

	if (result == HTCLIENT)
	{
		ScreenToClient(&point);
		if (point.y < 4)
			return HTTOP;
	}

	return result;


	return CDialogEx::OnNcHitTest(point);
}

void CTimeListDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CTime	t = CTime::GetCurrentTime();
	bool	has_floating = false;

	for (int i = 0; i < m_list.size(); i++)
	{
		//if (!m_item[i].is_paused)
		CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(i);

		//20260802 by claude. save_timelist 가 NULL data 경로를 로그로 남기고 있어 발생 가능한 상태다.
		if (!item)
			continue;

		has_floating = has_floating || item->is_floating;
		//sctrace(item->start);
		//sctrace(item->ts_duration);
		//TRACE(_T("start: %s, duration: %s\n"), get_time_str(item->start), get_time_str(item->ts_duration));
		CTime end = item->start + item->ts_duration;
		CTimeSpan remain = end - t;
		LONGLONG remain_seconds = remain.GetTotalSeconds();
		CString str;

		//24시간 넘게 남았다면 "n일 m시간"으로 표기.
		if (remain.GetTotalHours() > 24)
			str.Format(_T("%d일 %d시간"), remain.GetDays(), remain.GetHours());
		else
			str = get_time_str(remain.GetTotalSeconds());

		//남은 시간에 관계없이 남은 시각은 계속 변경되어야 한다.
		if (item->is_floating)
		{
			CSCShapeDlgTextSetting* setting = m_floating.get_text_setting();
			setting->text = str;
			m_floating.set_text(setting);
#ifdef _DEBUG
			//m_floating.save(_T("D:\\floating.png"));
#endif
		}

		//역시 남은 시각도 계속 변경되어야 한다.
		m_list.set_text(i, col_remain, get_time_str(remain_seconds));


		//해당 시각이면 알림을 띠워주고
		//20260802 by claude. remain == 0 정확 일치는 그 1초에 tick 이 들어오지 못하면(절전 복귀·
		//시각 점프·1초 이상 메시지 펌프 정지) 알람이 영영 울리지 않는다. 지났으면 발동한다.
		//단 alarm_expire_seconds 를 넘겨 지난 것은 울리지 않는다 — 밤새 절전이었다면 그 사이 만료된
		//알람들이 깨어난 순간 한꺼번에 뜬다. 아래 자동 삭제와 같은 기준이라 그대로 목록에서 사라진다.
		if (!item->fired && remain_seconds <= 0 && remain_seconds > -alarm_expire_seconds)
		{
			//fired 를 먼저 세우고 저장한다. 아래 DoModal 이 메시지를 펌프하는 동안 이 OnTimer 가
			//재진입해도 중복 발화하지 않고, 모달이 떠 있는 사이에 앱이 죽어도 다시 울리지 않는다.
			item->fired = true;
			save_timelist();

			//20260801 by claude. [진단] beep 발동 확정 — 이 라인이 로그에 없으면 소리는 이 앱이 아님.
			logWrite(_T("[timelist] BEEP fire: idx=%d title=%s end=%s remain=%I64d locked=%d floating=%d"),
				i, item->title.GetString(),
				get_time_str(end).GetString(),
				remain_seconds,
				(int)item->is_locked, (int)item->is_floating);

			::MessageBeep(MB_ICONEXCLAMATION);
			m_msgbox.DoModal(item->title, MB_OK, 10);
		}
		else if (remain_seconds < 0)
		{
			//초과된 항목은 색상을 붉게 표시하고
			m_list.set_text_color(i, -1, Gdiplus::Color(128, 96, 16));
			if (item->is_floating)
			{
				m_floating.set_text_color(Gdiplus::Color(128, 128, 96, 16));
			}

			//10분이 지났다면 목록에서 완전 삭제한다.
			if (remain_seconds < -alarm_expire_seconds)
			{
				//20260801 by claude. [진단] 10 분 경과 auto-delete.
				//20260802 by claude. fired=0 으로 삭제된 항목 = 울리지 못한 채 만료 (절전·앱 종료 구간).
				logWrite(_T("[timelist] auto-delete (>10min past): idx=%d title=%s end=%s fired=%d"),
					i, item->title.GetString(), get_time_str(end).GetString(), (int)item->fired);

				if (item->is_floating)
					has_floating = false;

				delete item;
				m_list.delete_item(i);
				ensure_floating();
				i--;
				save_timelist();
			}
		}
		else
		{
			if (item->is_floating)
			{
				m_list.set_text_color(i, -1, gRGB(96, 128, 128));
				m_floating.set_text_color(Gdiplus::Color(255, 128, 128, 192));
			}
			else if (item->is_locked)
			{
				m_list.set_text_color(i, -1, Gdiplus::Color::DimGray);
			}
			else
			{
				m_list.set_text_color(i, -1, listctrlex_unused_color);
			}
		}
	}

	int day_of_week = t.GetDayOfWeek();
	if ((day_of_week == 2 || day_of_week == 6) && (t.GetHour() == 8 && t.GetMinute() == 55 && t.GetSecond() == 0))
	{
		m_msgbox.DoModal(_T("특정 요일 및 시각 알림!"));
	}

	m_floating.ShowWindow(has_floating ? SW_SHOW : SW_HIDE);

	CDialogEx::OnTimer(nIDEvent);
}

void CTimeListDlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CDialogEx::OnActivateApp(bActive, dwThreadID);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (GetOwner())
		GetOwner()->SendMessage(WM_ACTIVATEAPP, (WPARAM)bActive, (LPARAM)dwThreadID);
}

void CTimeListDlg::OnBnClickedCheckAutoHide()
{
	theApp.WriteProfileInt(_T("TimeListDlg"), _T("auto hide"), m_check_autohide.GetCheck());
}

void CTimeListDlg::OnNMDblclkListTime(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (pNMItemActivate->iItem < 0)
	{
		((CMiniClock2Dlg*)GetParent())->OnMenuAlarmAfterMinutes();
	}

	*pResult = 0;
}

void CTimeListDlg::OnMenuResetStartTime()
{
	int selected = m_list.get_selected_index();
	if (selected < 0)
		return;

	CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(selected);
	if (item->is_locked)
	{
		m_msgbox.set_message(_T("잠긴 항목입니다."), MB_OK, 1);
		return;
	}

	item->start = CTime::GetCurrentTime();
	m_list.set_text(selected, col_start, get_time_str(item->start));

	save_timelist();
	refresh_remain_and_sort();
}

void CTimeListDlg::OnMenuFloating()
{
	int selected = m_list.get_selected_index();
	if (selected < 0)
		return;

	CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(selected);
	item->is_floating = !item->is_floating;

	//선택 이외 항목들은 floating false로 만든다.
	for (int i = 0; i < m_list.size(); i++)
	{
		if (i != selected)
		{
			item = (CAlarmItem*)m_list.GetItemData(i);
			item->is_floating = false;
		}
	}

	save_timelist();
}

void CTimeListDlg::OnMenuCopyToClipboard()
{
	if (m_list.size() == 0)
		return;

	std::deque<CString> dq;
	m_list.get_selected_items(&dq);

	if (dq.empty())
	{
		for (int i = 0; i < m_list.size(); i++)
			dq.push_back(m_list.get_line_text(i, 0, -1));
	}

	CString str;

	str = m_list.get_header_text() + _T("\r\n");

	for (const auto& item : dq)
	{
		str += item + _T("\r\n");
	}

	copy_to_clipboard(m_hWnd, str);
	MessageBeep(MB_OK);
}

void CTimeListDlg::OnMenuLockListitem()
{
	int selected = m_list.get_selected_index();
	if (selected < 0)
		return;

	CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(selected);
	item->is_locked = !item->is_locked;

	save_timelist();
}

void CTimeListDlg::OnMenuDelete()
{
	std::deque<int> selected;
	m_list.get_selected_items(&selected);

	//20260801 by claude. [진단] delete 요청 시점 상태.
	int size_before = (int)m_list.size();
	int locked_skipped = 0;
	int actually_deleted = 0;
	logWrite(_T("[timelist] OnMenuDelete begin: selected=%d list.size=%d"),
		(int)selected.size(), size_before);

	//중간 항목이 삭제되어도 인덱스가 유지되도록 뒤에서부터 삭제한다.
	for (int i = selected.size() - 1; i >= 0; i--)
	{
		CAlarmItem* item = (CAlarmItem*)m_list.GetItemData(selected[i]);

		if (item->is_locked)
		{
			//20260801 by claude. [진단] 잠금 항목 스킵 — 사용자 인지 여부 확인용.
			logWrite(_T("[timelist] OnMenuDelete skip locked: idx=%d title=%s"),
				selected[i], item->title.GetString());
			locked_skipped++;
			continue;
		}

		if (item->is_floating)
			m_floating.ShowWindow(SW_HIDE);

		//20260801 by claude. [진단] 삭제 대상 identify.
		CTime end = item->start + item->ts_duration;
		logWrite(_T("[timelist] OnMenuDelete delete: idx=%d title=%s end=%s"),
			selected[i], item->title.GetString(), get_time_str(end).GetString());

		delete item;
		m_list.delete_item(selected[i]);
		actually_deleted++;
	}

	//20260801 by claude. [진단] delete_item 회귀 검증 — size 가 요청만큼 감소했는지.
	int size_after = (int)m_list.size();
	logWrite(_T("[timelist] OnMenuDelete end: locked_skipped=%d deleted=%d size_before=%d size_after=%d (expected_after=%d)"),
		locked_skipped, actually_deleted, size_before, size_after, size_before - actually_deleted);

	save_timelist();
	//남은 항목 중 가장 임박한(양의) 항목을 floating 으로 승격.
	ensure_floating();
}

LRESULT CTimeListDlg::on_message_CSCShapeDlg(WPARAM wParam, LPARAM lParam)
{
	CSCShapeDlgMessage* msg = (CSCShapeDlgMessage*)wParam;
	if (msg && msg->message == CSCShapeDlg::message_window_pos_changed && msg->pThis == &m_floating)
	{
		//메인의 통합 검증으로 위임 ? lock + visible + monitor + work-area intersect.
		CMiniClock2Dlg* main = (CMiniClock2Dlg*)GetParent();
		if (main && !main->should_skip_position_save(&m_floating))
			SaveWindowPosition(&theApp, &m_floating, _T("TimeListDlg\\m_floating"));
	}
	return 0;
}

void CTimeListDlg::on_menu_favorites(UINT nID)
{
	int index = nID - menu_favorite_start;

	CString str = theApp.GetProfileString(_T("favorite"), i2S(index, false, true, 3), _T(""));
	if (str.IsEmpty())
		return;

	std::deque<CString> token;
	get_token_str(str, token, _T("|"));

	//20260802 by claude. 메뉴를 구성하는 OnContextMenu 는 같은 검사를 하고 있으나 여기에는 없었다.
	//레지스트리 값이 손상되면 token[1] 이 범위 밖 접근이 된다.
	if (token.size() < 2)
		return;

	add(token[0], token[1], false, (token.size() == 3 ? _ttoi(token[2]) : false));
}

void CTimeListDlg::OnLvnEndLabelEditListTime(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int item = m_list.get_recent_edit_item();
	int sub_item = m_list.get_recent_edit_subitem();
	CString	text = m_list.get_text(item, sub_item);
	TRACE(_T("edit. %d, %d\n"), item, sub_item);

	CAlarmItem* data = (CAlarmItem*)m_list.GetItemData(item);

	if (sub_item == col_title)
	{
		data->title = text;
	}
	else if (sub_item == col_start)
	{
		// 절대 시각 입력 해석 (HH:MM:SS):
		//   순수 숫자 1~2자리 → 좌측 0 패딩 → HH       (예: "9"      → 09:00:00)
		//   순수 숫자 3~4자리 → 좌측 0 패딩 → HHMM     (예: "1234"   → 12:34:00)
		//   순수 숫자 5~6자리 → 좌측 0 패딩 → HHMMSS   (예: "123456" → 12:34:56)
		//   콜론 형식("12:34", "12:34:56" 등)은 그대로 get_CTime_from_datetime_str 에 전달.
		if (IsNatural(text) && text.GetLength() >= 1 && text.GetLength() <= 6)
		{
			int len = text.GetLength();
			int target = (len <= 2) ? 2 : (len <= 4) ? 4 : 6;
			while (text.GetLength() < target)
				text = _T("0") + text;

			CString hh = text.Left(2);
			CString mm = (target >= 4) ? text.Mid(2, 2) : _T("00");
			CString ss = (target >= 6) ? text.Mid(4, 2) : _T("00");
			text.Format(_T("%s:%s:%s"), (LPCTSTR)hh, (LPCTSTR)mm, (LPCTSTR)ss);
		}

		data->start = get_CTime_from_datetime_str(_T(""), text);
		CTime tEnd = data->start + data->ts_duration;
		m_list.set_text(item, col_start, get_time_str(data->start));
		m_list.set_text(item, col_end, get_time_str(tEnd));
	}
	else if (sub_item == col_duration)
	{
		// 입력 해석 규칙:
		//   "H:M:S" / "M:S" 콜론 형식    → 시:분:초
		//   순수 숫자 6자리              → HHMMSS (예: "000015" → 15초)
		//   그 외 순수 숫자              → 분 단위 (예: "15" → 15분, "115" → 115분)
		CString t = text;
		t.Trim();

		int colon_count = 0;
		for (int i = 0; i < t.GetLength(); ++i)
		{
			if (t[i] == _T(':'))
				++colon_count;
		}

		int total_secs = 0;

		if (colon_count >= 1)
		{
			int parts[3] = { 0, 0, 0 };
			int n = 0;
			int p = 0;
			while (n < 3)
			{
				int c = t.Find(_T(':'), p);
				CString seg = (c < 0) ? t.Mid(p) : t.Mid(p, c - p);
				parts[n++] = _ttoi(seg);
				if (c < 0)
					break;
				p = c + 1;
			}

			if (colon_count >= 2)
				total_secs = parts[0] * 3600 + parts[1] * 60 + parts[2];
			else
				total_secs = parts[0] * 60 + parts[1];
		}
		else if (IsNatural(t) && t.GetLength() == 6)
		{
			int hh = _ttoi(t.Left(2));
			int mm = _ttoi(t.Mid(2, 2));
			int ss = _ttoi(t.Mid(4, 2));
			total_secs = hh * 3600 + mm * 60 + ss;
		}
		else
		{
			total_secs = _ttoi(t) * 60;
		}

		data->ts_duration = CTimeSpan(0, 0, 0, total_secs);
		CTime tEnd = data->start + data->ts_duration;
		m_list.set_text(item, col_duration, get_time_str(data->ts_duration));
		m_list.set_text(item, col_end, get_time_str(tEnd));
	}
	else if (sub_item == col_end)
	{
		// 절대 시각 입력 해석 (col_start 와 동일 규칙).
		if (IsNatural(text) && text.GetLength() >= 1 && text.GetLength() <= 6)
		{
			int len = text.GetLength();
			int target = (len <= 2) ? 2 : (len <= 4) ? 4 : 6;
			while (text.GetLength() < target)
				text = _T("0") + text;

			CString hh = text.Left(2);
			CString mm = (target >= 4) ? text.Mid(2, 2) : _T("00");
			CString ss = (target >= 6) ? text.Mid(4, 2) : _T("00");
			text.Format(_T("%s:%s:%s"), (LPCTSTR)hh, (LPCTSTR)mm, (LPCTSTR)ss);
		}

		CTime tEnd = get_CTime_from_datetime_str(_T(""), text);

		// 종료 시각이 시작 시각보다 이르면 다음 날을 의도한 것으로 보고 +1일 보정.
		if (tEnd < data->start)
			tEnd += CTimeSpan(1, 0, 0, 0);

		data->ts_duration = tEnd - data->start;
		m_list.set_text(item, col_duration, get_time_str(data->ts_duration));
		m_list.set_text(item, col_end, get_time_str(tEnd));
	}

	save_timelist();

	//전 항목 col_remain 동기화 후 정렬.
	refresh_remain_and_sort();

	*pResult = 0;
}

void CTimeListDlg::set_alpha(int alpha)
{
	m_floating.set_alpha(alpha);
}
