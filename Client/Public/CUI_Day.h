#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_Day : public CUI_Default
{
public:
	struct CUI_DAY_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_Day(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Day(const CUI_Day& Prototype);
	virtual ~CUI_Day() = default;

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

	wstring m_wstrDay{};

	_float m_fCenterX{};
	_float m_fCenterY{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	HRESULT Ready_Layer_Timer(const _wstring& strLayerTag);

	HRESULT Render_Text();

public:
	static CUI_Day* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END