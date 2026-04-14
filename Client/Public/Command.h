#pragma once

#include "Engine_Defines.h"
#include "LiDailin_Info.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

enum COMMAND_TYPE
{
	NONE,
	MOVE,
	ATTACK_Q,
};

struct COMMAND
{
	COMMAND_TYPE eCommandType{};
	_float3 vTargetPos{};
	CGameObject* pGameObject{};
};