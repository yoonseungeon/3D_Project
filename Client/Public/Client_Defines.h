#pragma once

#include "Character_Meta.h"
#include "Map_Meta.h"
#include "Skill_Info.h"
#include "Item_Meta.h"

#include <Windows.h>

#include "nlohmann/json.hpp"

namespace Client
{
	static constexpr unsigned int		g_iWinSizeX = { 1280 };
	static constexpr unsigned int		g_iWinSizeY = { 720 };

	static constexpr float fDefaultFontSize = (1.f + (static_cast<float>(g_iWinSizeY) / 720.f - 1.f) * 0.7f) * 0.6f;

	enum class LEVEL { STATIC, LOADING, LOGO, LOBBY, GAMEPLAY, ENDING, END_DECO };

	enum class UILAYER { INGMAE, BACKGROUND, DECO_LAYER1, DECO_LAYER2, DECO_LAYER3, PANEL, PANEL_DECO, SLOT_PANEL, SLOT, SLOT_DECO, BUTTON_IMAGE, BUTTON, BUTTON_IMAGE_OVER, PANEL_OVER, PANEL_DECO_OVER, PANEL_SLOT_OVER, BG_END, END_DECO, END};

	enum class STAGE { LOBBY, SELECT, STAGE_END };

	enum class ANIMMESH_SHADER { DEFAULT, SHADOW, OUTLINE};
	enum class MESH_SHADER { DEFAULT, NORMALMAP, WATER, SHADOW, OUTLINE, OUTLINE_NO_DEPTH_TEST, STENCILWRITE, NORMALMAPSTENCILWRITE};
	enum class EFFECT_SHADER { DEFAULT, ALPHABLEND };
	enum class MESHEFFECT_SHADER { DEFAULT, ALPHABLEND, BLEND, Q_SPIN, Q_DRAGON, WIND, CONVERT_ALPHA };

	enum class Collision_Layer
	{
		PLAYER = 1 << 0,
		MONSTER = 1 << 1,
		ITEMBOX = 1 << 2,
		ITEMBOX_COLLECTIBLE = 1 << 3,
		SKILL = 1 << 4,
		ENEMY_SKILL = 1 << 5,
		ENEMY = 1 << 6,
	};

	enum class SOUND_KEY {
		BGM_LOBBY, BGM_SELECT, LOBBY_TAB_HOVER, LOBBY_TAB_CLICK, CHAR_HOVER, CHAR_CLICK, SELECT_CLICK, GAMESTART_CLICK,
		LIDAILIN_SELECT, HYUNWOO_SELECT,
		LIDAILIN_NORMAL_ATK, LIDAILIN_NORMAL_ATK_P, LIDAILIN_Q1, LIDAILIN_Q2, LIDAILIN_Q3, LIDAILIN_W, LIDAILIN_E, LIDAILIN_E_P, LIDAILIN_E_VOICE, LIDAILIN_E_HIT, LIDAILIN_R_DASH, LIDAILIN_R_HIT
	};
	enum class SOUND_CHANNEL_LOBBY { BGM, SKIN_SELECT, FIXED_EFFECT1 };
	enum class SOUND_CHANNEL_GAMEPLAY { FIXED_EFFECT1 };

	static constexpr float		fColliderCenterY = { 0.5f };

	struct INVENTORY_SLOT
	{
		int iItemId{ -1 };
		unsigned int iItemCnt{};
	};

	struct EQUIPMENT_SLOT
	{
		int iItemId{ -1 };
		ITEM_TYPE eItemType{};
	};

	struct DAMAGE_INFO
	{
		class CUnit* pUnit{};
		int iDamage{};
		bool bStun{};
		float fStunTime{};
	};
}


// 이 변수가 다른 곳에 정의되어 있다고 선언만 한다.
// 링크 단계에서 실제 정의(주소)와 연결된다.(이 헤더파일 포함하면 사용 가능)
extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

// Client namespace를 만든 적이 없어서 위에서 반드시 만들어줘야 에러 안 뜸.
// 이거 해주면 매번 cpp에서 namespace 안 붙여도 됨.
using namespace Client;
