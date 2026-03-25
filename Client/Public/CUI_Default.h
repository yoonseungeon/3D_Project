#pragma once

#include "Client_Defines.h"
#include "CUIObject.h"

NS_BEGIN(Client)

class CUI_Default abstract : public CUIObject
{
public:
	enum BLENDSTATE { BS_DEFAULT, BS_ALPHATEST, BS_ALPHABLEND };

public:
	struct CUI_DEFAULT : public CUIObject::UIOBJECT_DESC
	{
		LEVEL eTexPrototypeLV{};
		wstring wstrTexturePrototypeTag;
		BLENDSTATE eBlendState{};

		_float fImageAlpha{ 1.f };
	};

protected:
	CUI_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Default(const CUI_Default& Prototype);
	virtual ~CUI_Default() = default;

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
	LEVEL m_eTexPrototypeLV{};
	wstring m_wstrTexturePrototypeTag;
	BLENDSTATE m_eBlendState{};

	_float m_fImageAlpha{ 1.f };

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END