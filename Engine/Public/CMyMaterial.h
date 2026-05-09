#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CShader;

class CMyMaterial final : public CBase
{
private:
	CMyMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMyMaterial() = default;

private:
	HRESULT Initialize(const myMaterial* pMyMaterial, const _char* pModelFilePath);

public:
	HRESULT Bind_ShaderResource(CShader* pShader, const _char* pConstantName, MyTextureType eType, _uint iIndex);
	size_t Get_MaterialCount(MyTextureType eType) { return m_Materials[eType].size(); }

private:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };

	vector<ID3D11ShaderResourceView*> m_Materials[TEXTURE_TYPE_MAX];

public:
	static CMyMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const myMaterial* pMyMaterial, const _char* pModelFilePath);
protected:
	virtual void Free() override;
};

NS_END