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

	//vsunc(��������ȭ) ���� ����
	m_vsync_enabled = vsync;
	
	//Direct3D �ʱ�ȭ ���� �׷���ī�� / ����� �ֻ����� �˾ƾߵǱ⿡ ��ȸ
	//DirectX �׷��� �������̽� ���丮 ����
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� ����͸� ����
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//���(�����)�� ���� ù��° ����� ����
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ���� ����
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� ����
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//���÷��� ��忡 ���� ����Ʈ ������ ä���ֱ�
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//���÷��̸�忡 ���� �ʺ�/���� �´� ���÷��� ��� ã��
	//������ ���� ã���� ������� ���ΰ�ġ ������ �и�� ���� �� ����
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				//�ֻ����� ���� / �и�
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//�����(�׷���ī��)�� description�� ������
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//���� �׷���ī���� �޸� �뷮�� MB������ ����
	m_videoCardMemory = (int)(adapterDesc.DedicatedSystemMemory / 1024 / 1024);

	//�׷���ī���� �̸� char�� ���ڿ� �迭�� �ٲ� �� ����
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//���÷��� ��� ����Ʒ �Ҵ��� ����
	delete[] displayModeList;
	displayModeList = 0;

	//��� ����͸� �Ҵ� ����
	adapterOutput->Release();
	adapterOutput = 0;

	//����� �Ҵ� ����
	adapter->Release();
	adapter = 0;

	//���丮 ��ü�� �Ҵ� ����
	factory->Release();
	factory = 0;
	
	//���� ������ �� ����۸� ��� ����Ʈ ���ۿ� swap �Ͽ� ȭ�� ���� - swapChain
	//Swap Chain description �ʱ�ȭ
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//�ϳ��� ����۸� ���
	swapChainDesc.BufferCount = 1;

	//������� �ʺ�� ���̸� ����
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	//����۷� �Ϲ����� 32bit�� �����̽��� ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

}
