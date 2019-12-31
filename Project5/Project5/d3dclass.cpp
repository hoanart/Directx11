#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

D3DClass::D3DClass(const D3DClass &)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backbufferptr;
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	//vsunc(수직동기화) 설정 저장
	m_vsync_enabled = vsync;
	
	//Direct3D 초기화 전에 그래픽카드 / 모니터 주사율을 알아야되기에 조회
	//DirectX 그래픽 인터페이스 팩토리 만듬
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 어댑터를 만듬
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//출력(모니터)에 대한 첫번째 어댑터 연결
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드의 개수 구함
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//디스플레이 모드에 대한 리스트 구조를 채워넣기
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//디스플레이모드에 대한 너비/높이 맞는 디스플레이 모드 찾기
	//적합한 것을 찾으면 모니터의 새로고치 비율의 분모와 분자 값 저장
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				//주사율의 분자 / 분모
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//어대터(그래픽카드)의 description을 가져옴
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//현재 그래픽카드의 메모리 용량을 MB단위로 저장
	m_videoCardMemory = (int)(adapterDesc.DedicatedSystemMemory / 1024 / 1024);

	//그래픽카드의 이름 char형 문자열 배열로 바꾼 뒤 저장
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//디스플레이 모드 리스틔 할당을 해제
	delete[] displayModeList;
	displayModeList = 0;

	//출력 어댑터를 할당 해제
	adapterOutput->Release();
	adapterOutput = 0;

	//어댑터 할당 해제
	adapter->Release();
	adapter = 0;

	//팩토리 객체를 할당 해제
	factory->Release();
	factory = 0;
	
	//보통 렌더링 시 백버퍼만 사용 프론트 버퍼와 swap 하여 화면 보임 - swapChain
	//Swap Chain description 초기화
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//하나의 백버퍼만 사용
	swapChainDesc.BufferCount = 1;

	//백버퍼의 너비와 높이를 설정
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//백버퍼로 일반적인 32bit의 서페이스를 지정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

}
