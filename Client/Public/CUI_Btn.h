#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Btn abstract : public CUI_Default
{
protected:
	enum BTN_STATE { NORMAL, HOVER, PRESSED, CLICKED };

public:
	struct CUI_BTN_DESC : public CUI_Default::CUI_DEFAULT
	{
		function<void()> funcCallBack{ nullptr };
	};

protected:
	CUI_Btn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Btn(const CUI_Btn& Prototype);
	virtual ~CUI_Btn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Update_BtnState();
	virtual void BtnClick();

protected:
	RECT m_rcBtnRange{};
	BTN_STATE m_eCurBtnState{};
	_bool m_bPressedInBtn{};

	function<void()> m_funcCallBack{nullptr};

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END