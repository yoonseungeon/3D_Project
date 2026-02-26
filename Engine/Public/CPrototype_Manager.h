#pragma once

#include "CBase.h"

/* 1. 원형객체(CGameObject, CComponent)를 보관한다. */
/* 2. 픽된 원형객체를 복제하여 리턴해준다. */

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void	Clear(_uint iLevelIndex);


private:
	size_t		m_iNumLevels	= {};
	using PROTOTYPES = map<const _wstring, class CBase*>;
	// Level별로 구분
	PROTOTYPES* m_pPrototypes	= { nullptr };

private:
	CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);

public:
	static CPrototype_Manager* Create(_uint iNumLevels);
protected:
	virtual void Free() override;
};

NS_END