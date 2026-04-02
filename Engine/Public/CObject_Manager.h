#pragma once

#include "CBase.h"

/* 1.실제 동작을 할 객체(사본객체)를 보관한다. */
/* 1-1. 레벨별로 구분짓자. */
/* 1-2. 레이어별로 그룹짓는다. */

/* 2. 보관하고 있는 객체들의 갱신(Update, Tick) 을 수행해준다. */

NS_BEGIN(Engine)

class CGameInstance;
class CLayer;
class CGameObject;

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

private:
	HRESULT Initialize(_uint iNumLevels);

public:
	HRESULT Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg, CGameObject** ppOut);
	void Priority_Update(_float fTimeDelta);
	void Parallel_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

	_bool Is_Parallel_Update_Finished();
	void Set_Parallel_Update_Mode(PARALLEL_UPDATE_MODE eParallelMode);

#ifdef _DEBUG
	size_t Get_NumLevels() const { return m_iNumLevels; }
	const map<const _wstring, CLayer*>* Get_MapLevelLayers() const { return m_pLayers; }
#endif

private:
	size_t				m_iNumLevels = {};
	using LAYERS = map<const _wstring, CLayer*>;
	// Level별로 구분
	LAYERS* m_pLayers = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };

private:
	CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);

private:
	_int			m_iTotalJobCnt{ 0 };
	atomic<_int>	m_iFinishedJobCnt{ 0 };
	PARALLEL_UPDATE_MODE m_eParallelMode{};

public:
	static CObject_Manager* Create(_uint iNumLevels);
protected:
	virtual void Free() override;
};

NS_END