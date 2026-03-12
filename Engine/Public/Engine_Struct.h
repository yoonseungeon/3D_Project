#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	// Graphic Device 초기화에 필요한 변수들(구조체)
	typedef struct tagEngineDesc
	{
		HWND			hWnd;
		WINMODE			eWinMode;
		unsigned int	iViewportWidth, iViewportHeight;
		unsigned int	iNumLevels;
	}ENGINE_DESC;

	typedef struct tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;
	}VTXTEX;


}

#endif // Engine_Struct_h__
