#pragma once

// 모든 class는 CBase를 상속받기 때문에 Client, Engine 둘 다 Engine 쪽의 Defines 사용 가능
#include "Engine_Defines.h"

NS_BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	CBase(const CBase& Prototype);
	virtual ~CBase() = default;

public:
	_uint AddRef();
	_uint Release();

protected:
	atomic_uint m_iRefCnt = { 1 };

protected:
	virtual void Free() = 0;
};

NS_END