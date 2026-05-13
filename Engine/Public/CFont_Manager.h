#pragma once

#include "CBase.h"

NS_BEGIN(Engine)

class CCustomFont;

class CFont_Manager final : public CBase
{
private:
	CFont_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition,
		_fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), const _float2& vScale = _float2(1.f, 1.f), _float fRotation = 0.f, const _float2& vOrigin = _float2(0.f, 0.f));

	_float2 Cal_Length(const _wstring& strFontTag, const _wstring& Text);

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	map<const _wstring, class CCustomFont*>		m_Fonts;

private:
	CCustomFont* Find_Font(const _wstring& strFontTag);

public:
	static CFont_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END