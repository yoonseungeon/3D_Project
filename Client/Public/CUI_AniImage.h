#pragma once

#include "Client_Defines.h"
#include "CUI_Image.h"

NS_BEGIN(Client)

class CUI_AniImage final : public CUI_Image
{

public:
	struct CUI_ANIIMAGE_DESC : public CUI_Image::CUI_IMAGE_DESC
	{
		_float fFrameDelay{};
	};

protected:
	CUI_AniImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_AniImage(const CUI_AniImage& Prototype);
	virtual ~CUI_AniImage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint	m_iMaxTextureCnt{};
	_uint	m_iCurTextureIdx{};
	_float	m_fFrameDelay{};
	_float	m_fAccTime{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Animation(_float fTimeDelta);

public:
	static CUI_AniImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END