#include "CMyMaterial.h"
#include "CShader.h"

CMyMaterial::CMyMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CMyMaterial::Initialize(const myMaterial* pMyMaterial, const _char* pModelFilePath)
{
	// assimp가 준 텍스처 경로는 부정확할 수 있다.
	// 따라서 내 드라이브와 경로 + assimp에서 뽑는 파일 이름과 확장자를 통해 텍스처를 읽어온다.

	// 드라이브와 경로
	_char szDrive[MAX_PATH]{};
	_char szDir[MAX_PATH]{};

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

	// 머테리얼은 27가지의 타입으로 구분할 수 있다.(e.g. Diffuse, Specular, Ambient, Normal 등)
	for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
	{
		// 머테리얼 타입마다 여러 장의 텍스처가 존재할 수 있다.
		_uint iNumTextures = pMyMaterial->mNumTextures[i];
		
		// 0이 나오는 경우도 있다.(그 타입은 텍스처 없음)
		for (_uint j = 0; j < iNumTextures; ++j)
		{
			// assimp에서 제공하는 스트링
			string strTexturePath = pMyMaterial->Textures[i][j];

			_char szFileName[MAX_PATH]{};
			_char szExt[MAX_PATH]{};

			// data 문자열 배열(주소)
			// 파일 이름과 확장자 추출
			_splitpath_s(strTexturePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			// 합치기
			_char szFullPath[MAX_PATH]{};
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDir);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			// wchar 요구해서 변환
			_tchar szFinalPath[MAX_PATH]{};
			//1. 원본 문자열 인코딩 형식, 2. flag 3. 원본 문자열, 4. 원본 문자열 길이, 5. 저장 받을 문자열, 6. 저장 받을 문자열 길이
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szFinalPath, MAX_PATH);

			HRESULT hr{};

			ID3D11ShaderResourceView* pSRV{ nullptr };

			if (strcmp(szExt, ".dds") == 0)
			{
				hr = CreateDDSTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
			}
			else if (strcmp(szExt, ".tga") == 0)
			{
				hr = E_FAIL;
			}
			else
			{
				hr = CreateWICTextureFromFile(m_pDevice, szFinalPath, nullptr, &pSRV);
			}

			if (FAILED(hr))
				return E_FAIL;

			m_Materials[i].push_back(pSRV);
		}
	}

	return S_OK;
}

HRESULT CMyMaterial::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, aiTextureType eType, _uint iIndex)
{
	if (eType >= TEXTURE_TYPE_MAX || iIndex >= m_Materials[eType].size())
		return E_FAIL;

	return pShader->Bind_SRV(pConstantName, m_Materials[eType][iIndex]);
}

CMyMaterial* CMyMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const myMaterial* pMyMaterial, const _char* pModelFilePath)
{
	CMyMaterial* pInstance = new CMyMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pMyMaterial, pModelFilePath)))
	{
		MSG_BOX("Failed to Created: CMyMaterial");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMyMaterial::Free()
{
	for (auto& SRVs : m_Materials)
	{
		for (auto& pSRV : SRVs)
			Safe_Release(pSRV);
		SRVs.clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
