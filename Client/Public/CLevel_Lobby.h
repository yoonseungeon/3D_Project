#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class CStage;

class CLevel_Lobby : public CLevel
{
private:
	explicit CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Lobby() = default;

public:
	HRESULT Initialize();
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Change_Stage(STAGE eNewStage);

private:
	void Update_Stage();

private:
	HRESULT Ready_Lobby();
	CStage* m_Stages[ETOUI(STAGE::STAGE_END)]{};

	STAGE m_eCurStage{};
	STAGE m_ePreStage{};

public:
	static CLevel_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END