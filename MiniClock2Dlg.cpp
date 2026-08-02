
// MiniClock2Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MiniClock2.h"
#include "MiniClock2Dlg.h"
#include "afxdialogex.h"

#include "Common/CDialog/CSCColorPicker/SCColorPicker.h"

#include <mmsystem.h>
#include "AddAlarmDlg.h"
#include "ShutdownTimeDlg.h"
#include "Common/log/SCLog/SCLog.h"		//20260801 by claude. [진단] monitor off/on 시각을 alarm 시각과 대조.


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMiniClock2Dlg 대화 상자



CMiniClock2Dlg::CMiniClock2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MINICLOCK2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiniClock2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

UINT CMiniClock2Dlg::s_msg_taskbar_created =
	::RegisterWindowMessage(_T("TaskbarCreated"));

BEGIN_MESSAGE_MAP(CMiniClock2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DISPLAYCHANGE()
	ON_BN_CLICKED(IDOK, &CMiniClock2Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMiniClock2Dlg::OnBnClickedCancel)
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &CMiniClock2Dlg::OnMouseLeave)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_COLOR, &CMiniClock2Dlg::OnMenuColor)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_MENU_VIEW_TIME_LIST, &CMiniClock2Dlg::OnMenuViewTimeList)
	ON_COMMAND(ID_MENU_RESET_TIME_LIST_POS, &CMiniClock2Dlg::OnMenuResetTimeListPos)
	ON_COMMAND(ID_MENU_ALARM_AFTER_MINUTES, &CMiniClock2Dlg::OnMenuAlarmAfterMinutes)
	ON_COMMAND(ID_MENU_FONT, &CMiniClock2Dlg::OnMenuFont)
	ON_COMMAND(ID_MENU_ALWAYS_ON_TOP, &CMiniClock2Dlg::OnMenuAlwaysOnTop)
	ON_COMMAND(ID_MENU_SHUTDOWN, &CMiniClock2Dlg::OnMenuShutdown)
	ON_COMMAND(ID_MENU_RESTART_EXPLORER_TASKBARX, &CMiniClock2Dlg::OnMenuRestartExplorerTaskbarx)
	ON_COMMAND(ID_MENU_CLOSE, &CMiniClock2Dlg::OnMenuClose)
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//ON_WM_NCHITTEST()
	ON_MESSAGE(WM_SYSTRAYMSG, &CMiniClock2Dlg::on_message_CSysTrayIcon)
	ON_COMMAND(ID_MENU_NVIDIA_INFO, &CMiniClock2Dlg::OnMenuNvidiaInfo)
	ON_REGISTERED_MESSAGE(s_msg_taskbar_created, &CMiniClock2Dlg::OnTaskbarCreated)
	ON_WM_POWERBROADCAST()
	ON_WM_QUERYENDSESSION()
	ON_WM_ENDSESSION()
	ON_WM_DESTROY()
	ON_REGISTERED_MESSAGE(Message_CSCShapeDlg, &CMiniClock2Dlg::on_message_CSCShapeDlg)
	ON_REGISTERED_MESSAGE(Message_CSCMenu, &CMiniClock2Dlg::on_message_CSCMenu)
	ON_COMMAND(ID_MENU_SAVE_POS1, &CMiniClock2Dlg::OnMenuSavePos1)
	ON_COMMAND(ID_MENU_SAVE_POS2, &CMiniClock2Dlg::OnMenuSavePos2)
	ON_COMMAND(ID_MENU_SAVE_POS3, &CMiniClock2Dlg::OnMenuSavePos3)
	ON_COMMAND(ID_MENU_MOVE_TO_POS1, &CMiniClock2Dlg::OnMenuMoveToPos1)
	ON_COMMAND(ID_MENU_MOVE_TO_POS2, &CMiniClock2Dlg::OnMenuMoveToPos2)
	ON_COMMAND(ID_MENU_MOVE_TO_POS3, &CMiniClock2Dlg::OnMenuMoveToPos3)
END_MESSAGE_MAP()


// CMiniClock2Dlg 메시지 처리기

