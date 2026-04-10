#pragma once

#include "Client_Defines.h"
#include "CContainerObject.h"

NS_BEGIN(Engine)
class CMove;
NS_END

NS_BEGIN(Client)

class CBody_Player;

class CPlayer final : public CContainerObject
{
public:
	enum ACTION_STATE { IDLE_P, RUN_P, REST_P, END_P };

	enum REQUEST_FLAG {
		RQ_IDLE = 1 << 0,
		RQ_RUN = 1 << 1,
		RQ_REST = 1 << 2
	};

	enum ControlFlag {
		BLOCK_RUN = 1 << 0
	};

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
	CMove* m_pMoveCom{};

	CBody_Player* m_pBody{};

private:
	_uint m_iCurState{ ACTION_STATE::END_P };
	_uint m_iPreState{ ACTION_STATE::END_P };

private:
	_uint m_iControlFlag{};
	_uint m_iRequestFlag{};
	_uint m_iReserveRequestFlag{};

private:
	_float3 m_vTargetPos{};

private:
	HRESULT Ready_Components();
	HRESULT Ready_PartObjects();
	HRESULT Bind_ShaderResources();

private:
	void StateRequestProcessing(_float fTimeDelta);
	void Enter_State(ACTION_STATE eNewState);

private:
	void Player_Input(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END