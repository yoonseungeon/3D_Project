#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
class CImage;
NS_END

NS_BEGIN(Client)

class CUI_ItemImage final : public CUI_Default
{
public:
	struct CUI_ITEMIMAGE_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{

	};

private:
	CUI_ItemImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_ItemImage(const CUI_ItemImage& Prototype);
	virtual ~CUI_ItemImage() = default;

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
	void Set_CurItem(_uint iItemIdx);

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };
	CImage* m_pImageCom{ nullptr };

	_float m_fScaleRatioSlotY{};

public:
	static CUI_ItemImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END