BOOL CMiniClock2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(_T("MiniClock2"));

	m_sys_tray.SetParent(m_hWnd);							//마퓖E?이벤트를 처리할 parent hwnd 설정
	HICON hIcon = ::AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sys_tray.CreateIcon(hIcon, 1, _T("MiniClock2"));	//트레이 아이콘컖E툴팁 설정
	m_sys_tray.ShowIcon(1);								//아이콘 표시

	//HideTaskBar(false);

	//작업표시줄에서 숨긴다. 단, Alt+Tab해도 나타나지 않으므로
	//trayicon을 이용해서 창을 표시하거나 숨긴다.
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW, 0);
	//출처: http://greenbblog.tistory.com/entry/wtl-dialog-hide-taskbar [그린마톩E[시햨E]]


	//1회성 마이그레이션: 과거에 모니터 off / phantom audio 모니터로 굳어버린 좌표를 일괄 reset.
	//이번 빌드부터 SaveWindowPosition 이 monitor fingerprint 를 함께 저장하므로 다음 save 시점부터
	//fingerprint 가 들어가고 이후 Restore 는 fingerprint 매칭으로 phantom 좌표를 거른다.
	//*반드시* m_timelistDlg.Create() 보다 앞에서 실행 ? TimeListDlg::OnInitDialog 가 자기/자식의
	//RestoreWindowPosition 을 즉시 호출하므로 그 전에 phantom section 들이 비워져 있어야 한다.
	int pos_ver = theApp.GetProfileInt(_T("schema"), _T("position_version"), 0);
	if (pos_ver < 2)
	{
		theApp.WriteProfileString(_T("screen"), NULL, NULL);
		theApp.WriteProfileString(_T("m_temperature\\screen"), NULL, NULL);
		theApp.WriteProfileString(_T("TimeListDlg\\screen"), NULL, NULL);
		theApp.WriteProfileString(_T("TimeListDlg\\m_floating\\screen"), NULL, NULL);
		theApp.WriteProfileInt(_T("schema"), _T("position_version"), 2);
	}

	m_timelistDlg.Create(IDD_TIME_LIST, this);

	m_msgbox.create(this, _T("MiniClock2"), IDR_MAINFRAME);
	m_msgbox.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_msgbox.set_show_on_parent_center(false);

	load_setting();
	m_system_shutdown = _T("");

	bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
	SetWindowPos(onTop ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	RestoreWindowPosition(&theApp, this);

	m_temperature.set_text(this, _T("GPU -% -℃"), 12,
		Gdiplus::FontStyle::FontStyleBold, 0.0f, 1.6f, _T("DSEG7 Classic"),
		Gdiplus::Color(212, 132, 125, 91),
		Gdiplus::Color(255, 0, 0, 0),
		Gdiplus::Color(255, 64, 64, 64),
		Gdiplus::Color(1, 0, 0, 0));		//완전 투명한 배경처럼 보이면서 드래그하여 이동하기도 편하다.
	RestoreWindowPosition(&theApp, &m_temperature, _T("m_temperature"), false, true, false);
	m_temperature.ShowWindow(theApp.GetProfileInt(_T("setting"), _T("nvidia info"), true) ? SW_SHOW : SW_HIDE);

	rebuild_image();

	SetTimer(timer_convert_ime, 1000, NULL);
	SetTimer(timer_time, 1000, NULL);
	SetTimer(timer_gpu_temperature, 500, NULL);
	SetTimer(timer_on_top, 5000, NULL);
	SetTimer(timer_audio_detect, 5000, NULL);

	//현재 모니터 개수를 정상 상태로 기록. 이후 WM_DISPLAYCHANGE 에서 줄어들면 lock.
	enum_display_monitors();
	m_monitor_count_normal = (int)g_monitors.size();

	//WM_DISPLAYCHANGE 는 OS 의 윈도우 reposition 보다 늦게 와서 race 가 있다.
	//모니터 DPMS off 는 PBT_POWERSETTINGCHANGE 로 더 먼저 잡힌다. lock 활성화 신호로 사용.
	m_hpwr_monitor = RegisterPowerSettingNotification(m_hWnd, &GUID_MONITOR_POWER_ON, DEVICE_NOTIFY_WINDOW_HANDLE);

	//context menu (CSCMenu) 1회 생성. dynamic state (check/caption) 는 popup 직전에 반영.
	m_menu_context.create(this, 240);
	m_menu_context.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_menu_context.load(IDR_MENU_CONTEXT, 0);

	//이미 load 된 m_menu_context 의 m_items 를 그대로 seed — 리소스를 CMenu 로 재로드하지 않는다.
	m_keybindings.seed_from_scmenu(&m_menu_context);
	m_hAccel = m_keybindings.build_haccel();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMiniClock2Dlg::load_setting()
{
	//최초 초기값 설정. 이후에 저장된 설정이 있으면 그것으로 덮어쓴다.
	_tcscpy_s(m_text_prop.name, _T("DSEG7 Classic"));
	m_text_prop.style = Gdiplus::FontStyleBold;
	m_text_prop.size = 13;
	m_text_prop.shadow_depth = 0.0f;
	m_text_prop.thickness = 1.6f;
	m_text_prop.cr_text = Gdiplus::Color(255, 132, 125, 91);
	m_text_prop.cr_stroke = Gdiplus::Color(255, 0, 0, 0);
	m_text_prop.cr_shadow = Gdiplus::Color(255, 64, 64, 64);
	m_text_prop.cr_back = Gdiplus::Color(1, 0, 0, 0);


	CSCTextProperty* prop;
	UINT sz = sizeof(CSCTextProperty);
	if (AfxGetApp()->GetProfileBinary(_T("setting"), _T("text property"), reinterpret_cast<LPBYTE*>(&prop), &sz))
	{
		if (sz == sizeof(CSCTextProperty))
		{
			memcpy(&m_text_prop, prop, sizeof(CSCTextProperty));
		}

		delete[] reinterpret_cast<BYTE*>(prop);
	}

	m_text_prop.shadow_depth = 0.0f;
}

void CMiniClock2Dlg::save_setting()
{
	AfxGetApp()->WriteProfileBinary(_T("setting"), _T("text property"), (LPBYTE)&m_text_prop, sizeof(CSCTextProperty));
}

void CMiniClock2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMiniClock2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMiniClock2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMiniClock2Dlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

//종료/콜백/메시지 등 모든 save 진입점에서 공통으로 사용. lock + visible + monitor + work-area intersect 통합.
//OS 가 모니터 off / 종료 sequence 중에 윈도우를 phantom 영역으로 옮긴 좌표가 굳지 않도록.
bool CMiniClock2Dlg::should_skip_position_save(CWnd* pWnd) const
{
	if (m_position_save_locked)
		return true;

	if (!pWnd || !pWnd->GetSafeHwnd() || !pWnd->IsWindowVisible() || pWnd->IsIconic())
		return true;

	CRect rc;
	pWnd->GetWindowRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0)
		return true;

	HMONITOR hMon = MonitorFromRect(&rc, MONITOR_DEFAULTTONULL);
	if (hMon == NULL)
		return true;

	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hMon, &mi))
		return true;

	CRect inter;
	if (!inter.IntersectRect(rc, CRect(mi.rcWork)) || inter.Width() < 20 || inter.Height() < 20)
		return true;

	return false;
}

