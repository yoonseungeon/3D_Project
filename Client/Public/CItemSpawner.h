#pragma once

#include "Client_Defines.h"
#include "CInvenOwner.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CItemSpawner abstract : public CInvenOwner
{
public:
	struct ITEMSPAWNER_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrModelPrototypeTag;
		_float4 vQuaternion{};
		_float3 vScale{};

		SPAWN_MAP eSpawnArea{};
	};

protected:
	CItemSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemSpawner(const CItemSpawner& Prototype);
	virtual ~CItemSpawner() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool TakeItemToInventory(_uint iSlotIndex);

	_bool XM_CALLCONV IsInOpenRange(_fvector vPos, _float fWorldDistance);

protected:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };

	SPAWN_MAP m_eSpawnArea{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END