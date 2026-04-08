#pragma once

#include "Client_Defines.h"
#include "CContainerObject.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CPlayer final : public CContainerObject
{
public:
	enum PLAYER_STATE { P_IDLE, P_RUN, P_STATE_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint m_iCurState{ PLAYER_STATE::P_STATE_END };
	_uint m_iPreState{ PLAYER_STATE::P_STATE_END };

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

private:
	void Update_State(_float fTimeDelta);
	void Enter_State(PLAYER_STATE eNewState);
	void Execute_State(_float fTimeDelta);

	void Player_Input(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END