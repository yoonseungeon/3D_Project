#pragma once

#include "Client_Defines.h"
#include "CLevel.h"

NS_BEGIN(Client)

class CInGame_Manager;
class CSharedUI_Manager;

class CLevel_GamePlay : public CLevel
{
private:
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

private:
	HRESULT Initialize();

public:
	virtual void	Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Update_EnvironmentSound(_float fTimeDelta);

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Shadow();
	HRESULT Ready_Layer_Camera(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Monster(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Map_Lumia(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const _wstring& strLayerTag);

	HRESULT Ready_Layer_UI_Image(const _wstring& strLayerTag);

	HRESULT Ready_Layer_ItemBox(const _wstring& strLayerTag);
	SOUND_KEY Get_ItemBoxOpenSoundKey(const string& strBoxType);
	HRESULT Ready_Layer_ItemBox_Collectible(const _wstring& strLayerTag);
	void Set_ItemBoxSpwanArea(const string& strArea, SPAWN_MAP& eSpawnArea);

	HRESULT Ready_Layer_River(const _wstring& strLayerTag);

	HRESULT Ready_Layer_Chicken(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Wolf(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Dog(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Bear(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Bat(const _wstring& strLayerTag);
	HRESULT Ready_Layer_Boar(const _wstring& strLayerTag);

	HRESULT Ready_Layer_AIFiora(const _wstring& strLayerTag);

	HRESULT Ready_Layer_GameResult(const _wstring& strLayerTag);

private:
	CInGame_Manager*	m_pInGame_Manager{};
	CSharedUI_Manager*	m_pSharedUI_Manager{};

	_float m_fAccDaySoundLoopTime{};
	_float m_fDaySoundLoopGap{};

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END