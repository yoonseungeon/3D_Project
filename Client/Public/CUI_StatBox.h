#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CUI_StatBox final : public CUI_Default
{
public:
	enum STATBOX_TYPE
	{
		ATKPower,
		SkillAmp,
		PenetrationDefense,
		Defense,
		ATKSpeed,
		CoolDown,
		Critical,
		Speed,
	};

public:
	struct CUI_STATBOX_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
		STATBOX_TYPE eStatBoxType{};
	};

private:
	CUI_StatBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_StatBox(const CUI_StatBox& Prototype);
	virtual ~CUI_StatBox() = default;

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

	STATBOX_TYPE m_eStatBoxType{};

	_int m_iData1{ -1 };
	_int m_iData2{ -1 };

	_float m_fData1{ -1.f };

	wstring m_wstrText{ L"" };

	_float m_fImagePosX{};
	_float m_fImagePosY{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CUI_StatBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END