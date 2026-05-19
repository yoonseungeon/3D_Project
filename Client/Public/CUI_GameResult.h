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
class CGameResultTextBox;

class CUI_GameResult final : public CUI_Default
{
public:
	struct CUI_GAMERESULT_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	struct TargetsizeInfo
	{
		_float fStartSize{};
		_float fTargetSize{};
	};

private:
	enum CIRCLE {INNER1, INNER2, INNER3, INNER4};

private:
	CUI_GameResult(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_GameResult(const CUI_GameResult& Prototype);
	virtual ~CUI_GameResult() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void GameResultStart();
	void GameResultReset();

	_bool Get_ResultEnd() { return m_bResultEnd; }

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };
	
private:
	CUI_Image* m_pBg{};

	vector<CUI_Image*> m_CUI_Circles;
	vector<TargetsizeInfo> m_CircleSizeInfos;

	CGameResultTextBox* m_pTextBox{};

	_float m_fMaxTime{};
	_float m_fAccTime{};
	_bool m_bIsPlay{};

	_bool m_bResultEnd{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void GameResultInit();

	HRESULT Ready_Layer_GameResult(const _wstring& strLayerTag);

	void Update_GameResult(_float fTimeDelta);

	void Update_CircleLayer(_float fRatio);
	void Update_CircleRot(_float fTimeDelta, _float fRatio);
	void Update_Bg(_float fRatio);

public:
	static CUI_GameResult* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END