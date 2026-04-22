#pragma once

#include "CGameObject.h"

NS_BEGIN(Engine)

class CPartObject;

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
protected:
	CContainerObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CContainerObject(const CContainerObject& Prototype);
	virtual ~CContainerObject() = default;

protected:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Parallel_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CComponent* Find_PartObjectComponent(const _wstring& strPartTag, const _wstring& strComponentTag);

protected:
	map<const _wstring, CPartObject*> m_PartObjects;

protected:
	// GameObject_Manager에 넣지 않음.
	HRESULT Add_PartObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, const _wstring& strPartTag, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
protected:
	virtual void Free();
};

NS_END