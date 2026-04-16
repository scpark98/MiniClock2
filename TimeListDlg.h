#pragma once
#include "afxdialogex.h"

#include "Common/Functions.h"
#include "Common/colors.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CListCtrl/CVtListCtrlEx/VtListCtrlEx.h"
#include "Common/messagebox/XMessageBox/XMessageBox.h"
#include "Common/CStatic/SCStatic/SCStatic.h"
#include "Common/ResizeCtrl.h"

class CAlarmItem
{
public:
	CAlarmItem(TCHAR* _title, CTime _start, int _duration, bool _is_lock, bool _is_floating)
	{
		_tcscpy_s(title, sizeof(title) / sizeof(TCHAR),	_title);
		start = _start;
		duration = _duration;
		is_lock = _is_lock;
		is_floating = _is_floating;
		is_paused = false;
	}

	TCHAR		title[128] = { 0, };
	CTime		start = 0;
	int			duration = 0;			//시작부터 종료까지의 total seconds
	bool		is_lock = false;
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

	CResizeCtrl				m_resize;
	std::deque<CAlarmItem>	m_item;

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
};
