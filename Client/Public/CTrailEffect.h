#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Trail;
class CTexture;
NS_END

NS_BEGIN(Client)

class CTrailEffect : public CGameObject
{
public:
	struct TRAIL_EFFECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		wstring wstrTexturePrototypeTag;
		_uint iTexIndex{};
	};

private:
	struct TRAIL_POINT_INFO
	{
		_float3 vTopWorldPos{};
		_float3 vBottomWorldPos{};

		_float fProgressRatio{};
	};

protected:
	CTrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrailEffect(const CTrailEffect& Prototype);
	virtual ~CTrailEffect() = default;

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
	void Add_TrailPoint(const _float3& vTopWorldPos, const _float3& vBottomWorldPos, const _float& fProgressRatio);

	void End_Trail();

private:
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Trail* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

	wstring m_wstrTexturePrototypeTag;
	_uint m_iTexIndex{};

	_float m_fMaxLifeTime{ 0.5f };
	_float m_fAccLifeTime{ 0.f };

	_float m_fMaxAlpha{ 1.f };
	_float m_fAccAlpha{ 0.f };

	// 중간 삽입 삭제가 없어서 list보단 deque
	deque<TRAIL_POINT_INFO> m_TrailPointsInfo;
	vector<VTXTEX> m_Vertices;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Check_LifeTime(_float fTimeDelta);
	void Make_Vertices(_float fTimeDelta);

public:
	static CTrailEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END