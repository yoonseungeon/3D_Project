#pragma once

#include "Client_Defines.h"
#include "CUIObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Image : public CUIObject
{
public:
	enum BLENDSTATE { BS_DEFAULT, BS_ALPHATEST, BS_ALPHABLEND };

public:
	struct CUI_IMAGE_DESC : public CUIObject::UIOBJECT_DESC
	{
		LEVEL eTexPrototypeLV{};
		wstring wstrTexturePrototypeTag;
		BLENDSTATE eBlendState{};

		_float fImageAlpha{ 1.f };
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

public:
	void Add_Alpha(_float fAlpha);

protected:
	CShader*		m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture*		m_pTextureCom{ nullptr };

	LEVEL m_eTexPrototypeLV{};
	wstring m_wstrTexturePrototypeTag;
	BLENDSTATE m_eBlendState{};

	_float m_fImageAlpha{1.f};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_Image* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END