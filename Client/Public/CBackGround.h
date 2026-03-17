#pragma once

#include "Client_Defines.h"
#include "CGameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	struct BACKGROUND_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		int a = 10;
	};

protected:
	CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround(const CBackGround& Prototype);
	virtual ~CBackGround() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom	= { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom	= { nullptr };
	CTexture*		m_pTextureCom	= { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
protected:
	virtual void Free();
};

NS_END