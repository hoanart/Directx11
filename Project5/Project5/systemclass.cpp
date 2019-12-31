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

	//�ʺ�� ���� �ʱ�ȭ
	screenWidth = 0;
	screenHeight = 0;

	//������ api�� �ʱ�ȭ
	InitializeWindows(screenWidth, screenHeight);

	//input ��ü ����, �����κ��� ������ Ű���� �Է��� ó������ ���
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}
	// Initialize the input object.
	m_Input->Initialize();
	//graphics ��ü ����, ���ø����̼��� ��� �׷��� ��� �׸��� ��
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//graphics ��ü �ʱ�ȭ
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
// ���� ���鼭 ���ø����̼� ��� �۾� ó��
// frame �Լ����� ����

void SystemClass::Run()
{
	MSG msg; //�޼��� ����ü�� ����
	bool done, result;

	//�޼��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));
	
	//�����κ��� ����޽����� ���� ������ ����
	done = false;
	while (!done)
	{
		//������ �޽��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�����쿡�� ���ø����̼� ���� ��û�� ��������
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else // ��û�� ���� �ÿ��� Frame �Լ� ����
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
		//Ű���� Ű�� ���ȴ��� Ȯ��
	case WM_KEYDOWN:
	{
		//Ű�� ���ȴٸ� input ��ü�� ��� �����ؼ� ����ϵ�����
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	
	//Ű���尡 ���� Ű�� ���������� Ȯ��
	case WM_KEYUP:
	{
		//Ű�� �������ٸ� input ��ü�� �����ؼ� Ű ����
		m_Input->KeyUp((unsigned int)wparam);
		return 0;
	}
	
	//�ٸ� �޽������� ���x�̹Ƿ� �⺻ �޼��� ó���⿡ ����
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
	}
}
//��� �۾��� ó����

bool SystemClass::Frame()
{
	bool result;

	//������  esc ���� �� ���ø����̼� ���� ��� ���� Ȯ��
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//grapics ��ü �۾� ó��
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

//������ �ϰ� �� �����츦 ����� �ڵ�
void SystemClass::InitializeWindows(int &screenWidth, int &screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//�ܺ� �����͸� �� ��ü�� ����
	ApplicationHandle = this;

	//���ø����̼��� �̸� ����, �ν��Ͻ� ������
	m_hinstance = GetModuleHandle(NULL);
	m_applicationName = L"Engine";

	//������ Ŭ���� �⺻ ���� ���߱�
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
	wc.cbSize = sizeof(WNDCLASSEX); //����üũ��(�ڱ��ڽ�) Ȯ��

	//������ Ŭ������ ���
	RegisterClassEx(&wc);

	//����� ȭ�� �ػ� Ȯ��
	screenWidth = GetSystemMetrics(SM_CXSCREEN); //SystemMetrix_ComputerX?
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//Ǯ��ũ�� ��� ������ ���� ���� ȭ�� ����
	if (FULL_SCREEN)
	{
		//ȭ�� ũ�⸦ ����ũ�� ũ�⿡ ���߰� ������ 32bit
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;//�ȼ� �� ��Ʈ | �ȼ� �� | �ȼ� ��
		
		//Ǯ��ũ���� �´� ���÷��� ����
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN); //�ٸ�����ũ������ �ٲ��, ��� �缳��X 

		// �������� ��ġ�� ȭ���� ���� ���� ����
		posX = posY = 0;
	}
	else
	{
		//������ ����ϰ�� 800x600
		screenWidth = 800;
		screenHeight = 600;

		//â�� ����� �߾ӿ� ������ ��
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//������ ���� ������ â�� ����� �� �ڵ��� ������
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
		//Child���� ��ĥ��� �׸��⿵������ ���� | Child�� ��ġ�� ���� �׸��� �������� ���� | �˾������� ����
	//�����츦 ȭ�鿡 ǥ��, Ȱ��ȭ
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);//�ش� ������ �ֻ�ܿ� ��������
	SetFocus(m_hwnd);//��Ŀ���� ����

	//���콺 Ŀ�� ǥ��x
	ShowCursor(false);

	return;
}
//ȭ�鼳�� �ǵ����� ������� �� �ڵ���� �ǵ���
void SystemClass::ShutdownWindows()
{
	//���콺 Ŀ�� ǥ��
	ShowCursor(true);

	//Ǯ��ũ�� ��� ���Ë� ���÷��� ���� ����
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//â ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	//���ø����̼� �ν��Ͻ� ����
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	//�� Ŭ������ ���� �ܺ� ������ ���� ����
	ApplicationHandle = NULL;
	
	return;
}
//������ý��ۿ��� �޽��� ������ ��
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//������ ���� �Ǿ��°�
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	//������ �������� Ȯ��
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	//�ٸ� ��� �޽������� system Ŭ������ �޽��� ó���⿡ ����
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}