void CMiniClock2Dlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	if (lpwndpos && (lpwndpos->flags & SWP_HIDEWINDOW))
		return;

	if (should_skip_position_save(this))
		return;

	SaveWindowPosition(&theApp, this);
}

void CMiniClock2Dlg::OnBnClickedOk()
{
	OnMenuAlarmAfterMinutes();
}

void CMiniClock2Dlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//OKCANCEL로 하지 않은 이유는 띵~ 소리가 난다.
	if (AfxMessageBox(_T("MiniClock2를 종료합니다"), MB_YESNO) == IDNO)
		return;

	if (!should_skip_position_save(&m_temperature))
		SaveWindowPosition(&theApp, &m_temperature, _T("m_temperature"));

	save_setting();

	CDialogEx::OnCancel();
}

bool CMiniClock2Dlg::prepare_render_buffer(HDC hDC, SIZE sz)
{
	if (sz.cx <= 0 || sz.cy <= 0)
		return false;

	if (m_hbmp_render && m_sz_render.cx == sz.cx && m_sz_render.cy == sz.cy)
		return true;

	release_render_buffer();

	m_hdc_render = ::CreateCompatibleDC(hDC);
	if (!m_hdc_render)
		return false;

	BITMAPINFOHEADER bmih = { 0 };
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = sz.cx;
	bmih.biHeight = sz.cy;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biClrUsed = 0;
	bmih.biSizeImage = (((sz.cx * 32 + 31) & (~31)) >> 3) * sz.cy;

	m_hbmp_render = ::CreateDIBSection(NULL, (PBITMAPINFO)&bmih, DIB_RGB_COLORS, &m_render_bits, NULL, 0);
	if (!m_hbmp_render)
	{
		release_render_buffer();
		return false;
	}

	m_hbmp_render_old = (HBITMAP)::SelectObject(m_hdc_render, m_hbmp_render);
	m_sz_render = CSize(sz.cx, sz.cy);

	return true;
}

void CMiniClock2Dlg::release_render_buffer()
{
	if (m_hdc_render)
	{
		if (m_hbmp_render_old)
		{
			::SelectObject(m_hdc_render, m_hbmp_render_old);
			m_hbmp_render_old = NULL;
		}

		::DeleteDC(m_hdc_render);
		m_hdc_render = NULL;
	}

	if (m_hbmp_render)
	{
		::DeleteObject(m_hbmp_render);
		m_hbmp_render = NULL;
	}

	m_render_bits = NULL;
	m_sz_render = CSize(0, 0);
}

void CMiniClock2Dlg::render(Gdiplus::Bitmap* img)
{
	if (!IsWindow(m_hWnd) || !img)
		return;

	CRect rc;
	GetWindowRect(rc);
	POINT ptSrc = { 0, 0 };
	POINT ptWinPos = { rc.left, rc.top };

	//PotPlayer64.exe / Endorphin(2).exe 가 재생 중이면 알파를 낮춰 시야 가림 최소화.
	//재생 여부 판정 자체는 timer_audio_detect 가 담당하고 여기서는 그 결과만 읽는다.
	int alpha_eff = m_alpha;
	if (m_audio_alpha_lowered && !m_mouse_hover)
		alpha_eff = (int)(m_alpha * 0.2);

	m_temperature.set_alpha(alpha_eff);
	m_timelistDlg.set_alpha(alpha_eff);

	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, (BYTE)alpha_eff, AC_SRC_ALPHA };

	SIZE sz = { (LONG)img->GetWidth(), (LONG)img->GetHeight() };

	HDC hDC = ::GetDC(m_hWnd);
	if (!hDC)
		return;

	//20260802 by claude. 이전에는 실패 시 그냥 return 해서 GetDC/CreateCompatibleDC 핸들이 샜다.
	//매초 도는 경로라 GDI 고갈이 시작되면 누수가 가속된다.
	if (!prepare_render_buffer(hDC, sz))
	{
		::ReleaseDC(m_hWnd, hDC);
		return;
	}

	memset(m_render_bits, 0, (((sz.cx * 32 + 31) & (~31)) >> 3) * sz.cy);

	{
		Gdiplus::Graphics g(m_hdc_render);

		g.SetPageScale(1.0);
		g.SetPageUnit(Gdiplus::UnitPixel);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		g.DrawImage(img, 0, 0, sz.cx, sz.cy);
	}
	//Graphics 를 먼저 소멸시켜 GDI+ 가 백버퍼에 flush 한 뒤 GDI 로 합성한다.

	::UpdateLayeredWindow(m_hWnd
		, hDC
		, &ptWinPos
		, &sz
		, m_hdc_render
		, &ptSrc
		, 0
		, &stBlend
		, ULW_ALPHA
	);

	::ReleaseDC(m_hWnd, hDC);
}

void CMiniClock2Dlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//dynamic state 반영 — check 표시 / 종료 예약 시각 caption.
	m_menu_context.check_item(ID_MENU_ALWAYS_ON_TOP, is_top_most(m_hWnd));
	m_menu_context.check_item(ID_MENU_NVIDIA_INFO, theApp.GetProfileInt(_T("setting"), _T("nvidia info"), true) ? true : false);

	if (CSCMenuItem* item = m_menu_context.get_menu_item(ID_MENU_SHUTDOWN))
	{
		if (m_system_shutdown.IsEmpty())
			item->m_caption = _T("시스템 종료 시각 설정(&S)...");
		else
			item->m_caption.Format(_T("%s시 %s분에 자동 종료 예정. 변경..."),
				m_system_shutdown.Left(2).GetString(), m_system_shutdown.Right(2).GetString());
	}

	//CSCMenu 는 modeless — 여기서 flag 만 true 로 두고, 실제 close 는 on_message_CSCMenu 의 hide 분기에서 false.
	m_context_menu_open = true;
	m_menu_context.popup_menu(point.x, point.y);
}

