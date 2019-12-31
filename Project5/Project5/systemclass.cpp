#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{}

SystemClass::~SystemClass()
{}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//너비와 높이 초기화
	screenWidth = 0;
	screenHeight = 0;

	//윈도우 api로 초기화
	InitializeWindows(screenWidth, screenHeight);

	//input 객체 생성, 유저로부터 들어오는 키보드 입력을 처리위해 사용
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}
	// Initialize the input object.
	m_Input->Initialize();
	//graphics 객체 생성, 어플리케이션의 모든 그래픽 요소 그리는 일
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//graphics 객체 초기화
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}
	return true;
}

void SystemClass::Shutdown()
{
	
	//Graphics object return
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}
	
	//Input Object return
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;

	}
	// Window Shutdown
	ShutdownWindows();
	return;
}
// 루프 돌면서 어플리케이션 모든 작업 처리
// frame 함수에서 수행

void SystemClass::Run()
{
	MSG msg; //메세지 구조체의 변수
	bool done, result;

	//메세지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));
	
	//유저로부터 종료메시지를 받을 떄까지 루프
	done = false;
	while (!done)
	{
		//윈도우 메시지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//윈도우에서 어플리케이션 종료 요청시 빠져나감
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else // 요청이 없을 시에는 Frame 함수 실행
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}
	return;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//키보드 키가 눌렸는지 확인
	case WM_KEYDOWN:
	{
		//키가 눌렸다면 input 객체에 사실 전달해서 기록하도록함
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	
	//키보드가 눌린 키가 떼어졌는지 확인
	case WM_KEYUP:
	{
		//키가 떼어졌다면 input 객체에 전달해서 키 해제
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	
	//다른 메시지들은 사용x이므로 기본 메세지 처리기에 전달
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}
//모든 작업이 처리됨

bool SystemClass::Frame()
{
	bool result;

	//유저가  esc 누를 시 어플리케이셔 종료 희망 여부 확인
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//grapics 객체 작업 처리
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

//렌더링 하게 될 윈도우를 만드는 코드
void SystemClass::InitializeWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//외부 포인터를 이 객체로 설정
	ApplicationHandle = this;

	//어플리케이션의 이름 설정, 인스턴스 가져엄
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"Engine";

	//윈도우 클래스 기본 설정 맞추기
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX); //구조체크기(자기자신) 확인

	//윈도우 클래스를 등록
	RegisterClassEx(&wc);

	//모니터 화면 해상도 확인
	screenWidth = GetSystemMetrics(SM_CXSCREEN); //SystemMetrix_ComputerX?
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//풀스크린 모드 변수의 값에 따라 화면 설정
	if (FULL_SCREEN)
	{
		//화면 크기를 데스크톱 크기에 맞추고 색상을 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;//픽셀 당 비트 | 픽셀 폭 | 픽셀 노
		
		//풀스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN); //다른데스크톱으로 바뀌면, 모드 재설정X 

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춤
		posX = posY = 0;
	}
	else
	{
		//윈도우 모드일경우 800x600
		screenWidth = 800;
		screenHeight = 600;

		//창을 모니터 중앙에 오도록 함
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//설정한 것을 가지고 창을 만들고 그 핸들을 가져옴
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
		//Child끼리 겹칠경우 그리기영역에서 제외 | Child가 위치한 영역 그리기 영역에서 제외 | 팝업윈도우 만듬
	//윈도우를 화면에 표시, 활성화
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);//해당 윈도우 최상단에 오도록함
	SetFocus(m_hwnd);//포커스를 맞춤

	//마우스 커서 표시x
	ShowCursor(false);

	return;
}
//화면설정 되돌리고 윈도우와 그 핸들들을 되돌림
void SystemClass::ShutdownWindows()
{
	//마우스 커서 표시
	ShowCursor(true);

	//풀스크린 모드 나올떄 디스플레이 설정 변경
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//창 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//어플리케이션 인스턴스 제거
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//이 클래스에 대한 외부 포이터 참조 제거
	ApplicationHandle = NULL;
	
	return;
}
//윈도우시스템에서 메시지 보내는 것
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//윈도우 제거 되었는가
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	//윈도우 닫혔는지 확인
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	//다른 모든 메시지들은 system 클래스의 메시지 처리기에 전달
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
