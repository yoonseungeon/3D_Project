#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

#include "Command.h"

NS_BEGIN(Client)

class CUI_SkillIcon abstract : public CUI_Image
{

public:
	struct CUI_SKILLICON_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
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

protected:
	virtual HRESULT RenderText();

public:
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END