BOOL CMiniClock2Dlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnEraseBkgnd(pDC);
}

void CMiniClock2Dlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (IsShiftPressed())
		DefWindowProc(WM_NCLBUTTONDOWN, HTCAPTION, MAKEWORD(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CMiniClock2Dlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CMiniClock2Dlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_context_menu_open)
	{
		CDialogEx::OnMouseMove(nFlags, point);
		return;
	}

	if (!m_mouse_hover)
	{
		m_mouse_hover = true;
		rebuild_image();

		//앱이 이미 활성 상태면 OnActivateApp 이 timelist show/hide 를 담당 — hover 는 개입하지 않는다.
		//비활성 상태에서만 hover 로 임시 표시. 포커스는 뺏지 않도록 SW_SHOWNA.
		if (m_timelistDlg.m_hWnd
			&& m_timelistDlg.m_check_autohide.GetCheck()
			&& !m_timelistDlg.IsWindowVisible()
			&& GetForegroundWindow() != this
			&& GetForegroundWindow() != &m_timelistDlg)
		{
			m_timelistDlg.ShowWindow(SW_SHOWNA);
			m_timelist_shown_by_hover = true;
			//polling 시작 — main→timelist→외부 경로 커버.
			SetTimer(timer_hover_poll, 200, NULL);
		}
	}

	TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
	TrackMouseEvent(&tme);

	CDialogEx::OnMouseMove(nFlags, point);
}

LRESULT CMiniClock2Dlg::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_context_menu_open)
		return 0;

	if (m_mouse_hover)
	{
		m_mouse_hover = false;
		rebuild_image();
	}

	//hover 로 띄운 timelist — 커서가 timelist 로 이동한 게 아니라 완전히 외부로 나갔다면 즉시 hide.
	//timelist 안으로 이동한 경우엔 flag/타이머를 유지 → timer_hover_poll 이 이후 timelist 벗어남을 감지해 hide.
	if (m_timelist_shown_by_hover && m_timelistDlg.m_hWnd)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CRect rc_tl;
		m_timelistDlg.GetWindowRect(&rc_tl);

		if (!rc_tl.PtInRect(pt))
		{
			m_timelistDlg.ShowWindow(SW_HIDE);
			m_timelist_shown_by_hover = false;
			KillTimer(timer_hover_poll);
		}
	}
	return 0;
}

