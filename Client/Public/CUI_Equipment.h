#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CItem_Manager;
class CUI_EquipmentSlot;

class CUI_Equipment : public CUI_Default
{
public:
	struct CUI_EQUIPMENT_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_Equipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Equipment(const CUI_Equipment& Prototype);
	virtual ~CUI_Equipment() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<EQUIPMENT_SLOT> m_UIEquipments;
	_uint m_iEquipmentChangeFlag{};

	CItem_Manager* m_pCItem_Manager{};

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

private:
	vector<CUI_EquipmentSlot*> m_Slots;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Initialize_Equipment();
	HRESULT Ready_Layer_UI_EquipmentSlot(const _wstring& strLayerTag);
	HRESULT Slot_Creator(const _wstring& strLayerTag, void* pSlotDesc);

public:
	static CUI_Equipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END