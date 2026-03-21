#pragma once

#include "Client_Defines.h"
#include "CUIObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Image final : public CUIObject
{
public:
	struct CUI_IMAGE_DESC : public CUIObject::UIOBJECT_DESC
	{
		LEVEL eTexPrototypeLV;
		wstring wstrTexturePrototypeTag;
	};

protected:
	CUI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Image(const CUI_Image& Prototype);
	virtual ~CUI_Image() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom	= { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom	= { nullptr };
	CTexture* m_pTextureCom	= { nullptr };

private:
	HRESULT Ready_Components(LEVEL eTexPrototypeLV, wstring& pTexturePrototypeTag);
	HRESULT Bind_ShaderResources();

public:
	static CUI_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END