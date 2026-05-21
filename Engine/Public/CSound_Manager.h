#pragma once

#include "CBase.h"

#pragma region include_tbb/concurrent_unordered_map.h

#ifdef _DEBUG
#ifdef new
#undef new
#endif
#endif

#include <tbb/concurrent_unordered_map.h>

#ifdef _DEBUG
#ifndef new
#define new DBG_NEW
#endif
#endif

#pragma endregion

NS_BEGIN(FMOD)
class System;
class Sound;
class Channel;
NS_END

NS_BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSound_Manager() = default;

private:
	HRESULT Initialize();

public:
	void Update_Sound();

	HRESULT PlaySound_Loop(_uint iKey, _uint iChannelID, _float fVolume);
	HRESULT PlaySound_OnceFixed(_uint iKey, _uint iChannelID, _float fVolume);
	HRESULT PlaySound_Once(_uint iKey, _float fVolume);
	HRESULT StopSoundChannel(_uint iChannelID);
	HRESULT StopAll();
	HRESULT Set_ChannelVolume(_uint iChannelID, _float fVolume);
	_bool IsPlaying(_uint iChannelID);

public:
	HRESULT Add_Sound(const string& strPath, _uint iKey);
	void Clear_Sound();

private:
	ID3D11Device*			m_pDevice{ nullptr };
	ID3D11DeviceContext*	m_pContext{ nullptr };

private:
	static constexpr _uint m_iMaxSound = { 64 };

	FMOD::System* m_pSystem{};
	FMOD::Channel* m_pLoopChannels[m_iMaxSound]{};
	vector<FMOD::Channel*> m_pOnceChannels;

	tbb::concurrent_unordered_map<_uint, FMOD::Sound*> m_Sounds;

public:
	static CSound_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
protected:
	virtual void Free() override;
};

NS_END