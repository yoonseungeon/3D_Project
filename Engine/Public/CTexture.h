#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& Prototype);
	virtual ~CTexture() = default;

private:
	HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	HRESULT Initialize(void* pArg);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);
	HRESULT Bind_ShaderResources(class CShader* pShader, const _char* pConstantName);

private:
	_uint								m_iNumTextures = {};
	vector<ID3D11ShaderResourceView*>	m_Textures;

public:
	const _uint Get_TextureCnt() { return m_iNumTextures; }

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END