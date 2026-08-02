
// MiniClock2Dlg.h: 헤더 파일
//

#pragma once

#include "Common/SCGdiplusBitmap.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"
#include "Common/system/SysTrayIcon/SysTrayIcon.h"
#include "Common/device/nvidia/nvidia_info.h"
#include "Common/device/gpu_usage.h"
#include "Common/CMenu/CSCMenuBar/SCMenu.h"
#include "Common/system/SCKeyBindings/SCKeyBindings.h"

#include "TimeListDlg.h"

// CMiniClock2Dlg 대화 상자
class CMiniClock2Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMiniClock2Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CSCGdiplusBitmap	m_img;
	//CSCShapeDlg		m_main;

	CTimeListDlg		m_timelistDlg;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINICLOCK2_DIALOG };
#endif

protected:
	enum
	{
		timer_convert_ime = 0,	//주로 한글 입력을 많이하므로 앱이 시작되면 한글로 자동 변경해준다.
		timer_time,
		timer_gpu_temperature,
		timer_on_top,
		timer_audio_alpha_restore,
		//20260802 by claude. 오디오 재생 감지는 프로세스 스냅샷 + WASAPI 세션 열거라 비싸다.
		//매초 도는 render() 에서 빼내어 이 주기로만 검사한다.
		timer_audio_detect,
		//hover 로 timelist 를 띄운 동안만 동작 — WM_MOUSELEAVE 는 창당 1회만 발화하므로
		//main→timelist→외부 경로에서 main 이 두 번째 leave 를 못 받음. 폴링으로 커서가 두 창 어느 쪽에도
		//없으면 timelist hide.
		timer_hover_poll,
	};

	CSysTrayIcon		m_sys_tray;
	LRESULT				on_message_CSysTrayIcon(WPARAM, LPARAM);

	//우클릭 컨텍스트 메뉴 (CSCMenu — color theme / full-custom paint). 표준 TrackPopupMenu 대신 사용.
	CSCMenu				m_menu_context;
	//메뉴 캡션의 '\t' 뒤 단축키 표기(1/2/3/F1~F3/Ctrl+T/F5 등)를 파싱해 HACCEL 로 등록.
	//표준 accelerator 경로 → PreTranslateMessage 의 TranslateAccelerator 가 WM_COMMAND 발화.
	CSCKeyBindings		m_keybindings;
	HACCEL				m_hAccel = NULL;
	LRESULT				on_message_CSCMenu(WPARAM, LPARAM);

	CNVidiaInfo			m_nvidia;
	CGpuUsage			m_gpu_usage;
	CSCShapeDlg			m_temperature;

	std::deque<std::deque<CSCParagraph>> m_para;
	CSCTextProperty		m_text_prop;

	CSCMessageBox		m_msgbox;

	bool				m_first_run = true;

	//부팅 시 자기를 topmost 로 올리는 다른 startup 앱들이 우리보다 늦게 올라오면 그 시점에 그 앱이
	//topmost Z-order 의 위로 가서 우리 앱이 가려진다. 부팅 후 일정 시간 동안 timer_on_top 으로
	//반복 재assert 해서 늦게 올라오는 topmost 들 위로 다시 가져온다.
	int					m_topmost_reassert_count = 0;

	int					m_alpha = 255;
	//미디어 플레이어 사운드 출력 중에는 알파를 낮춰 시야를 가리지 않는다.
	//사운드가 멈춰도 즉시 복원하지 말고 n초 대기 — 트랙 이동·짧은 무음 구간에서
	//알파가 펄럭이는 산만함 방지.
	bool				m_audio_alpha_lowered = false;
	bool				m_audio_alpha_restore_pending = false;
	//마우스가 윈도우 위에 있는 동안엔 audio_lowered 와 무관하게 원래 알파로 표시.
	bool				m_mouse_hover = false;
	//autohide 켜진 상태에서 hover 로 timelist 를 띄웠는지. 사용자 클릭·활성화로 뜬 경우와 구분해
	//leave 시 hover-show 한 것만 다시 감춘다.
	bool				m_timelist_shown_by_hover = false;
	//우클릭 컨텍스트 메뉴 표시 중 — 메뉴 위로 마우스가 이동하면 main dlg 가 leave 를 받아
	//hover 로직이 timelist 를 감추거나 알파를 낮추는 부작용을 유발. TrackPopupMenu 사이에는 hover 처리 skip.
	bool				m_context_menu_open = false;
	void				render(Gdiplus::Bitmap* img);
	void				rebuild_image();

	//20260802 by claude. 레이어드 윈도우 합성용 백버퍼. 매 프레임 새로 만들면 초당 GDI 객체
	//생성·파괴가 반복되고 실패 경로마다 해제를 빠뜨리기 쉽다. 크기가 바뀔 때만 다시 만든다.
	HDC					m_hdc_render = NULL;
	HBITMAP				m_hbmp_render = NULL;
	HBITMAP				m_hbmp_render_old = NULL;
	void*				m_render_bits = NULL;
	CSize				m_sz_render = CSize(0, 0);
	//요청 크기의 백버퍼를 확보한다. 이미 같은 크기면 아무것도 하지 않고 true.
	bool				prepare_render_buffer(HDC hDC, SIZE sz);
	void				release_render_buffer();

	void				load_setting();
	void				save_setting();

	bool				m_is_dragging = false;
	POINT				m_drag_start = {};

	CString				m_system_shutdown;	//종료 시각(ex. "2350")
	//20260802 by claude. hhmm 문자열 == 비교는 그 1초에 tick 이 들어오지 못하면(절전 복귀·시각 점프·
	//1초 이상 메시지 펌프 정지) 예약이 영영 발동하지 않는다. 설정 시점에 절대 시각으로 환산해 두고
	//now >= target 으로 판정한다. m_system_shutdown 이 비어 있으면 이 값은 무효.
	CTime				m_system_shutdown_time = CTime(0);

	//모니터를 끄면 OS 가 사라진 모니터의 윈도우들을 visible 모니터로 자동 reposition 한다.
	//그 시점에 OnWindowPosChanged 가 발생해 의도치 않은 좌표가 레지스트리에 저장되면
	//다음 부팅 시 원치 않는 모니터에서 윈도우가 뜬다.
	//WM_DISPLAYCHANGE 는 OS reposition 보다 늦게 오므로 race 가 있다. 그래서 lock 활성화 시점을
	//더 이른 신호에 묶는다 — WM_POWERBROADCAST(GUID_MONITOR_POWER_ON: DPMS off) +
	//WM_QUERYENDSESSION(시스템 종료 시작). 모니터 개수 비교는 fallback.
	int					m_monitor_count_normal = 0;
	bool				m_position_save_locked = false;
	HPOWERNOTIFY		m_hpwr_monitor = NULL;
