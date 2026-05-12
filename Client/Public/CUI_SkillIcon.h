#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

#include "Command.h"

NS_BEGIN(Client)

class CInGame_Manager;

class CUI_SkillIcon abstract : public CUI_Image
{

public:
	struct CUI_SKILLICON_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
		SKILL_SLOT eSkillSlot{};

		_uint iMaxSkillLevel{};

		_float fRatioYNoExtend{};
	};

protected:
	CUI_SkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillIcon(const CUI_SkillIcon& Prototype);
	virtual ~CUI_SkillIcon() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	RECT			m_rcTextRange{};
	_float			m_fHalfSizeX{};
	_float			m_fHalfSizeY{};

	_bool m_bTextRender{};
	wstring m_wstCoolText;

	SKILL_SLOT m_eSkillSlot{};

	CInGame_Manager* m_pInGameManager{};

	_int m_iMaxSkillLevel{};
	_int m_iCurSkillLevel{};

	_float m_fRatioYNoExtend{};

protected:
	virtual HRESULT RenderText();

	virtual HRESULT Ready_Layer_SkillLevelUpBtn(const _wstring& strLayerTag) { return S_OK; }
	virtual HRESULT Ready_Layer_SkillCoolDisplay(const _wstring& strLayerTag) { return S_OK; }

public:
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END