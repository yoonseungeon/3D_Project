#pragma once

#include "Client_Defines.h"
#include "CBase.h"

// using namespace Engine;는 "지금 스코프에서 이름이 안 보이면 Engine 안에서도 한 번 찾아봐도 된다"는 규칙이다.
// 하지만 전방선언 class CGameInstance;는 만나는 순간 지금 스코프(전역)에 ::CGameInstance라는 새 타입을 등록하는 거라,
// Engine::CGameInstance면 namespace Engine { class CGameInstance; }로 해야 한다.
// using namespace Engine이 되어있더라도 namespace Engine로 감싸야 한다.
NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

#ifdef _DEBUG
class CImGui_Manager;
#endif

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance		= { nullptr };

	ID3D11Device*				m_pDevice				= { nullptr };
	ID3D11DeviceContext*	m_pContext				= { nullptr };

#ifdef _DEBUG
	CImGui_Manager*			m_pImGui_Manager	= { nullptr };
#endif

private:
	HRESULT Start_Level(LEVEL eStartLevelID);

	HRESULT Ready_Prototype_For_Static();

public:
	static CMainApp*	Create();
protected:
	virtual void		Free() override;
};

NS_END