#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CTrailEffect;
class CNunchaku_AfterImage;

class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJECT_DESC
	{
		// 플레이어 위치(중앙)가 아닌, 뼈에 붙어서 표현되어야 한다.
		// 부착하기 위한 행렬 SocketBone이라고 많이 부름.
		const _float4x4* pSocketBoneMatrix{ nullptr };
	};

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;

public:
	CMyModel* Get_ModelCom() const { return m_pModelCom; }

private:
	CShader* m_pShaderCom{ nullptr };
	CMyModel* m_pModelCom{ nullptr };

	CTrailEffect* m_pTrailEffect{ };

	CNunchaku_AfterImage* m_pNunchaku_AfterImage{};

private:
	const _float4x4* m_pSocketBoneMatrix{ nullptr };

	_float m_fOutLineLength{ 1.f };
	_float4 m_vOutLineColor{ 0.f, 0.f, 0.f, 1.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_OutLineShaderResources();

	HRESULT Ready_Layer_Nunchaku_AfterImage(const _wstring& strLayerTag);

	void Generate_Effect();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END