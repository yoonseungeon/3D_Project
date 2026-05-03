#pragma once

#include "Client_Defines.h"
#include "CInvenOwner.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CItemBox final : public CInvenOwner
{
public:
	struct ITEMBOX_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrModelPrototypeTag;
		_float4 vQuaternion{};
		_float3 vScale{};

		SPAWN_MAP eSpawnArea{};
	};

protected:
	CItemBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemBox(const CItemBox& Prototype);
	virtual ~CItemBox() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	_bool TakeItemToInventory(_uint iSlotIndex);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };

	SPAWN_MAP m_eSpawnArea{};

	_uint m_iMaxSize{ 10 };

private:
	HRESULT Ready_Components(wstring wstrModelPrototypeTag);
	HRESULT Bind_ShaderResources();

	HRESULT Generate_Item();

public:
	static CItemBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END