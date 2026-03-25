#pragma once

#include "Client_Defines.h"
#include "CBase.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CStage abstract : public CBase
{
protected:
	CStage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual ~CStage() = default;

public:
	virtual void Enable_Stage() = 0;
	virtual void Disable_Stage() = 0;

protected:
	virtual HRESULT Initialize(function<void(STAGE)> funcBtnCallBack) = 0;

protected:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };
	CGameInstance*			m_pGameInstance{ nullptr };

	vector<CGameObject*> m_vecGameObjects;

	function<void(STAGE)> m_funcBtnCallBack{};


protected:
	virtual void Free() override;
};

NS_END