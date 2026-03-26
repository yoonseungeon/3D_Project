#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CGameObject;

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
	void	Priority_Update(_float fTimeDelta);
	void	Parallel_Update_Parallel(_float fTimeDelta, _int& iTotalJobCnt, atomic<_int>& iFinishedJobCnt, function<void(function<void()> funcJob)> func);
	void	Parallel_Update_Single(_float fTimeDelta);
	void	Update(_float fTimeDelta);
	void	Late_Update(_float fTimeDelta);

private:
	list<CGameObject*> m_GameObjects;

public:
	static CLayer* Create();
protected:
	virtual void Free() override;
};

NS_END