#include "CBase.h"

CBase::CBase()
{
}

CBase::CBase(const CBase& Prototype)
	: m_iRefCnt{ 1 }

{
}

_uint CBase::AddRef()
{
	return m_iRefCnt.fetch_add(1, std::memory_order_relaxed) + 1;
}

_uint CBase::Release()
{
	_uint iRefCnt = m_iRefCnt.fetch_sub(1, std::memory_order_acq_rel) - 1;

	if (iRefCnt == 0)
	{
		Free();
		delete this;
	}

	return iRefCnt;
}

void CBase::Free()
{
}
