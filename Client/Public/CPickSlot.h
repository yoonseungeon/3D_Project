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
class CGame_Manager;

class CPickSlot final : public CUI_Btn
{
private:
	enum TEX_STATE { NORMAL, HOVER };

public:
	struct CHAR_INFO_DESC
	{
		wstring wstrTexturePrototypeTag;
	};

public:
	struct CPICKSLOT_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		CHAR_NAME eCharName{};
		CHAR_INFO_DESC tCharInfo{};
	};

private:
	CPickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPickSlot(const CPickSlot& Prototype);
	virtual ~CPickSlot() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_IsInactive(_bool bIsInactive) override;

	void Set_Deselect();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	CGame_Manager* m_pGame_Manager{ nullptr };

	TEX_STATE m_eCurTexState{};
	CUI_Image* m_pChar{};

	CHAR_NAME m_eCharName{};

	_bool m_bIsSelected{};

public:
	static CPickSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END