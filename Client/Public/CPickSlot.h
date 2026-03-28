#pragma once

#include "Client_Defines.h"
#include "CUI_Btn.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Image;

class CPickSlot final : public CUI_Btn
{
	enum TEX_STATE { NORMAL, OVER };

public:
	struct CHAR_INFO
	{
		wstring wstrTexturePrototypeTag;
	};

public:
	struct CPICKSLOT_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		CHAR_INFO tCharInfo{};
	};


protected:
	CPickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPickSlot(const CPickSlot& Prototype);
	virtual ~CPickSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	TEX_STATE m_eCurTexState{};
	CUI_Image* m_pChar{};

public:
	static CPickSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END