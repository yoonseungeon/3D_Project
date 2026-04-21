#pragma once

#include "Client_Defines.h"
#include "CUI_Default.h"

NS_BEGIN(Client)

class CInGameCharLevel_Empty : public CUI_Default
{
private:
	CInGameCharLevel_Empty(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInGameCharLevel_Empty(const CInGameCharLevel_Empty& Prototype);
	virtual ~CInGameCharLevel_Empty() = default;

private:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg);

public:
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Parallel_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

public:
	static CInGameCharLevel_Empty* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free() override;
};

NS_END