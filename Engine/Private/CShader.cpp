#include "CShader.h"

CShader::CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CShader::CShader(const CShader& Prototype)
    : CComponent{ Prototype }
    , m_pEffect{ Prototype.m_pEffect }
    , m_iNumPasses{ Prototype.m_iNumPasses }
    , m_InputLayouts{ Prototype.m_InputLayouts }
{
    for (auto& pInputLayout : m_InputLayouts)
        Safe_AddRef(pInputLayout);

    Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    _uint iHlslFlag = {};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    // 이 함수로 셰이더 파일을 빌드할 것임.(ID3DX11Effect도 만듬)
    if (FAILED(D3DX11CompileEffectFromFile(
        pShaderFilePath,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        iHlslFlag,
        0,
        m_pDevice,
        &m_pEffect,
        nullptr
    )))
        return E_FAIL;

    // pass는 Technique한테 감싸져 있음. Technique한테 먼저 접근해야함.
    // 원래 Technique 여러 개랑 개수 몇 개 인지 찾아야 하는데, 하나만 만들 거라 그냥 0
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (pTechnique == nullptr)
        return E_FAIL;

    // Technique 정보를 꺼내 올 거임
    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);

    // pass의 개수를 가지고 옴.
    m_iNumPasses = TechniqueDesc.Passes;

    // 모든 pass 꺼내와서 정점의 정보 확인할 거임.
    // pass 여러 개 만들어서, 서로 다른 셰이더 수행할 수 있기 때문에
    for (size_t i = 0; i < m_iNumPasses; i++)
    {
        // pass 꺼내 옴.
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);

        // pass의 정보를 꺼내온다. 이 안에
        // pIAInputSignature: 어떤 정점을 입력받고 있는가
        // IAInputSignatureSize: 어떤 크기를 가진 정점을 입력받고 있는가
        // 이 두 개랑 그리려하는 정점이 일치해야 한다.
        D3DX11_PASS_DESC PassDesc{};
        pPass->GetDesc(&PassDesc);

        ID3D11InputLayout* pInputLayout = { nullptr };

        // 정점 정보가 잘 맞으면 inputLayout을 만들어준다.
        if (FAILED(m_pDevice->CreateInputLayout(
            pElements,                              // 내가 그릴려하는 정점의 정보
            iNumElements,                           // 그릴려고 하는 정점의 멤버 변수 개수
            PassDesc.pIAInputSignature,             // 셰이더가 어떤 정점을 입력받고 있는가
            PassDesc.IAInputSignatureSize,          // 셰이더가 어떤 크기를 가진 정점을 입력받고 있는가
            &pInputLayout                           // 리턴 값
        )))
            return E_FAIL;

        // pass 개수만큼 inputLayout을 만들어서 보관
        m_InputLayouts.push_back(pInputLayout);

        // inputLayout을 왜 만드냐? 렌더링 할 때 장치에 set해야 해서
    }

    return S_OK;
}

HRESULT CShader::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
    if (iPassIndex >= m_iNumPasses)
        return E_FAIL;

    // inputLayout 세팅
    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

    // 어떤 pass로 그릴지는 선택하지 않았음
    // 그래서 Technique -> pass 가져와서 -> Apply 함수: 이 pass로 그리려고 하는 정점을 공급해라
    // Apply(이 함수) 이전에 셰이더 내부의 전역 변수 채워야 한다.
    m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

    return S_OK;
}

CShader* CShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pShaderFilePath, pElements, iNumElements)))
    {
        MSG_BOX("Failed to Created: CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    for (auto& pInputLayout : m_InputLayouts)
        Safe_Release(pInputLayout);
    m_InputLayouts.clear();

    Safe_Release(m_pEffect);

    __super::Free();
}
