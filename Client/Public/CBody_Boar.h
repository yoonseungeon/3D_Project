#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"
#include "Command.h"

NS_BEGIN(Engine)
class CShader;
class CMyModel;
NS_END

NS_BEGIN(Client)

class CBody_Boar final : public CPartObject
{
public:
	struct BODY_BOAR_DESC : public CPartObject::PARTOBJECT_DESC
	{
	};

	enum BOAR_ANI { APPEAR, ATK1, ATK2, DANCE, DEATH, DYING, ENDBATTLE, RUN, SKILL_ASSAULT, SKILL_READY, WAIT, BEWARE_END, BEWARE_LOOP, BEWARE_START };

private:
	CBody_Boar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBody_Boar(const CBody_Boar& Prototype);
	virtual ~CBody_Boar() = default;

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
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	CMyModel* Get_ModelCom() const { return m_pModelCom; }

private:
	CShader* m_pShaderCom{ nullptr };
	CMyModel* m_pModelCom{ nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBody_Boar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END