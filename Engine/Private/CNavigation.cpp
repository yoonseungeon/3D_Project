#include "CNavigation.h"

#include "CCell.h"
#include "CGameInstance.h"
#include "CTransform.h"

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

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile, const _tchar* pNeighborIndicesFilePath)
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

    SetUp_Neighbors(pNeighborIndicesFilePath);

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (m_pShader == nullptr)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (pArg == nullptr) {
        MSG_BOX("Need Argument Component: CNavigation");
        return E_FAIL;
    }

    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    if (pDesc->bIsGround == true) {
        m_iCurrentCellIndex = -1;

        if (pDesc->pParentMarix == nullptr) {
            MSG_BOX("Need m_pParentMatrixPtr Component: CNavigation");
            return E_FAIL;
        }
        m_pParentMatrixPtr = pDesc->pParentMarix;
    }
    else
    {
        if (pDesc->pTransformCom == nullptr) {
            MSG_BOX("Need Transform Component: CNavigation");
            return E_FAIL;
        }
        m_pParentTransform = pDesc->pTransformCom;
        Safe_AddRef(m_pParentTransform);

        Find_CurCell_Index(pDesc->vObjectWorldPos);
    }

    return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
    // src에서 dst한테 점 두 개를 주고, 이웃인지 물어본다.
    // 이웃이면 이 변의 이웃이라고 인덱스 저장
    //for (auto& pSourCell : m_Cells)
    //{
    //    for (auto& pDestCell : m_Cells)
    //    {
    //        if (pSourCell == pDestCell)
    //        {
    //            continue;
    //        }

    //        if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::A), pSourCell->Get_Point(CELL_POINT::B)) == true)
    //            pSourCell->Set_Neighbor(LINE::AB, pDestCell);
    //        else if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::B), pSourCell->Get_Point(CELL_POINT::C)) == true)
    //            pSourCell->Set_Neighbor(LINE::BC, pDestCell);
    //        else if (pDestCell->Compare(pSourCell->Get_Point(CELL_POINT::C), pSourCell->Get_Point(CELL_POINT::A)) == true)
    //            pSourCell->Set_Neighbor(LINE::CA, pDestCell);
    //    }
    //}

    for (size_t i = 0; i < m_Cells.size(); ++i)
    {
        for (size_t j = 1 + i; j < m_Cells.size(); ++j)
        {
            _vector vA = m_Cells[i]->Get_Point(CELL_POINT::A);
            _vector vB = m_Cells[i]->Get_Point(CELL_POINT::B);
            _vector vC = m_Cells[i]->Get_Point(CELL_POINT::C);

            LINE eLine = {};

            if (m_Cells[j]->Compare(vA, vB, eLine) == true)
            {
                m_Cells[i]->Set_Neighbor(LINE::AB, m_Cells[j]);
                m_Cells[j]->Set_Neighbor(eLine, m_Cells[i]);
            }
            else if (m_Cells[j]->Compare(vB, vC, eLine) == true)
            {
                m_Cells[i]->Set_Neighbor(LINE::BC, m_Cells[j]);
                m_Cells[j]->Set_Neighbor(eLine, m_Cells[i]);
            }
            else if (m_Cells[j]->Compare(vC, vA, eLine) == true)
            {
                m_Cells[i]->Set_Neighbor(LINE::CA, m_Cells[j]);
                m_Cells[j]->Set_Neighbor(eLine, m_Cells[i]);
            }
        }
    }

    return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors(const _tchar* pNeighborIndicesFilePath)
{
    _ulong dwByte = {};
    HANDLE hFile = CreateFile(pNeighborIndicesFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == 0)
        return E_FAIL;

    _int iNeighbors[3] = {};

    for (auto& pCell : m_Cells)
    {
        ReadFile(hFile, iNeighbors, sizeof(_int) * 3, &dwByte, nullptr);

        pCell->Set_Neighbors(iNeighbors);
    }
    CloseHandle(hFile);

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

void CNavigation::Compute_OnNavigation()
{
    if (-1 == m_iCurrentCellIndex)
        return;

    _vector vCurrentPosition = m_pParentTransform->Get_State(STATE::POSITION);

    _float fHeight = m_Cells[m_iCurrentCellIndex]->Compute_Height(vCurrentPosition);

    m_pParentTransform->Set_State(STATE::POSITION, XMVectorSetY(vCurrentPosition, fHeight));
}

const list<_vector>* CNavigation::Make_Route(_float3 vTargetPos)
{
  /*  _int iTargetCellIndex = -1;

    _int iDumy = { -1 };
    for (auto pCell : m_Cells)
    {
        if (pCell->isIn(XMLoadFloat3(&vTargetPos), &iDumy))
        {
            iTargetCellIndex = pCell->Get_CellIdx();
            break;
        }
    }

    if (iTargetCellIndex == -1)
    {
        return nullptr;
    }

    m_iOldTargetCellIndex = iTargetCellIndex;

    CloseList.clear();
    while (OpenPriQ.empty() != true) {
        OpenPriQ.pop();
    }
    m_Route.clear();

    _float fG = {};
    _float fH = m_Cells[m_iCurrentCellIndex]->Compute_Cost(m_Cells[iTargetCellIndex]->Get_Center());
    _float fF = fG + fH;

    OpenPriQ.emplace(-1, m_iCurrentCellIndex, fF, fG, fH);

    while (true) {

        AStar_Info tAStarInfo = OpenPriQ.top();

        CloseList.push_back(tAStarInfo);
        OpenPriQ.pop();

        _int Neighbors[3];
        m_Cells[tAStarInfo.CellIndex]->Get_Neighbors(Neighbors);

        for (size_t i = 0; i < 3; ++i)
        {
            _float fG = tAStarInfo.fF;
            _float fH = m_Cells[Neighbors[i]]->Compute_Cost(m_Cells[iTargetCellIndex]->Get_Center());
            _float fF = fG + fH;

            OpenPriQ.emplace(-1, Neighbors[i], fF, fG, fH);
        }


    }
*/

    return nullptr;
}

_bool CNavigation::isMove(_fvector vResultPos)
{
    // 움직이고 난 결과 위치가 필요하다. 그래서 TransformCom 안에서 호출
    if (m_iCurrentCellIndex == -1)
        return false;

    _int iNeighborIndex = { -1 };

    // Cell 안에 있냐?
    while (true)
    {
        iNeighborIndex = -1;

        // 현재 셀 위인지 검사
        if (m_Cells[m_iCurrentCellIndex]->isIn(vResultPos, &iNeighborIndex) == true)
        {
            // 현재 셀 위면 true
            return true;
        }

        // 아닌데 이웃이 없으면 false
        if (iNeighborIndex == -1)
        {
            return false;
        }

        // 나간 방향에 해당하는 인덱스를 현재 인덱스로 바꿈
        // 새로운 이웃이 있으면 저장
        m_iCurrentCellIndex = iNeighborIndex;
    }
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    _float4x4 WorldMatrix = *m_pParentMatrixPtr;
    _float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);

    m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(D3DTS::VIEW));
    m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(D3DTS::PROJ));

    if (-1 == m_iCurrentCellIndex)
    {
        // 지형으로부터 얻어와야 함.
        m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
        m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor);
        m_pShader->Begin(0);

        for (auto& pCell : m_Cells)
        {
            if (nullptr != pCell)
                pCell->Render();
        }
    }
    else
    {
        // 지형이 아닌 경우 빨간색으로
        WorldMatrix._42 += 0.05f;
        vColor = _float4(1.f, 0.f, 0.f, 1.f);

        m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix);
        m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof vColor);
        m_pShader->Begin(0);

        m_Cells[m_iCurrentCellIndex]->Render();
    }
    return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFile, const _tchar* pNeighborIndicesFilePath)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile, pNeighborIndicesFilePath)))
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
    //_ulong          dwByte = {};
    //HANDLE          hFile = CreateFile(TEXT("../Bin/DataFiles/Neighbors.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    //_int         iNeighbors[3];

    //for (auto& pCell : m_Cells)
    //{
    //    memcpy(iNeighbors, pCell->Get_NeighborIndices(), sizeof(_int) * 3);
    //    WriteFile(hFile, iNeighbors, sizeof(_int) * 3, &dwByte, nullptr);
    //}

    //CloseHandle(hFile);

    Safe_Release(m_pParentTransform);

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);
    m_Cells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif

    __super::Free();
}
