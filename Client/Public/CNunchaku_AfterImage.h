#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
class CTexture;
NS_END

NS_BEGIN(Client)

class CNunchaku_AfterImage final : public CGameObject
{
public:
	struct NUNCHAKU_AFTERIMAGE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_uint iTexIdx{};
	};

protected:
	CNunchaku_AfterImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNunchaku_AfterImage(const CNunchaku_AfterImage& Prototype);
	virtual ~CNunchaku_AfterImage() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Position(const _float4x4* vParentMatrix);

private:
	CShader* m_pShaderCom = { nullptr };
	CMyModel* m_pModelCom = { nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float4x4 m_CombinedWorldMatrix{};

	_uint m_iTexIdx{};

	_float4 m_vDebugQuat = _float4(0.662921f, -0.017824f, 0.744414f, 0.077878f);
	_float4 m_vDebugPos = _float4(1.243823f, 0.626138f, 0.846655f, 1.f);
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CNunchaku_AfterImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END