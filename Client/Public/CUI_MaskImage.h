#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

NS_BEGIN(Client)

class CUI_MaskImage final : public CUI_Image
{

public:
	struct CUI_MASKIMAGE_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
		wstring wstrMaskPrototypeTag;
	};

protected:
	CUI_MaskImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MaskImage(const CUI_MaskImage& Prototype);
	virtual ~CUI_MaskImage() = default;

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
	CTexture* m_pMaskCom{ nullptr };
	wstring m_wstrMaskPrototypeTag;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_MaskImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END