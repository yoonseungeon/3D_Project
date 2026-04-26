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

	struct KEYFRAME
	{
		// 애니메이션에서 행렬 보간은 성분 나눠서 해야 함. + 함수 제공 x
		// 그래서 성분 나눠서 보간하고 행렬을 만들어 줌.
		XMFLOAT3		vScale;
		XMFLOAT4		vRotation; // 쿼터니언 회전
		XMFLOAT3		vTranslation;

		// keyframe 이 상태는 어떤 위치에서 재생되어야 하는지
		float			fTrackPosition;
	};

	struct VTXPOS
	{
		XMFLOAT3		vPosition;

		static const unsigned int		iNumElements = { 1 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
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

	struct VTXCUBE
	{
		XMFLOAT3		vPosition;
		// 스카이 박스는 방향 벡터
		XMFLOAT3		vTexcoord;

		static const unsigned int		iNumElements = { 2 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	};

	struct VTXMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;

		static const unsigned int		iNumElements = { 5 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};

	struct VTXANIMMESH
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;

		// 이 정점의 경우 뼈의 정보를 최대 4개 영향을 받을 수 있다.(int 4개라서)
		// assimp에서는 최대 8개?까지 가능?
		// 근데 대부분 4개 안으로 다 됨.
		XMUINT4			vBlendIndex;
		// 가중치(뼈의 영향을 얼만큼 받는가)도 개수 맞춰줘야 함.(모든 가중치의 합은 1)
		XMFLOAT4		vBlendWeight;

		static const unsigned int		iNumElements = { 7 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	};

	struct VTXPARTICLE_INSTANCE
	{
		XMFLOAT4 vRight{};
		XMFLOAT4 vUp{};
		XMFLOAT4 vLook{};
		XMFLOAT4 vTranslation{};

		XMFLOAT2 vLifeTime{};
	};

	// 따로 정의. 왜냐하면 한 번에 전달되기 때문에 합쳐줘야 함.
	struct VTXRECT_INSTANCE_DESC
	{
		static const unsigned int iNumElements = { 7 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},


			// 4. Input 슬롯(장치의 바인딩 버텍스 버퍼 중 몇 번째) 
			// 6. 입력 데이터가 정점마다 바뀌는지 인스턴스마다 바뀌는지
			// 7. InstanceDataStepRate 인스턴스 몇 개마다 넘길지
			{"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};
	};

	struct VTXPOINT_INSTANCE_DESC
	{
		static const unsigned int		iNumElements = { 6 };

		static constexpr D3D11_INPUT_ELEMENT_DESC   Elements[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			// TexCoord 필요없음. 점이라서

			// 행렬을 한 번에 던지기 위해 WORLD 시멘틱 사용
			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},

			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		};
	};

	struct VIEWPORT_SIZE
	{
		unsigned int x{};
		unsigned int y{};
	};

	struct MODEL_LOCAL_MIN_MAX
	{
		XMFLOAT3 vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
		XMFLOAT3 vMax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
	};
}

#endif // Engine_Struct_h__
