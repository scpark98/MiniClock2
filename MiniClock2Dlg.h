
// MiniClock2Dlg.h: 헤더 파일
//

#pragma once

#include "Common/SCGdiplusBitmap.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"
#include "Common/system/SysTrayIcon/SysTrayIcon.h"
#include "Common/device/nvidia/nvidia_info.h"
#include "Common/device/gpu_usage.h"

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
	};

	CSysTrayIcon		m_sys_tray;
	LRESULT				on_message_CSysTrayIcon(WPARAM, LPARAM);

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
	void				render(Gdiplus::Bitmap* img);;
	void				rebuild_image();

	void				load_setting();
	void				save_setting();

	bool				m_is_dragging = false;
	POINT				m_drag_start = {};

	CString				m_system_shutdown;	//종료 시각(ex. "2350")

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
};
