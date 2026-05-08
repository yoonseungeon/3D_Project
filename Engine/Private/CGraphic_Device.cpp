#include "CGraphic_Device.h"

CGraphic_Device::CGraphic_Device()
{

}

HRESULT CGraphic_Device::Initialize(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 그래픽카드가 dx 몇 버전까지 지원하는지에 대한 정보를 받아옴.
	D3D_FEATURE_LEVEL			FeatureLV;

	/* dx9: 장치 초기화를 하기위한 설정을 쭈욱 하고나서 최종적으로 장치객체를 생성한다. */
	/* dx11: 우선적으로 장치 객체를 생성하고 장치객체를 통해서 기타 초기화작업 및 설정을 해나간다. */

	// m_pDevice와, m_pDeviceContext 두 개의 컴 객체를 만든다.
	if (FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;



	/* SwapChain : 더블버퍼링. 전면과 후면버퍼를 번갈아가며 화면에 보여준다.(Present) */

	/* 스왑체인객체를 생성했고, 생성한 스왑체인 객체가 백버퍼를 내장한다. 백버퍼를 생성하기 위한 ID3D11Texture2D 만든거야. */
	/* 스왑체인 객체를 만들면서 백버퍼에 해당하는 ID3D11Texture2D객체를 만들어 스왑체인 객체가 내장한다. */
	if (FAILED(Ready_SwapChain(hWnd, isWindowed, iWinSizeX, iWinSizeY)))
		return E_FAIL;

	/* 스왑체인이 들고 있는 텍스쳐 2D를 가져와서 이를 바탕으로 백버퍼 렌더타겟 뷰를 만든다.*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(iWinSizeX, iWinSizeY)))
		return E_FAIL;


	// 생성한 View들 연결(바인딩) 필요

	/* 장치에 바인드해놓을 렌더 타겟들과 뎁스스텐실뷰를 세팅한다. */
	/* 장치는 동시에 최대 4(dx9) -> 8(dx11)개의 렌더타겟을 들고 있을 수 있다. */
	// 나중에 색/노말/디퓨즈 등을 여러 RenderTarget에 저장해두고, 셰이더에서 읽어 사용할 것이다.
	ID3D11RenderTargetView* pRTVs[] = {
		m_pBackBufferRTV,
	};


	/* 렌더타겟의 픽셀 수와 깊이스텐실버퍼의 픽셀수가 서로 다르다면 절대 렌더링이 불가능해진다. */
	// OM: Output Merger 내가 출력할 때 이용할 데이터를 모아서 세팅하는 공간(단계)
	// 깊이 테스트를 쓰려면 DSV도 함께 바인딩한다.
	m_pDeviceContext->OMSetRenderTargets(1, pRTVs,
		m_pDepthStencilView);

	// 뷰포트 영역 직접 설정. 클라이언트 영역 어디에서부터 얼마나 그릴지
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)iWinSizeX;
	ViewPortDesc.Height = (_float)iWinSizeY;
	// 깊이 범위 0 ~ 1
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	// 뷰포트 등록
	// 첫 번째 인자: 뷰포트 개수
	// 두 번째 인자: 뷰포트 설정
	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDevice = m_pDevice;
	*ppContext = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(const _float4* pClearColor)
{
	if (m_pDeviceContext == nullptr)
		return E_FAIL;

	/* DX9기준 : Clear함수는 백버퍼, 깊이스텐실버퍼를 한꺼번에 지운다.  */
	// m_pGraphic_Device->Clear(어떤 영역만큼 지울까, 어떤 것들을 지울까? , 뭘로 지울가. );	

	/* 백버퍼를 초기화한다.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, reinterpret_cast<const _float*>(pClearColor));

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (m_pDeviceContext == nullptr)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (m_pSwapChain == nullptr)
		return E_FAIL;

	/* 전면 버퍼와 후면 버퍼를 교체하여 후면 버퍼를 전면으로 보여주는 역할을 한다. */
	/* 후면 버퍼를 직접 화면에 보여줄게. */
	// 첫 번째 인자: 수직 동기 신호(V-Blank)를 몇 번 보낸 후에 화면을 교체할 것인가(0: 함수가 호출되는 즉시 전면 버퍼와 후면 버퍼를 교체)
	// 두 번째 인자: Flags(가변 주사율, 최적화, 디버깅 등)
	return m_pSwapChain->Present(0, 0);
}


HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, WINMODE isWindowed, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태 */
	// 스왑 체인 생성을 위한 설정값들을 담는 구조체 변수
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/* 백버퍼 == 텍스쳐 */
	/* 텍스처(백버퍼 == ID3D11Texture2D)를 생성하는 행위 */
	SwapChain.BufferDesc.Width = iWinCX;	/* 가로 픽셀 수 */
	SwapChain.BufferDesc.Height = iWinCY;	/* 세로 픽셀 수 */

	/* float4(1.f, 1.f, 1.f, 1.f) */
	/* float4(1.f, 0.f, 0.f, 1.f) */

	// 이 백버퍼(텍스쳐) 한 칸 한 칸은 어떤 정보를 가질까?
	/* D3DFMT_A8R8G8B8*/ /* 만든 픽셀하나의 데이터 정보: 32Bit 픽셀 생성하되 부호가 없는 정규화된 수를 저장할게 */
	// UNORM: 정규화된 실수를 담는다.
	// 정규화된 실수: 0 ~ 1사이의 실수를 정규화된 실수라고 한다.
	SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 모니터 주사 방식 선택(하드웨어와 드라이버가 알아서 최적의 상태를 선택)
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// 버퍼의 해상도와 모니터의 실제 출력 해상도가 일치하지 않을 때, 이미지를 어떻게 배치할지 결정하는 옵션(하드웨어와 드라이버가 알아서 최적의 상태를 선택)
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* 스케치북에 사과를 그릴꺼야. */
	/* RENDER_TARGET : 그림을 당하는 대상. 스케치북 */
	// 백 버퍼를 어떤 용도로 사용할지 지정하는 설정(렌더링 결과물을 기록하는 렌더 타겟으로 사용하겠다. 이 설정을 하지 않으면 해당 버퍼에 그림을 그릴 수 없다.)
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// 스왑 체인에서 관리할 백 버퍼의 개수를 지정
	SwapChain.BufferCount = 1;

	// 전체 화면인 경우, 요청할 디스플레이 주사율 힌트(값 설정할 경우 하드 코딩 x)
	// 0: unspecified(창 모드라서 의미 없어서 0으로)
	// 분자
	SwapChain.BufferDesc.RefreshRate.Numerator = 0;
	// 분모
	SwapChain.BufferDesc.RefreshRate.Denominator = 0;

	/* 멀티샘플링 : 안티얼라이징 (계단현상방지) */
	/* 나중에 배울 후처리(쉐이더) 렌더링에서 멀티샘플링 지원되지 않아 설정 x */
	// 나중에 직접 수행해줄 수 있다.
	// 드라이버가 제공하는 안티앨리어싱 품질 레벨 중 몇 번째를 쓸지(Count가 1일 땐 연산할 품질이 없으므로 무조건 0)
	SwapChain.SampleDesc.Quality = 0;
	// 섞을 픽샐 개수(1 == 사용 x)
	SwapChain.SampleDesc.Count = 1;

	// 어떤 창에 그림을 출력할지 지정
	SwapChain.OutputWindow = hWnd;
	// 창 모드(true)로 실행할지, 전체 화면(false)으로 실행할지 결정
	SwapChain.Windowed = static_cast<BOOL>(isWindowed);
	// 버퍼 교체 후 백 버퍼 어떻게 처리할지(폐기)
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스처(ID3D11Texture2D)를 생성했다. */
	// 스왑 체인 객체 생성
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (m_pDevice == nullptr)
		return E_FAIL;

	/* 내가 앞으로 사용 하기위한 용도의 텍스쳐를 생성하기위한 베이스 데이터를 가지고 있는 객체이다. */
	/* 내가 앞으로 사용 하기위한 용도의 텍스쳐 : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView */
	// Swap Chain에서 만든 ID3D11Texture2D가 있어도, 실제로 쓰려면 용도에 맞는 뷰(RTV/SRV 등)를 생성해 그 뷰로 사용해야 한다.
	ID3D11Texture2D* pBackBufferTexture = nullptr;

	/* 스왑체인이 들고있던 텍스처를 가져와봐. */
	// 첫 번째 인자: 액세스할 버퍼의 인덱스
	// 두 번째 인자: 가져올 버퍼 리소스를 어떤 인터페이스 타입으로 해석하여 반환받을지 결정하는 고유 식별자
	// 세 번째 인자: 리턴값
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	/* 실제 렌더타겟용도로 사용할 수 있는 텍스쳐 타입(ID3D11RenderTargetView)의 객체를 생성한다. */
	// pBackBufferTexture : 비디오 메모리(VRAM)에 할당된 순수 데이터 블록으로, 리소스 자체로는 GPU의 쓰기(Write) 권한이 없음.
	// m_pBackBufferRTV : 렌더 타겟으로 바인딩하기 위한 RTV(View)를 생성하고, 이 뷰(View)를 통해 해당 백버퍼(pBackBufferTexture)에 렌더링 결과를 기록(Write)하도록 지정할 수 있음.
	// 첫 번째 인자: 렌더링 결과가 기록될 대상 리소스
	// 두 번째 인자: 생성할 뷰의 데이터 해석 방식을 정의하는 구조체(D3D11_RENDER_TARGET_VIEW_DESC 구조체를 생략. 리소스 생성 시 설정된 설정값 기준으로 기본값 자동 선택)
	// 세 번째 인자: 리턴값
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint iWinCX, _uint iWinCY)
{
	if (m_pDevice == nullptr)
		return E_FAIL;

	// DSV는 메모리를 가진 실체(리소스)가 아니라 뷰(접근 창구)라서, Depth/Stencil을 저장할 Texture2D를 먼저 만든다.
	ID3D11Texture2D* pDepthStencilTexture = { nullptr };

	D3D11_TEXTURE2D_DESC TextureDesc{};

	/* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 테스트가 가능해진다. */
	/* 픽셀의 수가 다르면 아예 렌더링을 못함. */
	// 해상도(Width/Height)나 MSAA 샘플 수가 다르면 RTV와 DSV를 함께 바인딩할 수 없어, 깊이 테스트가 정상 적용되지 않는다.
	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	// 깊이 버퍼는 RTV와 1:1로 사용하므로 밉맵(축소 단계)이 필요 없다.
	TextureDesc.MipLevels = 1;
	// 같은 규격의 2D 텍스처를 몇 장(슬라이스) 만들지
	TextureDesc.ArraySize = 1;
	// DXGI_FORMAT_D24_UNORM_S8_UINT
	// - Depth 24bit : UNORM(정수로 저장하지만 0~1로 해석됨) → 투영 후 depth 범위(0~1) 값을 저장/비교(깊이 테스트)
	// - Stencil 8bit: 사용자 마스크 값(0~255) → 조건부 렌더링/마스킹 등에 사용(예: 실내 진입 시 지붕 숨김, 외곽선만 그리기, 거울 영역 등)
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// MSAA(Multisample anti-aliasing)
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* 동적? 정적?  */
	// 사용자는 이 리소스를 직접 Lock/Unlock해서 값(데이터)을 갱신하지 않는다. -> 정적
	// 깊이 값 기록/갱신은 렌더링 중에 GPU(DirectX 파이프라인)가 수행한다. -> 상관 x
	/* 정적 */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	// Texture2D 생성 시 지정한 BindFlags에 따라 만들 수 있는 View 종류가 정해진다(여러 플래그면 여러 View 생성 가능).
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL /* | D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE */;
	// CPU가 직접 접근할 때만 의미가 있는 값이라, 여기선 0으로 둔다.
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// 첫 번째 인자: 만들 텍스처 설정값(구조체)
	// 두 번째 인자: 초기 데이터(nullptr: 없음)
	// 세 번째 인자: 리턴값
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTargetView */
	/* ShaderResourceView */
	/* DepthStencilView */

	// 첫 번째 인자: 깊이/스텐실 값이 기록될 대상 리소스
	// 두 번째 인자: 생성할 뷰의 데이터 해석 방식을 정의하는 구조체(D3D11_DEPTH_STENCIL_VIEW_DESC 구조체를 생략. 리소스 생성 시 설정된 설정값 기준으로 기본값 자동 선택)
	// 세 번째 인자: 리턴값
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}


CGraphic_Device* CGraphic_Device::Create(HWND hWnd, WINMODE isWindowed, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice, ID3D11DeviceContext** ppDeviceContextOut)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(hWnd, isWindowed, iWinSizeX, iWinSizeY, ppDevice, ppDeviceContextOut)))
	{
		MSG_BOX("Failed to Created: CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif

	Safe_Release(m_pDevice);
}
