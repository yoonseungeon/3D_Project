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

class CUI_PickPanel final : public CUI_Default
{
public:
	struct CUI_PICKPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CUI_PickPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PickPanel(const CUI_PickPanel& Prototype);
	virtual ~CUI_PickPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
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
	vector<CPickSlot*> m_PickSlots;

	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};

	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	CCharData_Manager* m_pCharData_Manager{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_PickSlot(const _wstring& strLayerTag);

public:
	static CUI_PickPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END