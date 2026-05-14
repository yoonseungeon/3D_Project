#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"
#include "Command.h"

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
	virtual HRESULT Render_Shadow() override;
	virtual HRESULT Render_OutLine() override;

public:
	const _float4x4* Get_BoneMatrixPtr(const _char* pBoneName) const;

	CMyModel* Get_ModelCom() const { return m_pModelCom; }

private:
	CShader*	m_pShaderCom{ nullptr };
	CMyModel*	m_pModelCom{ nullptr };

	_float m_fOutLineLength{ 1.f };
	_float4 m_vOutLineColor{ 0.f, 0.f, 0.f, 1.f };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Bind_OutLineShaderResources();

public:
	static CBody_Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END