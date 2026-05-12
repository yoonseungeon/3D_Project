#pragma once

#include "Client_Defines.h"
#include "CUI_Btn.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CInGame_Manager;

class CSkillLevelUpBtn final : public CUI_Btn
{
private:
	enum TEX_STATE { NORMAL, HOVER, DISABLE };

public:
	struct SKILL_LEVELUP_BTN_DESC : public CUI_Btn::CUI_BTN_DESC
	{
		SKILL_SLOT eSkillSlot{};
	};

protected:
	CSkillLevelUpBtn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillLevelUpBtn(const CSkillLevelUpBtn& Prototype);
	virtual ~CSkillLevelUpBtn() = default;

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
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	virtual void BtnClick() override;
	void Execute_Btn(_float fTimeDelta);

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	TEX_STATE m_eCurTexState{};

	CInGame_Manager* m_pInGameManager{};

	_uint m_iCurSkillPoint{};
	_bool m_bCanLevelUp{};

	SKILL_SLOT m_eSkillSlot{};

public:
	static CSkillLevelUpBtn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END