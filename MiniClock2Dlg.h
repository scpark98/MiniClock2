
// MiniClock2Dlg.h: 헤더 파일
//

#pragma once

#include "Common/SCGdiplusBitmap.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"
#include "Common/system/SysTrayIcon/SysTrayIcon.h"
#include "Common/device/nvidia/nvidia_info.h"

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
	};

	CSysTrayIcon		m_sys_tray;
	LRESULT				on_message_CSysTrayIcon(WPARAM, LPARAM);

	CNVidiaInfo			m_nvidia; 
	CSCShapeDlg			m_temperature;

	std::deque<std::deque<CSCParagraph>> m_para;
	CSCTextProperty		m_text_prop;

	CSCMessageBox		m_msgbox;

	bool				m_first_run = true;

	int					m_alpha = 255;
	void				render(Gdiplus::Bitmap* img);;
	void				rebuild_image();

	void				load_setting();
	void				save_setting();

	bool				m_is_dragging = false;
	POINT				m_drag_start = {};

	CString				m_system_shutdown;	//종료 시각(ex. "2350")

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
};
