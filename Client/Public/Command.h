#pragma once

#include "Engine_Defines.h"
#include "LiDailin_Info.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

enum class MOVEMENT_COMMAND_TYPE
{
	NONE,
	MOVE,
};

enum class ACTION_COMMAND_TYPE
{
	NONE,
	ATTACK,
	ATTACK_Q,
	ATTACK_W,
	ATTACK_E,
	ATTACK_R,
};

struct MOVEMENT_COMMAND
{
	MOVEMENT_COMMAND_TYPE eCommandType{};
	_float3					vTargetPos{};
};

struct ACTION_COMMAND
{
	ACTION_COMMAND_TYPE eCommandType{};
	_float3				vTargetPos{};
	CGameObject*		pGameObject{};
};