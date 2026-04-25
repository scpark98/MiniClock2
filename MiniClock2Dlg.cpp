
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
	ON_BN_CLICKED(IDOK, &CMiniClock2Dlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMiniClock2Dlg::OnBnClickedCancel)
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
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


	m_timelistDlg.Create(IDD_TIME_LIST, this);

	m_msgbox.create(this, _T("MiniClock2"), IDR_MAINFRAME);
	m_msgbox.set_color_theme(CSCColorTheme::color_theme_dark_gray);
	m_msgbox.set_show_on_parent_center(false);

	load_setting();
	m_system_shutdown = _T("");

	bool onTop = theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
	SetWindowPos(onTop ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	RestoreWindowPosition(&theApp, this);

	m_temperature.set_text(this, _T("GPU -% -℃"), 13,
		Gdiplus::FontStyle::FontStyleBold, 1, 1.6f, _T("DSEG7 Classic"),
		Gdiplus::Color(212, 196, 166, 138),
		Gdiplus::Color(255, 0, 0, 0),
		Gdiplus::Color(255, 64, 64, 64),
		Gdiplus::Color(1, 1, 1, 1));		//완전 투명한 배경처럼 보이면서 드래그하여 이동하기도 편하다.
	RestoreWindowPosition(&theApp, &m_temperature, _T("m_temperature"), false, true, false);
	m_temperature.ShowWindow(theApp.GetProfileInt(_T("setting"), _T("nvidia info"), true) ? SW_SHOW : SW_HIDE);

	rebuild_image();

	SetTimer(timer_convert_ime, 1000, NULL);
	SetTimer(timer_time, 1000, NULL);
	SetTimer(timer_gpu_temperature, 500, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMiniClock2Dlg::load_setting()
{
	//최초 초기값 설정. 이후에 저장된 설정이 있으면 그것으로 덮어쓴다.
	_tcscpy_s(m_text_prop.name, _T("DSEG7 Classic"));
	m_text_prop.style = Gdiplus::FontStyleBold;
	m_text_prop.size = 13;
	m_text_prop.shadow_depth = 1.0f;
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

void CMiniClock2Dlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// 시스템 종료/세션 엔드 중에는 창이 숨겨지거나 phantom display 로 튕겨질 수 있다.
	// 이때 SaveWindowPosition 이 실행되면 다음 부팅에 보이지 않는 좌표가 복원된다.
	// 1) 숨김 이벤트 제외.
	if (lpwndpos && (lpwndpos->flags & SWP_HIDEWINDOW))
		return;

	// 2) 창이 실제 visible monitor 위에 있는지 검증. phantom(HDMI 오디오 등) 영역이거나
	//    어떤 모니터에도 속하지 않으면 저장하지 않는다.
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

	// 저장된 창 좌표는 다음 부팅 시 복원에 쓰이므로 실 작업 영역에 충분히 들어오는지 확인.
	CRect work(mi.rcWork);
	CRect inter;
	if (!inter.IntersectRect(rc, work) || inter.Width() < 20 || inter.Height() < 20)
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

	SaveWindowPosition(&theApp, &m_temperature, _T("m_temperature"));

	save_setting();

	CDialogEx::OnCancel();
}

void CMiniClock2Dlg::render(Gdiplus::Bitmap* img)
{
	if (!IsWindow(m_hWnd) || !img)
		return;

	CRect rc;
	GetWindowRect(rc);
	POINT ptSrc = { 0, 0 };
	POINT ptWinPos = { rc.left, rc.top };
	SIZE sz;
	// PotPlayer64.exe 실행 중이면 알파를 80%로 낮춰 영상 가림 최소화.
	int alpha_eff = m_alpha;
	if (get_process_running_count(_T("PotPlayer64.exe")) > 0)
	{
		alpha_eff = (int)(m_alpha * 0.2);
	}
	m_temperature.set_alpha(alpha_eff);
	m_timelistDlg.set_alpha(alpha_eff);

	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, (BYTE)alpha_eff, AC_SRC_ALPHA };

	if (img == NULL)
		sz = CSize(1, 1);
	else
		sz = CSize(img->GetWidth(), img->GetHeight());

	HDC hDC = ::GetDC(m_hWnd);
	HDC hdcMemory = ::CreateCompatibleDC(hDC);

	BITMAPINFOHEADER bmih = { 0 };
	int nBytesPerLine = ((sz.cx * 32 + 31) & (~31)) >> 3;
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = sz.cx;
	bmih.biHeight = sz.cy;
	bmih.biPlanes = 1;
	bmih.biBitCount = 32;
	bmih.biCompression = BI_RGB;
	bmih.biClrUsed = 0;
	bmih.biSizeImage = nBytesPerLine * sz.cy;

	PVOID pvBits = NULL;
	HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&bmih, DIB_RGB_COLORS, &pvBits, NULL, 0);
	if (!hbmpMem)
		return;

	memset(pvBits, 0, sz.cx * sz.cy * 4);
	if (hbmpMem)
	{
		HGDIOBJ hbmpOld = ::SelectObject(hdcMemory, hbmpMem);
		Gdiplus::Graphics g(hdcMemory);

		g.SetPageScale(1.0);
		g.SetPageUnit(Gdiplus::UnitPixel);
		g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

		g.DrawImage(img, 0, 0, sz.cx, sz.cy);
		//Draw customized figures
		//g.DrawRectangle(&Pen(Color(255, 255, 0, 0), 1.0f), Gdiplus::Rect(0, 0, sz.cx - 1, sz.cy - 1));

		::UpdateLayeredWindow(m_hWnd
			, hDC
			, &ptWinPos
			, &sz
			, hdcMemory
			, &ptSrc
			, 0
			, &stBlend
			, ULW_ALPHA
		);

		g.ReleaseHDC(hdcMemory);
		::SelectObject(hdcMemory, hbmpOld);
		::DeleteObject(hbmpMem);
	}

	::DeleteDC(hdcMemory);
	::ReleaseDC(m_hWnd, hDC);
}

