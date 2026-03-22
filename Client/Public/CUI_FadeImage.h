#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_FadeImage final : public CUI_Image
{

public:
	struct CUI_FADEIMAGE_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
	};

protected:
	CUI_FadeImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_FadeImage(const CUI_FadeImage& Prototype);
	virtual ~CUI_FadeImage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_ImageAlpha(_float fAlpha);

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	static CUI_FadeImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END