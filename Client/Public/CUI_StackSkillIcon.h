#pragma once

#include "Client_Defines.h"
#include "CUI_SkillIcon.h"

#include "Command.h"

NS_BEGIN(Client)

class CUI_StackSkillIcon final : public CUI_SkillIcon
{

public:
	struct CUI_STACKSKILLICON_DESC : public CUI_SkillIcon::CUI_SKILLICON_DESC
	{
	};

protected:
	CUI_StackSkillIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StackSkillIcon(const CUI_StackSkillIcon& Prototype);
	virtual ~CUI_StackSkillIcon() = default;

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

public:
	static CUI_StackSkillIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END