#pragma once

#include <Windows.h>

namespace Client
{
	static constexpr unsigned int		g_iWinSizeX = { 1280 };
	static constexpr unsigned int		g_iWinSizeY = { 720 };

	enum class LEVEL { STATIC, LOADING, LOGO, LOBBY, GAMEPLAY, ENDING, END };
}

// 이 변수가 다른 곳에 정의되어 있다고 선언만 한다.
// 링크 단계에서 실제 정의(주소)와 연결된다.(이 헤더파일 포함하면 사용 가능)
extern HWND g_hWnd;

// Client namespace를 만든 적이 없어서 위에서 반드시 만들어줘야 에러 안 뜸.
// 이거 해주면 매번 cpp에서 namespace 안 붙여도 됨.
using namespace Client;
