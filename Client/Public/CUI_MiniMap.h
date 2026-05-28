#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CInGame_Manager;

class CUI_MiniMap : public CUI_Default
{
public:
	struct CUI_MINIMAP_DESC : public CUI_Default::CUI_DEFAULT_DESC
	{
	};

private:
	CUI_MiniMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MiniMap(const CUI_MiniMap& Prototype);
	virtual ~CUI_MiniMap() = default;

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

	_float m_fPosRatioX{};
	_float m_fPosRatioY{};
	

	_float2 m_vCenterUV = { 0.525f, 0.505f };
	_float2 m_vMinimapUVSize = { 0.5f, 0.5f };

	CInGame_Manager* m_pInGame_Manager{};

	_float m_fWorldMinX{};
	_float m_fWorldMaxX{};					
	_float m_fWorldMinZ{};
	_float m_fWorldMaxZ{};
	_float2 m_vWorldCenter{};
	_float2 m_vWorldHalf{};

	_float m_fMiniMapRotRadian{};

	_float m_fWorldToMiniMapScale = 1.f;

	_float2 m_vCenter{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	_float2 Rotate_2D(const _float2& v, _float fAngle);

	void Init_MiniMap();

	void Update_MiniMapPos();

	HRESULT Ready_Layer_PlayerIcon(const _wstring& strLayerTag);

public:
	static CUI_MiniMap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END