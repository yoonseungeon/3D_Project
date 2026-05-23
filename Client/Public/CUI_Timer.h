#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Day;
class CUI_DayIcon;

class CUI_Timer : public CUI_Default
{
public:
	struct CUI_TIMTER_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_Timer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Timer(const CUI_Timer& Prototype);
	virtual ~CUI_Timer() = default;

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
	void Set_Timer(_uint iDay, _float fTime);

protected:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fLeftEndU{};
	_float m_fRightStartU{};

	_float m_fRenderLeftEnd{};
	_float m_fRenderRightStart{};

	_float m_fCenterX{};
	_float m_fCenterY{};

	wstring m_wstrTime[5];

	CUI_Day* m_pDay{};
	CUI_DayIcon* m_pDayIcon{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_Timer(const _wstring& strLayerTag);

	HRESULT Render_Text();

public:
	static CUI_Timer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END