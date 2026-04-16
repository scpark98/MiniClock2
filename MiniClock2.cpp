
// MiniClock2.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "MiniClock2.h"
#include "MiniClock2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMiniClock2App

BEGIN_MESSAGE_MAP(CMiniClock2App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMiniClock2App 생성

CMiniClock2App::CMiniClock2App()
{
	m_hMutex = NULL;
}

CMiniClock2App::~CMiniClock2App()
{
	if (m_hMutex)
	{
		::ReleaseMutex(m_hMutex);
		m_hMutex = NULL;
	}
}

// 유일한 CMiniClock2App 개체입니다.

CMiniClock2App theApp;


// CMiniClock2App 초기화

BOOL CMiniClock2App::InitInstance()
{
	m_hMutex = ::CreateMutex(NULL, FALSE, _T("MiniClock2"));
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//HWND	hWnd;;

		//중복실행 방지 및 기존 실행되는 프로그램이 minimized되어 있거나
		//inactive 상태이면 activate시키는 코드인데
		//GetHWndByExeFilename으로도 잘 얻어오지 못하는 듯하다.
		//따라서 dialog의 캡션 끝에 공백을 붙이고 FindWindow로 해당 윈도우를 찾아서
		//위의 액션을 수행하는 것이 가장 안전한듯하다.
		//참고로 공백을 붙이는 이유는 만약 윈도우 탐색기에서 현재 프로젝트 폴더를 열었다면
		//윈도우 탐색기의 타이틀도 이 프로젝트의 이름으로 표시되는 윈도우 버전이 있다.
		//따라서 이를 구분하기 위해 공백을 추가한다.
		//hWnd = GetHWndByExeFilename( GetExeFilename(false), true, true );
		//hWnd = ::FindWindow( NULL, _T("MiniClock") );
		CWnd* pWnd = FindWindowByCaption(_T("MiniClock2"), true);
		if (pWnd)
		{
			pWnd->SetForegroundWindow();
			pWnd->SetActiveWindow();
			return false;
		}
	}

	// Windows XP에서는 InitCommonControlsEx()를 필요로 합니다.
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("Legends Software"));

	CMiniClock2Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}


int CMiniClock2App::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinApp::ExitInstance();
}
