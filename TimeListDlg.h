#pragma once
#include "afxdialogex.h"

#include "Common/Functions.h"
#include "Common/colors.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CListCtrl/CSCListCtrl/SCListCtrl.h"
#include "Common/messagebox/CSCMessageBox/SCMessageBox.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/ResizeCtrl.h"
#include "Common/CDialog/SCShapeDlg/SCShapeDlg.h"
#include "Common/CDialog/SCThemeDlg/SCThemeDlg.h"

class CAlarmItem
{
public:
	CAlarmItem() {}
	CAlarmItem(CString _title, CTime _start, CTimeSpan _ts_duration, bool _is_locked, bool _is_floating)
	{
		title = _title;
		start = _start;
		ts_duration = _ts_duration;
		is_locked = _is_locked;
		is_floating = _is_floating;
	}

	//레지스트리 저장용 문자열. 필드 구분자는 '|' 이며 title 이 마지막이라 title 에 '|' 가 섞여도 안전하다.
	CString		to_string() const;
	//to_string() 형식을 파싱한다. 구분자가 모자라면 false 를 반환하며 이때 멤버는 변경되지 않는다.
	bool		from_string(const CString& str);

	CString		title;
	CTime		start = CTime(0);
	CTimeSpan	ts_duration = CTimeSpan(0);
	bool		is_locked = false;
	bool		is_floating = false;
	bool		is_paused = false;		//타이머 일시 정지. 종료 시각이 늘어난다.
	//발화 완료. 발화 판정이 remain == 0 정확 일치에서 remain <= 0 으로 바뀌었으므로 중복 발화를 막는다.
	bool		fired = false;
};

// CTimeListDlg 대화 상자
class CTimeListDlg : public CSCThemeDlg
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

	//부모(CMiniClock2Dlg)나 테마 변경 시점에 호출. base 가 색 복사 + DWM(dark/round/border)까지 처리하고,
	//여기서 자식 컨트롤들에 같은 테마를 전파한다. 개별 색 setter 를 쓰지 말 것 (Common claude.md §2.1) —
	//타이틀바가 없는 이 창은 외곽 프레임 색도 cr_back 이라, 이 경로를 거치지 않으면 DWM border 가
	//OS 기본 밝은 색으로 남는다.
	void			set_color_theme(const CSCColorTheme& theme);

protected:
	CSCMessageBox	m_msgbox;
	void			load_timelist();
	void			save_timelist();
	//구버전(REG_BINARY, item%02d)으로 저장된 항목을 신규 문자열 형식(alarm%02d)으로 1회 변환한다.
	//변환 후 legacy 키는 삭제하고 schema\timelist_version 을 올려 다시 실행되지 않게 한다.
	void			migrate_timelist_from_binary();
	//모든 항목의 col_remain 텍스트를 CAlarmItem (start + duration - now) 기준으로 동기화 후
	//남은 시각 오름차순 정렬. add/edit 직후 호출.
	void			refresh_remain_and_sort();
	//floating 항목이 1개도 없다면 0번 항목(가장 임박)을 floating 으로 보정.
	//정렬 직후 / 삭제 직후 등 리스트 변경 시점에 호출.
	void			ensure_floating();

	//만료 후 이 시간이 지나면 알람으로서 의미가 없다고 보고 (1) 발동시키지 않고 (2) 목록에서 삭제한다.
	//두 판정이 같은 기준을 써야 "울리지도 않았는데 남아있는" 또는 "한참 지나서 울리는" 항목이 안 생긴다.
	enum { alarm_expire_seconds = 600 };

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
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	CSCStatic m_static_alarm_list;
	CGdiButton m_check_autohide;
	CSCListCtrl m_list;
	CSCShapeDlg m_floating;
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
	afx_msg void OnEnterSizeMove();
	afx_msg void OnExitSizeMove();
};