public:
	bool				is_position_save_locked() const { return m_position_save_locked; }
	//종료/콜백 경로에서도 phantom 좌표 굳지 않게 통합 검증.
	bool				should_skip_position_save(CWnd* pWnd) const;
protected:

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuColor();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMenuViewTimeList();
	afx_msg void OnMenuResetTimeListPos();
	afx_msg void OnMenuAlarmAfterMinutes();
	afx_msg void OnMenuFont();
	afx_msg void OnMenuAlwaysOnTop();
	afx_msg void OnMenuShutdown();
	afx_msg void OnMenuRestartExplorerTaskbarx();
	afx_msg void OnMenuClose();
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnMenuNvidiaInfo();

	// 시작프로그램으로 실행 시 Shell_TrayWnd 가 뒤늦게 올라오는 경우 복구.
	// Explorer 크래시 후 재시작에도 동일하게 사용.
	static UINT s_msg_taskbar_created;
	afx_msg LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);

	afx_msg void OnDisplayChange(UINT uBitsPerPixel, int cxScreen, int cyScreen);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, LPARAM lEventData);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnDestroy();
	//m_temperature 가 이동될 때 실시간 저장. CSCShapeDlg 가 Message_CSCShapeDlg 로 알림.
	LRESULT				on_message_CSCShapeDlg(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMenuSavePos1();
	afx_msg void OnMenuSavePos2();
	afx_msg void OnMenuSavePos3();
	afx_msg void OnMenuMoveToPos1();
	afx_msg void OnMenuMoveToPos2();
	afx_msg void OnMenuMoveToPos3();
protected:
	//메인/온도/타임리스트 3창의 현재 위치를 slot(1/2/3)에 스냅샷/복원.
	//레지스트리 구조: pos<slot>\screen (메인), pos<slot>\m_temperature\screen, pos<slot>\TimeListDlg\screen.
	//SaveWindowPosition 은 hidden/iconic 창을 skip 하므로, 해당 slot 이 비어있는 창은 move 시 건너뛴다.
	void				save_positions_to_slot(int slot);
	void				move_positions_from_slot(int slot);
};
