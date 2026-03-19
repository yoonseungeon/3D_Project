#pragma once

#include "CGameObject.h"

NS_BEGIN(Engine)

class CShade;

class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
	struct UIOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float		fScaleRatioX, fScaleRatioY;
		_float		fPosRatioX, fPosRatioY;
	};

protected:
	CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIObject(const CUIObject& Prototype);
	virtual ~CUIObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float4x4				m_TransformMatrices[ETOUI(D3DTS::END)] = {};

protected:
	HRESULT Bind_ShaderResource(CShader* pShader, const _char* pConstantName, D3DTS eType);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free() override;
};

NS_END