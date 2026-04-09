#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class CTransform;

class ENGINE_DLL CMove final : public CComponent
{
public:
	struct MOVE_DESC
	{
		CTransform* pTransform{};
		_float fSpeed{ 1.f };
	};

private:
	enum MOVE_FLAG
	{
		FLAG_MOVE_TO_POS = 1 << 0,
	};

private:
	CMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMove(const CMove& Prototype);
	virtual ~CMove() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	_bool Update_Move_To_Pos(_float fTimeDelta);

public:
	void Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; };

public:
	void Move_To_Pos(_float3 vPos);
	void Stop_Move_To_Pos();

private:
	CTransform* m_pTransform{};

	_uint m_iCurMoveFlag{};

	_float3 m_vMovePos{};
	_float m_fSpeed{ 1.f };



public:
	static CMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END