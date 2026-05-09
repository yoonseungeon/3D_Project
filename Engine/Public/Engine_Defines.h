#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <Effects11/d3dx11effect.h>
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/WICTextureLoader.h>

#include <directxtk/SpriteBatch.h>
#include <directxtk/SpriteFont.h>
#include <directxtk/ScreenGrab.h>

#include <directxtk/PrimitiveBatch.h>
#include <directxtk/VertexTypes.h>
#include <directxtk/Effects.h>

using namespace DirectX;


#include <vector>
#include <list>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <atomic>
#include <fstream>
#include <memory>

using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#include "MyModel_Format.h"

namespace Engine
{
	static const _wstring g_strTransformTag = TEXT("Com_Transform");
	static constexpr _uint g_iNumMeshBones = { 512 };

	//constexpr unsigned int g_iMaxWidth = 2560;
	//constexpr unsigned int g_iMaxHeight = 1440;
	//constexpr unsigned int g_iMaxWidth = 1280;
	//constexpr unsigned int g_iMaxHeight = 720;
	
	constexpr unsigned int g_iMaxWidth = 16384;
	constexpr unsigned int g_iMaxHeight = 9216;	
	
	//constexpr unsigned int g_iMaxWidth = 8192;
	//constexpr unsigned int g_iMaxHeight = 4608;
}

#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

// 이거 해주면 매번 cpp에서 namespace 안 붙여도 됨.
using namespace Engine;

#endif // Engine_Define_h__
