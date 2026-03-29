#pragma once

#include <Windows.h>

namespace Client
{
	static constexpr unsigned int		g_iWinSizeX = { 1280 };
	static constexpr unsigned int		g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, LOGO, LOBBY, GAMEPLAY, ENDING, END };
}

enum class UILAYER {BACKGROUND, DECO_LAYER1, DECO_LAYER2, DECO_LAYER3, PANEL, SLOT, BUTTON, BUTTON_IMAGE};

enum class STAGE { LOBBY, SELECT, STAGE_END };

// 이 변수가 다른 곳에 정의되어 있다고 선언만 한다.
// 링크 단계에서 실제 정의(주소)와 연결된다.(이 헤더파일 포함하면 사용 가능)
extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

// Client namespace를 만든 적이 없어서 위에서 반드시 만들어줘야 에러 안 뜸.
// 이거 해주면 매번 cpp에서 namespace 안 붙여도 됨.
using namespace Client;

enum class CHAR_NAME{ LIDAILIN, HYUNWOO, CHARNAME_END};

constexpr const wchar_t* CharLobbyTex[]{
	L"Prototype_Texture_CharLobbyPick_LiDailin",
	L"Prototype_Texture_CharLobbyPick_Hyunwoo"
};