void CMiniClock2Dlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	CMenu* pMenu = NULL;
	CString str;

	menu.LoadMenu(IDR_MENU_CONTEXT);
	pMenu = (CMenu*)menu.GetSubMenu(0);

	pMenu->CheckMenuItem(ID_MENU_ALWAYS_ON_TOP, is_top_most(m_hWnd) ? MF_CHECKED : MF_UNCHECKED);
	pMenu->CheckMenuItem(ID_MENU_NVIDIA_INFO, theApp.GetProfileInt(_T("setting"), _T("nvidia info"), true) ? MF_CHECKED : MF_UNCHECKED);

	if (m_system_shutdown.IsEmpty())
	{
		str.Format(_T("시스템 종료 시각 설정(&S)..."));
	}
	else
	{
		str.Format(_T("%s시 %s분에 자동 종료 예정. 변경..."), m_system_shutdown.Left(2), m_system_shutdown.Right(2));
		pMenu->ModifyMenu(ID_MENU_SHUTDOWN, MF_BYCOMMAND, ID_MENU_SHUTDOWN, str);
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
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

void CMiniClock2Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == timer_time)
	{
		rebuild_image();
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

	CDialogEx::OnTimer(nIDEvent);
}

void CMiniClock2Dlg::rebuild_image()
{
	CString str = get_cur_datetime_str(1, true);

	CTime t = CTime::GetCurrentTime();

	CString cur_time;
	cur_time.Format(_T("%02d%02d%02d"), t.GetHour(), t.GetMinute(), t.GetSecond());
	if (m_system_shutdown + _T("00") == cur_time)
		SystemShutdownNT(SHUTDOWN_POWEROFF);
		//m_msgbox.DoModal(_T("시스템이 자동으로 종료됩니다."), MB_OK, 3);

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

	m_img.create(r.Width(), r.Height(), Gdiplus::Color::Transparent, PixelFormat32bppARGB);
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
}

BOOL CMiniClock2Dlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (IsShiftPressed())
	{
		m_text_prop.size += (zDelta > 0 ? 1 : -1);
		rebuild_image();
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
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
	int size = dlg.GetSize();
	m_text_prop.size = get_font_size_from_pixel_size(m_hWnd, lf.lfHeight);
	m_text_prop.style = (dlg.IsBold() ? Gdiplus::FontStyleBold : 0) | (dlg.IsItalic() ? Gdiplus::FontStyleItalic : 0);

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
	CShutdownTimeDlg dlg;

	if (dlg.DoModal() == IDCANCEL)
		return;

	if (dlg.m_shutdown_time == "")
	{
		m_system_shutdown = dlg.m_shutdown_time;
		::PlaySound(MAKEINTRESOURCE(IDR_WAVE_DING_MID), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		m_msgbox.DoModal(_T("시스템 종료 예약이 해제되었습니다."), MB_OK, 3);
		return;
	}

	if (dlg.m_shutdown_time.GetLength() != 4 || dlg.m_shutdown_time < _T("0000") || dlg.m_shutdown_time > _T("2359"))
	{
		m_msgbox.DoModal(_T("올바르지 않은 시간 설정입니다.\n\nex)밤 11시 50분에 종료하려면\"2350\"을 입력하세요\n\n빈 문자열을 입력하면 자동 종료 기능이 해제됩니다."));
		OnMenuShutdown();
		return;
	}

	m_system_shutdown = dlg.m_shutdown_time;

	CString str;
	str.Format(_T("%s시 %s분에 시스템이 자동 종료됩니다."), m_system_shutdown.Left(2), m_system_shutdown.Right(2));
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
	}
	else if (m_timelistDlg.m_check_autohide.GetCheck())
	{
		m_timelistDlg.ShowWindow(bActive ? SW_SHOW : SW_HIDE);
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
