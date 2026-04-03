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
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION
        // 구식 문법 차단, 경고도 에러로
        | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

    // 이 함수로 셰이더 파일을 빌드할 것임.(ID3DX11Effect도 만듬)
    if (FAILED(D3DX11CompileEffectFromFile(
        pShaderFilePath,                        // 컴파일한 셰이더 파일 경로
        nullptr,                                // 전처리 매크로 목록
        D3D_COMPILE_STANDARD_FILE_INCLUDE,      // 셰이더 안에서 #include를 누가 처리할지
        iHlslFlag,                              // 셰이더 컴파일 플래그(옵션)
        0,                                      // effect(.fx) 컴파일 옵션이라고 함.
        m_pDevice,                              // Effect 객체 생성을 위한 그래픽 디바이스
        &m_pEffect,                             // 리턴 값(Effect객체)
        nullptr                                 // 컴파일 실패 이유를 돌려받는 출력 포인터
    )))
        return E_FAIL;

    // pass는 Technique한테 감싸져 있음. Technique한테 먼저 접근해야함.
    // 원래 Technique 여러 개랑 개수 몇 개 인지 찾아야 하는데, 하나만 만들 거라 그냥 0
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (pTechnique == nullptr || pTechnique->IsValid() == false)
        return E_FAIL;

    // Technique 정보를 꺼내 올 거임
    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);

    // pass의 개수를 가지고 옴.
    m_iNumPasses = TechniqueDesc.Passes;
    m_InputLayouts.reserve(m_iNumPasses);

    // 모든 pass 꺼내와서 정점의 정보 확인할 거임.
    // pass 여러 개 만들어서, 서로 다른 셰이더 수행할 수 있기 때문에
    for (size_t i = 0; i < m_iNumPasses; ++i)
    {
        // pass 꺼내 옴.
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(static_cast<_uint>(i));

        // pass의 정보를 꺼내온다. 이 안에
        // pIAInputSignature: 셰이더가 요구하는 정점 입력 형식 정보의 시작 주소
        // IAInputSignatureSize: pIAInputSignature가 가리키는 시그니처 크기
        // 이 두 개랑 그리려하는 정점이 일치해야 한다.
        D3DX11_PASS_DESC PassDesc{};
        pPass->GetDesc(&PassDesc);

        ID3D11InputLayout* pInputLayout = { nullptr };

        // 정점 정보가 잘 맞으면 inputLayout을 만들어준다.
        if (FAILED(m_pDevice->CreateInputLayout(
            pElements,                              // 셰이더에 넘길 정점 입력 형식 정보 배열
            iNumElements,                           // pElements 배열의 원소 개수
            PassDesc.pIAInputSignature,             // 셰이더가 요구하는 정점 입력 형식 정보의 시작 주소
            PassDesc.IAInputSignatureSize,          // pIAInputSignature가 가리키는 시그니처 크기
            &pInputLayout                           // 리턴 값
        )))
            return E_FAIL;

        // pass 개수만큼 inputLayout을 만들어서 보관
        m_InputLayouts.push_back(pInputLayout);

        // inputLayout을 왜 만드냐? IA 단계에서 필요
        // 입력 데이터 식별, 셰이더 입력과 맞는지 검사
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
    // 그래서 Technique -> pass 가져와서 -> Apply
    // Apply: 이 pass에 들어 있는 셰이더와 설정을 context에 적용한다.
    // Apply(Begin 함수) 이전에 셰이더 내부의 전역 변수(행렬, 텍스처) 채워야 한다.
    m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(iPassIndex)->Apply(0, m_pContext);

    return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
    // 셰이더 파일 안에 있는 전역 변수를 찾아오는 함수
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (pVariable == nullptr || pVariable->IsValid() == false)
        return E_FAIL;

    // ID3DX11EffectVariable를 Matrix 변수 인터페이스로 해석하는 함수
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (pMatrixVariable == nullptr || pMatrixVariable->IsValid() == false)
        return E_FAIL;

    // Matrix를 셰이더 변수에 세팅
    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(pMatrix));
}

HRESULT CShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (pVariable == nullptr)
        return E_FAIL;

    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (pMatrixVariable == nullptr)
        return E_FAIL;

    return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(pMatrices), 0, iNumMatrices);
}

HRESULT CShader::Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
    // 셰이더 파일 안에 있는 전역 변수를 찾아오는 함수
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (pVariable == nullptr || pVariable->IsValid() == false)
        return E_FAIL;

    // ID3DX11EffectVariable를 Shader Resource 변수 인터페이스로 해석하는 함수
    ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
    if (pSRVariable == nullptr || pVariable->IsValid() == false)
        return E_FAIL;

    // SRV를 셰이더 변수에 세팅
    return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iSize)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
    if (pVariable == nullptr || pVariable->IsValid() == false)
        return E_FAIL;

    // SRV, 행렬은 SetRawValue 사용하면 문제 생김.
    return pVariable->SetRawValue(pData, 0, iSize);
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
