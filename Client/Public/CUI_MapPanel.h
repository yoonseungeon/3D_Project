#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CPickSlot;
class CCharData_Manager;

class CUI_MapPanel final : public CUI_Default
{
public:
	struct CUI_MAPPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CUI_MapPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MapPanel(const CUI_MapPanel& Prototype);
	virtual ~CUI_MapPanel() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_IsInactive(_bool bIsInactive) override;

protected:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	CCharData_Manager* m_pCharData_Manager{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_MapPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END