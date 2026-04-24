#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_EXPGauge final : public CUI_Default
{
public:
	struct CUI_EXPGAUGE_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

protected:
	CUI_EXPGauge(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_EXPGauge(const CUI_EXPGauge& Prototype);
	virtual ~CUI_EXPGauge() = default;

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

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_float m_fFillX{ 1.f };
	_float m_fFillY{ 0.5f };

	_float m_fData{ -1.f };

public:
	static CUI_EXPGauge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END