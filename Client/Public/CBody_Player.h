#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CBody_Player final : public CPartObject
{
public:
	struct BODY_PLAYER_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _uint* pCurMoveState{ nullptr };
	};

private:
	enum AniIndex {
		Ani_Idle = 9,
		Ani_Run = 26,
		Ani_ATK_1 = 35, Ani_ATK_2 = 33, Ani_ATK_1P = 34, Ani_ATK_2P = 32,
		Ani_Q1 = 21, Ani_Q2 = 19, Ani_Q3 = 17,
		Ani_E = 14,
		Ani_R1 = 11, Ani_R2 = 12,
		Ani_RestStart = 83, Ani_RestLoop = 85, Ani_RestEnd = 86,
		Ani_Craft = 98,
		Ani_Cook = 99,
		Ani_Collect = 100
	};

	enum AniState {
		NONE,
		START,
		LOOP,
		END
	};

	struct AniLock
	{
		_bool bIsAniLock{};
		_bool bIsRequestUnlock{};
		_bool bIsAniLockExit{};
	};

private:
	CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Player(const CBody_Player& Prototype);
	virtual ~CBody_Player() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;
	const _uint* Get_CurATKType() { return &m_iCurATKType; }

	_bool IsAniLock() { return m_tAniLockInfo.bIsAniLock; }
	void RequestUnlock() { m_tAniLockInfo.bIsRequestUnlock = true; }
	_bool IsAniLockExit() { return m_tAniLockInfo.bIsAniLockExit; }

private:
	CShader*	m_pShaderCom{ nullptr };
	CMyModel*	m_pModelCom{ nullptr };

private:
	const _uint* m_pCurState{ nullptr };
	_uint m_iCurState{};
	_uint m_iCurATKType{};

private:
	AniState m_eAniState{};
	AniLock m_tAniLockInfo{};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void Enter_State(_float fTimeDelta);
	void Execute_State(_float fTimeDelta);

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END