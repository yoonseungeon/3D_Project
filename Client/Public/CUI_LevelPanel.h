#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_LevelPanel final : public CUI_Default
{
public:
	struct CUI_LEVELPANEL_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_LevelPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_LevelPanel(const CUI_LevelPanel& Prototype);
	virtual ~CUI_LevelPanel() = default;

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
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };


	_int m_iData{ -1 };

	wstring m_wstrText{ L"" };

	_float m_fImagePosX{};
	_float m_fImagePosY{};

	_float m_fDigit2{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_LevelPanel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END