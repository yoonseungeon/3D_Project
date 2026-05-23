#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Image;

class CUI_DayIcon : public CUI_Default
{
public:
	struct CUI_DAYICON_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_DayIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_DayIcon(const CUI_DayIcon& Prototype);
	virtual ~CUI_DayIcon() = default;

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
	void Set_Day(_uint iDay);

protected:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fCenterX{};
	_float m_fCenterY{};

	_float m_fPosRatioX{};
	_float m_fPosRatioY{};

	CUI_Image* m_pDayIcon{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_Timer(const _wstring& strLayerTag);

public:
	static CUI_DayIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END