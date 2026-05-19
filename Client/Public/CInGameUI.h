#pragma once

#include "Client_Defines.h"
#include "CPartObject.h"

NS_BEGIN(Client)

class CInGameUI abstract : public CPartObject
{
public:
	struct INGAMEUI_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_float fHeight{};
	};

protected:
	CInGameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInGameUI(const CInGameUI& Prototype);
	virtual ~CInGameUI() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4x4 m_TransformMatrices[ETOUI(D3DTS::END_DECO)]{};
	_float m_fHeight{};

	_bool m_IsOn{ true };

protected:
	HRESULT Bind_ShaderResource(CShader* pShader, const _char* pConstantName, D3DTS eType);
	void Update_Pos();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END