void CMiniClock2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_time)
	{
		rebuild_image();
	}
	else if (nIDEvent == timer_hover_poll)
	{
		//커서가 main dlg 와 timelist 어느 쪽에도 없으면 hide. 메뉴 표시 중이면 skip
		//(§m_context_menu_open — 메뉴 팝업 위 커서로 잘못 hide 되는 것 방지).
		if (m_context_menu_open)
			return;

		if (!m_timelist_shown_by_hover || !m_timelistDlg.m_hWnd)
		{
			KillTimer(timer_hover_poll);
			m_timelist_shown_by_hover = false;
			return;
		}

		if (!m_timelistDlg.IsWindowVisible())
		{
			//다른 경로(활성화/메뉴 토글 등)로 이미 hide 됨 — 타이머만 정리.
			m_timelist_shown_by_hover = false;
			KillTimer(timer_hover_poll);
			return;
		}

		CPoint pt;
		GetCursorPos(&pt);

		CRect rc_main;
		GetWindowRect(&rc_main);

		CRect rc_tl;
		m_timelistDlg.GetWindowRect(&rc_tl);

		if (!rc_main.PtInRect(pt) && !rc_tl.PtInRect(pt))
		{
			m_timelistDlg.ShowWindow(SW_HIDE);
			m_timelist_shown_by_hover = false;
			KillTimer(timer_hover_poll);
		}
	}
	else if (nIDEvent == timer_on_top)
	{
		bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
		//컨텍스트 메뉴 표시 중이면 z-order 재assert 를 skip — main dlg 와 owned popup (timelist) 가
		//non-topmost 팝업 메뉴 위로 올라와 메뉴를 가리는 문제 방지.
		if (onTop && !m_context_menu_open)
		{
			//SWP_NOACTIVATE ? 반복 재assert 가 사용자 포커스를 뺏지 않도록.
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		//부팅 후 ~60초 (5초 × 12회) 동안만 재assert. 그 이후엔 OnActivateApp(FALSE) 에서 trigger.
		if (++m_topmost_reassert_count >= 12)
			KillTimer(timer_on_top);
	}
	else if (nIDEvent == timer_gpu_temperature)
	{
		KillTimer(timer_gpu_temperature);
		CString str;
		str.Format(_T("%d%% %d℃"), m_gpu_usage.get_usage(), m_nvidia.get_temperature(0));
		CSCShapeDlgTextSetting* setting = m_temperature.get_text_setting();
		setting->text = str;
		m_temperature.set_text(setting);

		SetTimer(timer_gpu_temperature, 5000, NULL);
		Invalidate(false);
	}
	else if (nIDEvent == timer_convert_ime)
	{
		KillTimer(timer_convert_ime);
		ime_convert(m_hWnd, true);
		m_first_run = false;
	}
	else if (nIDEvent == timer_audio_detect)
	{
		//20260802 by claude. 프로세스 스냅샷 + WASAPI 세션 열거를 매초 3회 돌리던 것을 5초 1회로.
		//이름 목록을 한 번에 넘겨 스냅샷·세션 열거가 이름 개수와 무관하게 1회만 수행된다.
		//사운드가 멈춰도 즉시 복원하지 않고 timer_audio_alpha_restore 로 n초 대기 후 복원 —
		//트랙 이동·짧은 무음 구간에서 알파가 펄럭이는 산만함 방지.
		bool audio_active = is_process_audio_active(
			{ _T("PotPlayer64.exe"), _T("Endorphin.exe"), _T("Endorphin2.exe") });

		if (audio_active)
		{
			if (m_audio_alpha_restore_pending)
			{
				KillTimer(timer_audio_alpha_restore);
				m_audio_alpha_restore_pending = false;
			}

			if (!m_audio_alpha_lowered)
			{
				m_audio_alpha_lowered = true;
				rebuild_image();
			}
		}
		else if (m_audio_alpha_lowered && !m_audio_alpha_restore_pending)
		{
			SetTimer(timer_audio_alpha_restore, 2000, NULL);
			m_audio_alpha_restore_pending = true;
		}
	}
	else if (nIDEvent == timer_audio_alpha_restore)
	{
		KillTimer(timer_audio_alpha_restore);
		m_audio_alpha_restore_pending = false;
		m_audio_alpha_lowered = false;
		rebuild_image();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMiniClock2Dlg::rebuild_image()
{
	CString str = get_cur_datetime_str(1, true);

	CTime t = CTime::GetCurrentTime();

	//20260802 by claude. 예약 시각을 지났으면 발동. 발동 직후 예약을 비워 종료 시퀀스가 진행되는
	//동안 매초 SystemShutdownNT 가 재호출되는 것을 막는다.
	if (!m_system_shutdown.IsEmpty() && t >= m_system_shutdown_time)
	{
		logWrite(_T("시스템 종료 실행. 예약=%s 현재=%s"),
			get_datetime_str(m_system_shutdown_time).GetString(),
			get_datetime_str(t).GetString());

		m_system_shutdown.Empty();
		SystemShutdownNT(SHUTDOWN_POWEROFF);
	}

	m_para.clear();
	CSCParagraph::build_paragraph_str(str, m_para, &m_text_prop);

	CClientDC dc(this);
	CRect r(0, 0, 100, 100);

	r = CSCParagraph::calc_text_rect(r, &dc, m_para, DT_CENTER | DT_VCENTER);
	r.InflateRect(1, 1);

	//배경을 그린다면 r은 더 크게 잡아줘야 한다. margin만큼 더 크게 키워준다.
	if (m_text_prop.cr_back.GetValue() != Gdiplus::Color::Transparent)
	{
		r.InflateRect((int)m_text_prop.round_thickness + 2, (int)m_text_prop.round_thickness + 2);
	}

	//20260802 by claude. 시:분:초만 바뀌고 크기는 대개 그대로다. 같은 크기면 비트맵을 재사용한다.
	//어차피 아래에서 g.Clear() 로 전면을 지우므로 create 의 초기 색은 의미가 없다.
	if (!m_img.m_pBitmap ||
		(int)m_img.m_pBitmap->GetWidth() != r.Width() ||
		(int)m_img.m_pBitmap->GetHeight() != r.Height())
	{
		m_img.create(r.Width(), r.Height());
	}

	r = CRect(0, 0, r.Width(), r.Height());
	r = CSCParagraph::calc_text_rect(r, &dc, m_para, DT_CENTER | DT_VCENTER);

	//해당 캔버스에
	Gdiplus::Graphics g(m_img.m_pBitmap);
	g.Clear(m_text_prop.cr_back);

	g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	CSCParagraph::draw_text(g, m_para);

	render(m_img);
}

void CMiniClock2Dlg::OnMenuColor()
{
	CSCColorPicker picker;
	if (picker.DoModal(this, m_text_prop.cr_text) == IDCANCEL)
		return;

	m_text_prop.cr_text = picker.get_selected_color();
	rebuild_image();
	save_setting();
}

BOOL CMiniClock2Dlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (IsShiftPressed())
	{
		m_text_prop.size += (zDelta > 0 ? 1 : -1);
		rebuild_image();
		save_setting();
	}

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CMiniClock2Dlg::OnMenuViewTimeList()
{
	if (m_timelistDlg.IsWindowVisible())
		m_timelistDlg.ShowWindow(SW_HIDE);
	else
		m_timelistDlg.ShowWindow(SW_SHOW);
}

void CMiniClock2Dlg::OnMenuResetTimeListPos()
{
	//사용자가 명시적으로 호출 ? TimeListDlg 를 primary monitor 의 작업영역 중앙으로 이동시키고
	//그 좌표를 즉시 레지스트리에 저장. lock 상태와 무관하게 강제 저장.
	CRect rc_work;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc_work, 0);

	CRect rc_dlg;
	m_timelistDlg.GetWindowRect(&rc_dlg);

	int cx = rc_work.CenterPoint().x - rc_dlg.Width() / 2;
	int cy = rc_work.CenterPoint().y - rc_dlg.Height() / 2;

	bool was_locked = m_position_save_locked;
	m_position_save_locked = false;

	m_timelistDlg.SetWindowPos(NULL, cx, cy, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	if (!m_timelistDlg.IsWindowVisible())
		m_timelistDlg.ShowWindow(SW_SHOW);

	m_position_save_locked = was_locked;
}

void CMiniClock2Dlg::OnMenuAlarmAfterMinutes()
{
	CAddAlarmDlg dlg;
	if (dlg.DoModal() == IDCANCEL)
		return;

	m_timelistDlg.add(dlg.m_name, dlg.m_duration, dlg.m_add_favorite, dlg.m_as_floating);
}

void CMiniClock2Dlg::OnMenuFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	_tcscpy_s(lf.lfFaceName, m_text_prop.name);
	lf.lfWeight = (m_text_prop.style == Gdiplus::FontStyleBold ? FW_BOLD : FW_NORMAL);
	lf.lfHeight = get_pixel_size_from_font_size(m_hWnd, m_text_prop.size);

	CFontDialog dlg(&lf, CF_SCREENFONTS | CF_WYSIWYG | CF_INITTOLOGFONTSTRUCT);
	dlg.m_cf.rgbColors = m_text_prop.cr_text.ToCOLORREF();

	if (dlg.DoModal() == IDCANCEL)
		return;

	dlg.GetCurrentFont(&lf);
	_tcscpy_s(m_text_prop.name, lf.lfFaceName);
	m_text_prop.size = get_font_size_from_pixel_size(m_hWnd, lf.lfHeight);
	m_text_prop.style = (dlg.IsBold() ? Gdiplus::FontStyleBold : 0) | (dlg.IsItalic() ? Gdiplus::FontStyleItalic : 0);

	rebuild_image();
	save_setting();
}

void CMiniClock2Dlg::OnMenuAlwaysOnTop()
{
	bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
	onTop = !onTop;
	theApp.WriteProfileInt(_T("setting"), _T("always on top"), onTop);

	if (onTop)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CMiniClock2Dlg::OnMenuShutdown()
{
	CString shutdown_time;

	//20260802 by claude. 이전에는 잘못된 입력마다 OnMenuShutdown() 을 자기 재귀 호출해서
	//입력을 틀릴 때마다 모달이 스택에 쌓였다.
	while (true)
	{
		CShutdownTimeDlg dlg;
		if (dlg.DoModal() == IDCANCEL)
			return;

		shutdown_time = dlg.m_shutdown_time;

		if (shutdown_time.IsEmpty())
		{
			logWrite(_T("공백 입력으로 시스템 종료 예약 해제됨."));
			m_system_shutdown.Empty();
			::PlaySound(MAKEINTRESOURCE(IDR_WAVE_DING_MID), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
			m_msgbox.DoModal(_T("시스템 종료 예약이 해제되었습니다."), MB_OK, 3);
			return;
		}

		if (shutdown_time.GetLength() == 4 && shutdown_time >= _T("0000") && shutdown_time <= _T("2359"))
			break;

		m_msgbox.DoModal(_T("올바르지 않은 시간 설정입니다.\n\nex)밤 11시 50분에 종료하려면\"2350\"을 입력하세요\n\n빈 문자열을 입력하면 자동 종료 기능이 해제됩니다."));
	}

	m_system_shutdown = shutdown_time;

	//20260802 by claude. hhmm 을 절대 시각으로 환산. 이미 지난 시각이면 다음 날 그 시각을 의도한 것으로 본다.
	CTime now = CTime::GetCurrentTime();
	m_system_shutdown_time = CTime(now.GetYear(), now.GetMonth(), now.GetDay(),
		_ttoi(m_system_shutdown.Left(2)), _ttoi(m_system_shutdown.Right(2)), 0);
	if (m_system_shutdown_time <= now)
		m_system_shutdown_time += CTimeSpan(1, 0, 0, 0);

	CString str;
	str.Format(_T("%s시 %s분에 시스템이 자동 종료됩니다."), m_system_shutdown.Left(2), m_system_shutdown.Right(2));
	logWrite(_T("%s"), str);
	::PlaySound(MAKEINTRESOURCE(IDR_WAVE_DING_MID), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
	m_msgbox.DoModal(str, MB_OK, 3);
}

void CMiniClock2Dlg::OnMenuRestartExplorerTaskbarx()
{
	run_process(_T("taskkill /f /im explorer.exe"), true);
	Wait(500);
	run_process(_T("C:\\Windows\\explorer.exe"), false);
}

void CMiniClock2Dlg::OnMenuClose()
{
	OnBnClickedCancel();
}

void CMiniClock2Dlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	//다른 앱이 포커스를 가져갈 때마다 topmost 재assert. timer 가 끝난 뒤에도 늦게 올라오는
	//topmost 앱들 (대화방, 동영상 플레이어 등) 위로 다시 우리 창을 올린다.
	if (!bActive && theApp.GetProfileInt(_T("setting"), _T("always on top"), true))
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	if (!m_timelistDlg.m_hWnd)
		return;

	if (m_first_run && (m_timelistDlg.m_list.size() == 0))
		return;

	//bActive 상태가 되면 타임리스트 창은 반드시 보여야 하고
	//InActive 상태가 될 경우는 타임리스트가 자동 숨김 옵션에 따라 유지하거나 감춰준다.
	//단, 맨 처음 실행시에도 이 코드가 수행되는데
	//등록된 알람 항목이 있다면 실행 시 리스트를 보여주면서 실행하면 되지만
	//항목이 없을 경우는 굳이 리스트를 보여줄 필요없이 실행하는게 좋다.
	//우선 프로그램 실행 시에는 적용 안되도록 플래그를 체크한다.

	if (bActive)
	{
		m_timelistDlg.ShowWindow(SW_SHOW);
		//사용자가 클릭·활성화로 진입 — 이후 leave 에서 hover-hide 가 발동하지 않도록 플래그·타이머 정리.
		m_timelist_shown_by_hover = false;
		KillTimer(timer_hover_poll);
	}
	else if (m_timelistDlg.m_check_autohide.GetCheck())
	{
		m_timelistDlg.ShowWindow(bActive ? SW_SHOW : SW_HIDE);
		m_timelist_shown_by_hover = false;
		KillTimer(timer_hover_poll);
	}
}

void CMiniClock2Dlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	OnMenuAlarmAfterMinutes();

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

void CMiniClock2Dlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CMiniClock2Dlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialogEx::OnKillFocus(pNewWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

BOOL CMiniClock2Dlg::PreTranslateMessage(MSG* pMsg)
{
	//메뉴 캡션의 '\tKey' 로 등록된 단축키를 accelerator table 로 발화 → 동일 WM_COMMAND 경로.
	if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CMiniClock2Dlg::OnNcHitTest(CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnNcHitTest(point);
}

LRESULT CMiniClock2Dlg::on_message_CSysTrayIcon(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		if (IsIconic())
		{
			m_temperature.ShowWindow(SW_SHOW);
			ShowWindow(SW_SHOW);
			ShowWindow(SW_RESTORE);
			SetForegroundWindow();
			SetActiveWindow();
		}
		else
		{
			m_temperature.ShowWindow(SW_HIDE);
			ShowWindow(SW_MINIMIZE);
			ShowWindow(SW_HIDE);
		}
		break;
		case WM_RBUTTONUP:
		{
			CMenu	menu;
			menu.LoadMenu(IDR_MENU_CONTEXT);

			CMenu* pMenu = menu.GetSubMenu(0);

			bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
			pMenu->CheckMenuItem(ID_MENU_ALWAYS_ON_TOP, onTop ? MF_CHECKED : MF_UNCHECKED);

			CPoint pt;
			GetCursorPos(&pt);

			CWnd* pMenuEventHandler = ::AfxGetMainWnd();
			//아래 코드를 추가해야 트레이 아이콘 팝업메뉴에서 단축키가 동작한다.
			pMenuEventHandler->SetForegroundWindow();
			pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, pMenuEventHandler);
			break;
		}
	}

	return 1;
}

void CMiniClock2Dlg::OnMenuNvidiaInfo()
{
	bool show_nvidia_info = theApp.GetProfileInt(_T("setting"), _T("nvidia info"), true);
	show_nvidia_info = !show_nvidia_info;
	theApp.WriteProfileInt(_T("setting"), _T("nvidia info"), show_nvidia_info);

	m_temperature.ShowWindow(show_nvidia_info ? SW_SHOW : SW_HIDE);
}

// Shell_TrayWnd 가 (재)생성될 때 전체 top-level 창에 브로드캐스트되는 메시지.
// 시작프로그램으로 부팅 직후 실행된 경우 셸이 늦게 올라오는 시점에 수신되어
// OnInitDialog 에서 일부 실패했던 초기화를 여기서 복구한다.
// 이미 부팅된 상태에서 수동 실행하면 이 메시지는 오지 않으므로 부작용 없음.
LRESULT CMiniClock2Dlg::OnTaskbarCreated(WPARAM, LPARAM)
{
	// 내부 리스트에 이미 ID 1 이 등록돼 있으면 CreateIcon 이 ASSERT 로 걸린다.
	// 먼저 DeleteIcon 으로 리스트와 Shell_NotifyIcon 모두 정리한 뒤 재등록.
	m_sys_tray.DeleteIcon(1);

	HICON hIcon = ::AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sys_tray.CreateIcon(hIcon, 1, _T("MiniClock2"));
	m_sys_tray.ShowIcon(1);

	bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
	SetWindowPos(onTop ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	RestoreWindowPosition(&theApp, this);
	RestoreWindowPosition(&theApp, &m_temperature, _T("m_temperature"), false, true, false);

	rebuild_image();
	Invalidate(false);
	return 0;
}

//모니터 on/off, 해상도 변경, 디스플레이 추가/제거 시 호출.
//OS 의 윈도우 reposition 보다 늦게 오는 경우가 있어 단독으로는 race 가 있다. 그래도 fallback 으로 유지.
void CMiniClock2Dlg::OnDisplayChange(UINT /*uBitsPerPixel*/, int /*cxScreen*/, int /*cyScreen*/)
{
	enum_display_monitors();
	int cur = (int)g_monitors.size();

	if (cur < m_monitor_count_normal)
	{
		m_position_save_locked = true;
	}
	else
	{
		m_position_save_locked = false;
		m_monitor_count_normal = cur;
	}
}

//모니터 DPMS off / 시스템 standby 신호. WM_DISPLAYCHANGE 보다 먼저 와서 OS 의 phantom reposition
//전에 lock 활성화 가능. GUID_MONITOR_POWER_ON.Data[0] == 0 이면 monitor off.
UINT CMiniClock2Dlg::OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData)
{
	if (nPowerEvent == PBT_POWERSETTINGCHANGE && lEventData != 0)
	{
		POWERBROADCAST_SETTING* p = (POWERBROADCAST_SETTING*)lEventData;
		if (p->PowerSetting == GUID_MONITOR_POWER_ON && p->DataLength >= sizeof(DWORD))
		{
			DWORD on = *(DWORD*)p->Data;

			//20260801 by claude. [진단] monitor DPMS 상태 변경 시각 — alarm 시각과 대조용.
			logWrite(_T("[power] MONITOR_POWER_ON=%u"), on);

			if (on == 0)
				m_position_save_locked = true;
			//on 일 때는 lock 해제하지 않는다 ? DPMS on 직후 OS 가 곧바로 정상 desktop 으로
			//복귀시키는 위치 이벤트들이 lock 해제와 동시에 새 좌표를 굳힐 수 있다.
			//WM_DISPLAYCHANGE 가 모니터 개수 회복을 확인하면 거기서 해제.
		}
	}
	else if (nPowerEvent == PBT_APMSUSPEND)
	{
		//20260801 by claude. [진단] 시스템 suspend (모니터 off 와 별개).
		logWrite(_T("[power] APM_SUSPEND"));
		m_position_save_locked = true;
	}
	else if (nPowerEvent == PBT_APMRESUMEAUTOMATIC || nPowerEvent == PBT_APMRESUMESUSPEND)
	{
		//20260801 by claude. [진단] 시스템 resume — 이 시점 직후 alarm 이 몰려 울리는지 확인.
		logWrite(_T("[power] APM_RESUME event=0x%X"), nPowerEvent);
	}

	return CDialogEx::OnPowerBroadcast(nPowerEvent, lEventData);
}

//시스템 종료 시작 신호. shutdown sequence 가 OS 의 reposition 을 동반할 수 있어 lock.
BOOL CMiniClock2Dlg::OnQueryEndSession()
{
	m_position_save_locked = true;
	logWrite(_T("[power] QueryEndSession"));		//20260801 by claude. [진단] 종료 시퀀스 진입 시각.
	return CDialogEx::OnQueryEndSession();
}

void CMiniClock2Dlg::OnEndSession(BOOL bEnding)
{
	logWrite(_T("[power] EndSession bEnding=%d"), bEnding);		//20260801 by claude. [진단] 종료 확정/취소 시각.
	if (bEnding)
		m_position_save_locked = true;
	CDialogEx::OnEndSession(bEnding);
}

void CMiniClock2Dlg::OnDestroy()
{
	if (m_hpwr_monitor)
	{
		UnregisterPowerSettingNotification(m_hpwr_monitor);
		m_hpwr_monitor = NULL;
	}
	if (m_hAccel)
	{
		::DestroyAcceleratorTable(m_hAccel);
		m_hAccel = NULL;
	}

	release_render_buffer();

	CDialogEx::OnDestroy();
}

//CSCShapeDlg 가 OnWindowPosChanged 마다 부모로 보내는 알림. m_temperature 의 위치를 실시간 저장.
//(이전에는 OnBnClickedCancel 종료 시점에만 저장돼서 사용자가 옮긴 위치가 즉시 굳지 않았고,
// 종료 sequence 의 phantom reposition 좌표가 그대로 저장되는 위험도 있었다.)
LRESULT CMiniClock2Dlg::on_message_CSCMenu(WPARAM wParam, LPARAM /*lParam*/)
{
	CSCMenuMessage* msg = (CSCMenuMessage*)wParam;
	if (!msg)
		return 0;

	if (msg->m_message == CSCMenu::message_scmenu_hide)
	{
		//메뉴 닫힘 — hover 로직 재개.
		m_context_menu_open = false;

		//메뉴 표시 중 swallow 한 leave 를 뒤늦게 반영 — 커서가 이 창 밖이면 hover 해제, 안이면 TrackMouseEvent 재무장.
		CPoint pt;
		GetCursorPos(&pt);

		CRect rc;
		GetWindowRect(&rc);

		if (!rc.PtInRect(pt))
			OnMouseLeave(0, 0);
		else
		{
			TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
			TrackMouseEvent(&tme);
		}
		return 0;
	}

	if (msg->m_message != CSCMenu::message_scmenu_selchanged || !msg->m_menu_item)
		return 0;

	//메뉴 항목 선택 → 기존 ON_COMMAND 핸들러로 dispatch (accelerator 경로와 동일).
	SendMessage(WM_COMMAND, MAKEWPARAM(msg->m_menu_item->m_id, 0), 0);
	return 0;
}

LRESULT CMiniClock2Dlg::on_message_CSCShapeDlg(WPARAM wParam, LPARAM /*lParam*/)
{
	CSCShapeDlgMessage* msg = (CSCShapeDlgMessage*)wParam;
	if (msg && msg->message == CSCShapeDlg::message_window_pos_changed && msg->pThis == &m_temperature)
	{
		if (!should_skip_position_save(&m_temperature))
			SaveWindowPosition(&theApp, &m_temperature, _T("m_temperature"));
	}
	return 0;
}

void CMiniClock2Dlg::save_positions_to_slot(int slot)
{
	CString prefix;
	prefix.Format(_T("pos%d"), slot);

	SaveWindowPosition(&theApp, this,                       prefix);
	SaveWindowPosition(&theApp, &m_temperature,             prefix + _T("\\m_temperature"));
	SaveWindowPosition(&theApp, &m_timelistDlg,             prefix + _T("\\TimeListDlg"));
	SaveWindowPosition(&theApp, &m_timelistDlg.m_floating,  prefix + _T("\\TimeListDlg\\m_floating"));
}

void CMiniClock2Dlg::move_positions_from_slot(int slot)
{
	CString prefix;
	prefix.Format(_T("pos%d"), slot);

	struct { CWnd* wnd; CString section; } items[] =
	{
		{ this,                       prefix                                    },
		{ &m_temperature,             prefix + _T("\\m_temperature")            },
		{ &m_timelistDlg,             prefix + _T("\\TimeListDlg")              },
		{ &m_timelistDlg.m_floating,  prefix + _T("\\TimeListDlg\\m_floating")  },
	};

	for (auto& it : items)
	{
		//slot 이 비어 있으면 (한 번도 저장 안 됨) 건너뜀 — RestoreWindowPosition 이 기본으로
		//호출하는 CenterWindow() 로 창이 중앙으로 튀는 것을 방지.
		CRect saved = get_profile_value<CRect>(it.section + _T("\\screen"), _T("position"), CRect());
		if (saved.IsRectNull())
			continue;

		RestoreWindowPosition(&theApp, it.wnd, it.section, false, true, false);
	}
}

void CMiniClock2Dlg::OnMenuMoveToPos1()
{
	move_positions_from_slot(1);
}

void CMiniClock2Dlg::OnMenuMoveToPos2()
{
	move_positions_from_slot(2);
}

void CMiniClock2Dlg::OnMenuMoveToPos3()
{
	move_positions_from_slot(3);
}

void CMiniClock2Dlg::OnMenuSavePos1()
{
	save_positions_to_slot(1);
}

void CMiniClock2Dlg::OnMenuSavePos2()
{
	save_positions_to_slot(2);
}

void CMiniClock2Dlg::OnMenuSavePos3()
{
	save_positions_to_slot(3);
}
