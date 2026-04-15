#pragma once

#include "Client_Defines.h"
#include "CUIObject.h"

NS_BEGIN(Client)

class CUI_Default abstract : public CUIObject
{
public:
	enum BLENDSTATE { DEFAULT, ALPHATEST, ALPHABLEND, ALPHABLEDN_GAUGE, MASK, COLOR_ALPHABLEND };

public:
	struct CUI_DEFAULT_DESC : public CUIObject::UIOBJECT_DESC
	{
		LEVEL eTexPrototypeLV{};
		wstring wstrTexturePrototypeTag;
		BLENDSTATE eBlendState{};

		_float fImageAlpha{ 1.f };
		_float3 vColor{};
	};

protected:
	CUI_Default(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Default(const CUI_Default& Prototype);
	virtual ~CUI_Default() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Add_Alpha(_float fAlpha);
	void Set_TexIdx(_uint iTexIdx) { m_iTexIdx = iTexIdx; }

protected:
	LEVEL m_eTexPrototypeLV{};
	wstring m_wstrTexturePrototypeTag;
	BLENDSTATE m_eBlendState{};

	_float3 m_vColor{};
	_float m_fImageAlpha{ 1.f };

	_uint m_iTexIdx{};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END