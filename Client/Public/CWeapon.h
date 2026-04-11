#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CWeapon final : public CPartObject
{
private:
	enum WPAniIndex {
		IDLE_WP = 12,
		RUN_WP = 4,
		ATK_1_WP = 0, ATK_2_WP = 2,
		ATK_1P_WP = 1, ATK_2P_WP = 3,
		Q1_WP = 7, Q2_WP = 9, Q3_WP = 10,
		E_WP,
		R_WP,
	};

public:
	struct WEAPON_DESC : public CPartObject::PARTOBJECT_DESC
	{
		// 플레이어 위치(중앙)가 아닌, 뼈에 붙어서 표현되어야 한다.
		// 부착하기 위한 행렬 SocketBone이라고 많이 부름.
		const _float4x4* pSocketBoneMatrix{ nullptr };
		const _uint* pCurMoveState{ nullptr };
		const _uint* pCurATKType{ nullptr };
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

private:
	CShader* m_pShaderCom{ nullptr };
	CMyModel* m_pModelCom{ nullptr };

private:
	const _float4x4* m_pSocketBoneMatrix{ nullptr };

	const _uint* m_pCurState{ nullptr };
	_uint m_iCurState{};
	const _uint* m_pCurATKType{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	void Enter_State(_float fTimeDelta);
	void Execute_State(_float fTimeDelta);

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END