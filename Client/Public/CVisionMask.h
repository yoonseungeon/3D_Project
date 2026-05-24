#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CVisionMask final : public CPartObject
{
public:
	struct VISIONMASK_DESC : public CPartObject::PARTOBJECT_DESC
	{
	};

protected:
	CVisionMask(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVisionMask(const CVisionMask& Prototype);
	virtual ~CVisionMask() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_VisionMask() override;

public:
	void Set_Range(_float fRange);

private:
	CShader*		m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVisionMask* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END