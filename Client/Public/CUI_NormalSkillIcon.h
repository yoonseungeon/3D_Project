#pragma once

#include "Client_Defines.h"
#include "CUI_SkillIcon.h"

#include "Command.h"

NS_BEGIN(Client)

class CUI_NormalSkillIcon final : public CUI_SkillIcon
{

public:
	struct CUI_NORMALSKILLICON_DESC : public CUI_SkillIcon::CUI_SKILLICON_DESC
	{
	};

protected:
	CUI_NormalSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_NormalSkillIcon(const CUI_NormalSkillIcon& Prototype);
	virtual ~CUI_NormalSkillIcon() = default;

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

	virtual HRESULT Ready_Layer_SkillLevelUpBtn(const _wstring& strLayerTag) override;

private:
	_float m_fScaleRatioX{};
	_float m_fScaleRatioY{};
	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

public:
	static CUI_NormalSkillIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END