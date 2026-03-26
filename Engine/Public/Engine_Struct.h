#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	// Graphic Device 초기화에 필요한 변수들(구조체)
	struct ENGINE_DESC
	{
		HINSTANCE		hInstance;
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iViewportWidth, iViewportHeight;
		unsigned int	iNumLevels;
	};

	struct LIGHT_DESC
	{
		LIGHT			eType;
		XMFLOAT4		vDiffuse, vAmbient, vSpecular;

		XMFLOAT4		vDirection;
		XMFLOAT4		vPosition;
		float			fRange;
	};

	struct VTXTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static constexpr unsigned int		iNumElements = { 2 };

		// 셰이더에 넘길 정점 입력 형식 정보 배열(구조체 하나가 정점 구조체 안에 선언된 멤버 변수 하나 묘사)
		// 1. 문자열(시맨틱) 2. 시맨틱 idx(같은 시맨틱 여러 개면) 3. 포맷(float3 개, float 2개)
		// 4. IA에 바인딩된 정점 버퍼 중 몇 번째에서 꺼내와야 되는지 5. 구조체 멤버 변수 사작되는 byte offset
		// 6. VERTEX_DATA(일반적인 정점) or INSTANCE_DATA 7. INSTANCE_DATA인 StepRate 설정
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};

	struct VTXNORTEX
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static constexpr unsigned int iNumElements = { 3 };

		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};
}

#endif // Engine_Struct_h__
