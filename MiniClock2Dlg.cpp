
// MiniClock2Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MiniClock2.h"
#include "MiniClock2Dlg.h"
#include "afxdialogex.h"

#include "Common/CDialog/CSCColorPicker/SCColorPicker.h"

#include "AddAlarmDlg.h"


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

	m_timelistDlg.Create(IDD_TIME_LIST, this);
	m_timelistDlg.ShowWindow(SW_SHOW);

	load_setting();

	bool onTop = true;// theApp.GetProfileInt(_T("setting"), _T("always on top"), true);
	SetWindowPos(onTop ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	RestoreWindowPosition(&theApp, this);

	SetTimer(timer_convert_ime, 1000, NULL);
	SetTimer(timer_time, 1000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMiniClock2Dlg::load_setting()
{
	_tcscpy_s(m_text_prop.name, _T("DSEG7 Classic"));
	m_text_prop.style = Gdiplus::FontStyleBold;
	m_text_prop.size = 14;
	m_text_prop.thickness = 2.0f;
	m_text_prop.cr_text = Gdiplus::Color(255, 132, 125, 91);
	m_text_prop.cr_stroke = Gdiplus::Color(255, 24, 24, 24);
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}

void CMiniClock2Dlg::OnBnClickedOk()
{
	OnMenuAlarmAfterMinutes();
}

void CMiniClock2Dlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	BLENDFUNCTION stBlend = { AC_SRC_OVER, 0, m_alpha, AC_SRC_ALPHA };

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

	menu.LoadMenu(IDR_MENU_CONTEXT);
	pMenu = (CMenu*)menu.GetSubMenu(0);

	pMenu->CheckMenuItem(ID_MENU_ALWAYS_ON_TOP, is_top_most(m_hWnd) ? MF_CHECKED : MF_UNCHECKED);

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

	//m_img.save(_T("D:\\MiniClock2.png"));

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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
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
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMiniClock2Dlg::OnMenuRestartExplorerTaskbarx()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMiniClock2Dlg::OnMenuClose()
{
	OnBnClickedCancel();
}

void CMiniClock2Dlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CDialogEx::OnActivateApp(bActive, dwThreadID);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_first_run && (m_timelistDlg.m_list.size() == 0))
		return;
}

void CMiniClock2Dlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	OnMenuAlarmAfterMinutes();

	CDialogEx::OnLButtonDblClk(nFlags, point);
}
