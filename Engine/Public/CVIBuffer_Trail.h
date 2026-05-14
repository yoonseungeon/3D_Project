#pragma once

#include "CVIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	HRESULT Trail_Map_Discard(const VTXTEX* pNewVertices, _uint iVertexCount);

private:
	_uint m_iValidIndices{};

public:
	static	CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;

};

NS_END