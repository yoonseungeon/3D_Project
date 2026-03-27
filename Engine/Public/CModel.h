#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class CMesh;

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Render();

private:
	HRESULT Ready_Meshes();

private:
	const aiScene*			m_pAIScene{ nullptr };
	Importer				m_Importer{};

	size_t					m_iNumMeshes{};
	vector<CMesh*>			m_Meshes;				// Model하나 당 여러 개의 Mesh를 갖는다.

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END