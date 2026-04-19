#pragma once

#include "Client_Defines.h"
#include "CCamera.h"

NS_BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	struct CAMERA_FREE_DESC final : public CCamera::CAMERA_DESC
	{
		_float fMouseSensor{};
	};

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& Prototype);
	virtual ~CCamera_Free() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	// 마우스 감도
	_float	m_fMouseSensor{};

	_bool m_bLock{};

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END