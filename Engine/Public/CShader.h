#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	// 셰이더 파일을 받아와야 함.
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg);

public:
	// inputLayout을 세팅하는 함수
	HRESULT Begin(_uint iPassIndex);

private:
	ID3DX11Effect* m_pEffect = { nullptr };
	_uint			m_iNumPasses = {};

	vector<ID3D11InputLayout*> m_InputLayouts;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END