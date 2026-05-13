#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CSkillCoolDisplay final : public CUI_Default
{
public:
	struct SKILLCOOLDISPLAY_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CSkillCoolDisplay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillCoolDisplay(const CSkillCoolDisplay& Prototype);
	virtual ~CSkillCoolDisplay() = default;

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
	void Set_CoolTime(const _float fMaxSkillCool, const _float fCurSkillCool);
	void Set_CoolDisplayColor(_float3 vSkillCoolColor) { m_vSkillCoolColor = vSkillCoolColor; }
	void Set_CoolText(const wstring& wstCoolText, _bool bTextRender) { m_wstCoolText = wstCoolText; m_bTextRender = bTextRender; }

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fMaxSkillCool{};
	_float m_fCurSkillCool{};

	_float3 m_vSkillCoolColor{};

	RECT m_rcTextRange{};
	_float m_fHalfSizeX{};
	_float m_fHalfSizeY{};

	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};

	_bool m_bTextRender{};

	wstring m_wstCoolText;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT RenderText();

public:
	static CSkillCoolDisplay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END