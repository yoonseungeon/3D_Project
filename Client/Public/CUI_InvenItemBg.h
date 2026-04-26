#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_InvenItemBg final : public CUI_Default
{
public:
	struct CUI_INVENITEMBG_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_InvenItemBg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InvenItemBg(const CUI_InvenItemBg& Prototype);
	virtual ~CUI_InvenItemBg() = default;

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
	void Set_ItemCnt(_uint iItemCnt);

protected:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_uint m_iItemCnt{};
	wstring m_wstrItemCnt{L""};

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	_float m_fImagePosX{};
	_float m_fImagePosY{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_InvenItemBg* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END