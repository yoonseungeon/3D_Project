#include "CNavigation.h"

#include "CCell.h"
#include "CGameInstance.h"

const _float4x4* CNavigation::m_pParentMatrixPtr = { nullptr };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent{ Prototype }
    , m_Cells{ Prototype.m_Cells }
#ifdef _DEBUG
    , m_pShader{ Prototype.m_pShader }
#endif
{
#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif

    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{
    _ulong dwByte{};
    HANDLE hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == 0)
        return E_FAIL;

    // 삼각형 단위로 계속 읽음
    while (true)
    {
        _float3 vPoints[3]{};

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        if (dwByte == 0)
            break;

        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_uint>(m_Cells.size()));
        if (pCell == nullptr)
            return E_FAIL;

        m_Cells.push_back(pCell);
    }

    CloseHandle(hFile);

    SetUp_Neighbors();

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (m_pShader == nullptr)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    m_iCurrentCellIndex = pDesc->iCurrentCellIndex;

    // 지형인 경우
    if (pDesc->iCurrentCellIndex == -1)
        m_pParentMatrixPtr = pDesc->pParentMarix;

    Find_CurCell_Index(pDesc->vObjectWorldPos);

    return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
    // src에서 dst한테 점 두 개를 주고, 이웃인지 물어본다.
    // 이웃이면 이 변의 이웃이라고 인덱스 저장
    for (auto& pSourCell : m_Cells)
    {
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
            {
                continue;
            }

            if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::A), pSourCell->Get_Point(CELL_POINT::B)) == true)
                pSourCell->Set_Neighbor(LINE::AB, pDestCell);
            else if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::B), pSourCell->Get_Point(CELL_POINT::C)) == true)
                pSourCell->Set_Neighbor(LINE::BC, pDestCell);
            else if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::C), pSourCell->Get_Point(CELL_POINT::A)) == true)
                pSourCell->Set_Neighbor(LINE::CA, pDestCell);
        }
    }

    return S_OK;
}

_bool CNavigation::Find_CurCell_Index(_float3& vWorldPos)
{
    _vector vPos = XMLoadFloat3(&vWorldPos);
    if (m_pParentMatrixPtr != nullptr)
    {
        vPos = XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(m_pParentMatrixPtr)));
    }

    _int iDumy = { -1 };

    for (auto pCell : m_Cells)
    {
        if (pCell->isIn(vPos, &iDumy))
        {
            m_iCurrentCellIndex = pCell->Get_CellIdx();
            return true;
        }
    }

    m_iCurrentCellIndex = -1;

    return false;
}

_bool CNavigation::isMove(_fvector vResultPos)
{
    // 움직이고 난 결과 위치가 필요하다. 그래서 TransformCom 안에서 호출

    if (m_iCurrentCellIndex == -1)
        return false;

    _int iNeighborIndex = { -1 };

    // Cell 안에 있냐?
    if (m_Cells[m_iCurrentCellIndex]->isIn(vResultPos, &iNeighborIndex) == true)
    {
        return true;
    }
    else
    {
        // 나간 방향에 해당하는 인덱스를 현재 인덱스로 바꿈
        if (iNeighborIndex != -1)
        {
            m_iCurrentCellIndex = iNeighborIndex;
            return true;
        }
        else // 이웃이 없으면 못 움직임.
        {
            return false;
        }
    }
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    // 지형으로부터 얻어와야 함.
    m_pShader->Bind_Matrix("g_WorldMatrix", m_pParentMatrixPtr);

    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ));

    m_pShader->Begin(0);

    for (auto& pCell : m_Cells)
    {
        if (pCell != nullptr)
            pCell->Render();
    }
    return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile)))
    {
        MSG_BOX("Failed to Created: CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif

    __super::Free();
}
