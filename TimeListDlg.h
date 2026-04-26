#pragma once
#include "afxdialogex.h"

#include "Common/Functions.h"
#include "Common/colors.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/ResizeCtrl.h"
#include "Common/CDialog/SCShapeDlg/SCShapeDlg.h"

class CAlarmItem
{
public:
	CAlarmItem(TCHAR* _title, CTime _start, CTimeSpan _ts_duration, bool _is_locked, bool _is_floating)
	{
		_tcscpy_s(title, sizeof(title) / sizeof(TCHAR),	_title);
		start = _start;
		ts_duration = _ts_duration;
		is_locked = _is_locked;
		is_floating = _is_floating;
		is_paused = false;
	}

	TCHAR		title[16] = { 0, };
	CTime		start = 0;
	CTimeSpan	ts_duration;
	bool		is_locked = false;
	bool		is_floating = false;
	bool		is_paused = false;		//타이머 일시 정지. 종료 시각이 늘어난다.
};

// CTimeListDlg 대화 상자
class CTimeListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeListDlg)

public:
	CTimeListDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTimeListDlg();

	CResizeCtrl		m_resize;

	//list와 m_item이 별도로 존재하면 안되고 CAlarmItem*을 list의 item data로 넣어서 관리하는 방식으로 바꿔야 한다.
	//그래야 정렬, 수정, 삭제시에도 쉽게 관리된다.
	//std::deque<CAlarmItem>	m_item;

	void			add(CString title, CString duration, bool add_favorite = false, bool floating = false, bool save_list = true);
	LRESULT			on_message_CSCShapeDlg(WPARAM wParam, LPARAM lParam);
	void			set_alpha(int alpha);

protected:
	CSCMessageBox	m_msgbox;
	void			load_timelist();
	void			save_timelist();
	//모든 항목의 col_remain 텍스트를 CAlarmItem (start + duration - now) 기준으로 동기화 후
	//남은 시각 오름차순 정렬. add/edit 직후 호출.
	void			refresh_remain_and_sort();

	enum TIMER_ID
	{
		timer_time = 0,
	};

	enum LIST_COLUMN
	{
		//"이름,60;등록 시각,80;간격,44;알람 시각,80;남은 시각,80;
		col_title = 0,
		col_start,
		col_duration,
		col_end,
		col_remain,
		col_date,
	};
	void			init_list();

	enum TIMELIST_POPUP_MENU
	{
		menu_favorite_start = 8000,
	};
	void			on_menu_favorites(UINT nID);

	//1:13(=1h 13m), 1d 20m(=1일 20분) 등의 문자열을 총 minutes로 변환한다.
	int				get_minutes_from_duration_string(CString& duration);

	CSCShapeDlg		m_floating;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIME_LIST };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	CSCStatic m_static_alarm_list;
	CGdiButton m_check_autohide;
	CVtListCtrlEx m_list;
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnBnClickedCheckAutoHide();
	afx_msg void OnMenuDelete();
	afx_msg void OnNMDblclkListTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuResetStartTime();
	afx_msg void OnMenuFloating();
	afx_msg void OnMenuCopyToClipboard();
	afx_msg void OnMenuLockListitem();
	afx_msg void OnLvnEndLabelEditListTime(NMHDR* pNMHDR, LRESULT* pResult);
};
