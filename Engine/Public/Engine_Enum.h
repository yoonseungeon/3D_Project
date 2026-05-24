#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };

	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };

	enum class RENDERID { PRIORITY, SHADOW, NONBLEND, NONLIGHT, BLEND, OUTLINE, UI, VISION_MASK, END_DECO };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END_DECO };

	enum class D3DTS { VIEW, PROJ, END_DECO };

	enum class LIGHT { DIRECTIONAL, POINT, END_DECO };

	enum class MODEL { NONANIM, ANIM, END_DECO };

	enum class COLLIDER { AABB, OBB, SPHERE, FRUSTUM, END_DECO };

	enum class DEFERRED { DEBUG, DIRECTIONAL, POINT, COMBINED, END_DECO };

	//// Dynamic 컴포넌트 경우 매 프레임마다 갱신해야하는 컴포넌트 집단
	//enum COMPONENTID { ID_DYNAMIC, ID_STATIC, ID_END };

	//enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	enum class DIMB { LBUTTON, RBUTTON, WHEEL, END_DECO };

	enum class DIMM { X, Y, WHEEL, END_DECO };

	enum class CELL_POINT { A, B, C, END_DECO };

	enum class LINE { AB, BC, CA, END_DECO };

	enum class PARALLEL_UPDATE_MODE { PARALLEL, SINGLE };
}
#endif // Engine_Enum_h__
