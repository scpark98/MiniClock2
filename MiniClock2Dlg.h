
// MiniClock2Dlg.h: 헤더 파일
//

#pragma once

#include "Common/SCGdiplusBitmap.h"


// CMiniClock2Dlg 대화 상자
class CMiniClock2Dlg : public CDialogEx
{
// 생성입니다.
public:
	CMiniClock2Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CSCGdiplusBitmap	m_img;
	//CSCShapeDlg		m_main;

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

	std::deque<std::deque<CSCParagraph>> m_para;
	CSCTextProperty		m_text_prop;

	int					m_alpha = 255;
	void				render(Gdiplus::Bitmap* img);;
	void				rebuild_image();

	void				load_setting();
	void				save_setting();


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
};
