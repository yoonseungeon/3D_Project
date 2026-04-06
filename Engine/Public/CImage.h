#pragma once

#include "CComponent.h"

NS_BEGIN(Engine)

class ENGINE_DLL CImage final : public CComponent
{
private:
	struct IMAGE_ALPHA
	{
		size_t iWidth{};
		size_t iHeight{};
		vector<_ubyte> vecAlphaImage;
	};

private:
	CImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CImage(const CImage& Prototype);
	virtual ~CImage() = default;

private:
	HRESULT Initialize_Prototype(const _tchar* pImageFilePath, _uint iImages);
	HRESULT Initialize(void* pArg);

private:
	_uint m_iNumImages = {};
	vector<shared_ptr<IMAGE_ALPHA>> m_vecAlphaImages;

public:
	static CImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pImageFilePath, _uint iImages = 1);
	virtual CComponent* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END