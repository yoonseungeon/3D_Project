#pragma once

#include "CBase.h"

/* 1. 원형객체(CGameObject, CComponent)를 보관한다. */
/* 2. 픽된 원형객체를 복제하여 리턴해준다. */

#pragma region include_tbb/concurrent_unordered_map.h

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

#include <tbb/concurrent_unordered_map.h>

#ifdef _DEBUG
#ifndef new
#define new DBG_NEW
#endif
#endif

#pragma endregion

NS_BEGIN(Engine)

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager();
	virtual ~CPrototype_Manager() = default;

private:
	HRESULT Initialize(_uint iNumLevels);

public:
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
	CBase*	Clone_Prototype(PROTOTYPE eType, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg);
	void	Clear(_uint iLevelIndex);

#ifdef _DEBUG
	_uint Get_PrototypeCnt(_uint iLevelIndex) { return static_cast<_uint>(m_pPrototypes[iLevelIndex].size()); }
	void Get_PrototypeTags(_uint iLevelIndex, vector<_wstring>& PrototypeTags) {
		for (auto pair : m_pPrototypes[iLevelIndex]) {
			PrototypeTags.push_back(pair.first);
		}
	}
#endif

private:
	size_t		m_iNumLevels	= {};
	using PROTOTYPES = tbb::concurrent_unordered_map<_wstring, CBase*>;
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