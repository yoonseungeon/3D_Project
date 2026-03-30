#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class CLevel_Ending : public CLevel
{
private:
	explicit CLevel_Ending(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Ending() = default;

private:
	HRESULT Initialize();

public:
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CLevel_Ending* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END