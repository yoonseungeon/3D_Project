#pragma once

#include "Client_Defines.h"
#include "CInGameUI.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CInGameHPBar final : public CInGameUI
{
public:
	enum UNIT_TYPE { MONSTER, CHAR };

public:
	struct INGAMEHPBAR_DESC : public CInGameUI::INGAMEUI_DESC
	{
		_int* pMaxHp{};
		_int* pCurHp{};

		_int* pMaxMp{};
		_int* pCurMp{};

		_float3 vHPColor{};
		_float3 vMPColor{};

		_bool bNoMp{};

		UNIT_TYPE eUnitType{};
	};

protected:
	CInGameHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInGameHPBar(const CInGameHPBar& Prototype);
	virtual ~CInGameHPBar() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	_int* m_pMaxHp{};
	_int* m_pCurHp{};

	_float m_fCurHp;
	_float m_fMaxHp;

	_int* m_pMaxMp{};
	_int* m_pCurMp{};

	_float m_fMpRatio{};

	_float3 m_vHpColor{};
	_float3 m_vMpColor{};

	_bool m_bNoMp{};

	UNIT_TYPE m_eUnitType{};
	_bool m_bDrawSmallLine{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInGameHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END