#include "CMyModelImporter.h"

CMyModelImporter::CMyModelImporter()
{
}

const myScene* CMyModelImporter::ReadFile(const char* pFilePath)
{
    if (pFilePath == nullptr)
    {
        MSG_BOX("ReadFile failed : pFilePath is nullptr");
        m_strError = "ReadFile failed : pFilePath is nullptr";
        return nullptr;
    }

    myScene tLoadedScene{};

    if (false == LoadMySceneBinary(pFilePath, tLoadedScene))
    {
        if (m_strError.empty())
        {
            MSG_BOX("ReadFile failed : LoadMySceneBinary failed");
            m_strError = "ReadFile failed : LoadMySceneBinary failed";
        }

        return nullptr;
    }

    m_tMyScene = std::move(tLoadedScene);
    m_strError.clear();

    return &m_tMyScene;
}

void CMyModelImporter::FreeScene()
{
    m_tMyScene = myScene{};
    m_strError.clear();
}

bool CMyModelImporter::ReadString(std::ifstream& ifs, std::string& str)
{
    _uint iLength = 0;

    ifs.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
    if (!ifs.good())
        return false;

    str.clear();

    if (iLength > 0)
    {
        str.resize(iLength);
        ifs.read(&str[0], iLength);

        if (!ifs.good())
            return false;
    }

    return true;
}

bool CMyModelImporter::ReadFloat2(std::ifstream& ifs, _float2& v)
{
    ifs.read(reinterpret_cast<char*>(&v), sizeof(_float2));
    return ifs.good();
}

bool CMyModelImporter::ReadFloat3(std::ifstream& ifs, _float3& v)
{
    ifs.read(reinterpret_cast<char*>(&v), sizeof(_float3));
    return ifs.good();
}

bool CMyModelImporter::ReadFloat4(std::ifstream& ifs, _float4& v)
{
    ifs.read(reinterpret_cast<char*>(&v), sizeof(_float4));
    return ifs.good();
}

bool CMyModelImporter::ReadMatrix(std::ifstream& ifs, _float4x4& mat)
{
    ifs.read(reinterpret_cast<char*>(&mat), sizeof(_float4x4));
    return ifs.good();
}

///////////////////////////////////////////////////////////////////////////////////////

bool CMyModelImporter::ReadVertex(std::ifstream& ifs, myVtxInfo& vtx)
{
    if (!ReadFloat3(ifs, vtx.mVertex))      return false;
    if (!ReadFloat3(ifs, vtx.mNormal))      return false;
    if (!ReadFloat2(ifs, vtx.mTexcoord))    return false;
    if (!ReadFloat3(ifs, vtx.mTangent))     return false;
    if (!ReadFloat3(ifs, vtx.mBitangent))   return false;

    return true;
}

bool CMyModelImporter::ReadVertexWeight(std::ifstream& ifs, myVertexWeight& weight)
{
    ifs.read(reinterpret_cast<char*>(&weight.mVertexId), sizeof(_uint));
    if (!ifs.good())
        return false;

    ifs.read(reinterpret_cast<char*>(&weight.mWeight), sizeof(_float));
    if (!ifs.good())
        return false;

    return true;
}

bool CMyModelImporter::ReadBone(std::ifstream& ifs, myBone& bone)
{
    if (!ReadString(ifs, bone.mName))
        return false;

    ifs.read(reinterpret_cast<char*>(&bone.mNumWeights), sizeof(_uint));
    if (!ifs.good())
        return false;

    bone.mWeights.clear();
    bone.mWeights.resize(bone.mNumWeights);

    for (_uint i = 0; i < bone.mNumWeights; ++i)
    {
        if (!ReadVertexWeight(ifs, bone.mWeights[i]))
            return false;
    }

    if (!ReadMatrix(ifs, bone.mOffsetMatrix))
        return false;

    return true;
}

bool CMyModelImporter::ReadMesh(std::ifstream& ifs, myMesh& mesh)
{
    if (!ReadString(ifs, mesh.mName))
        return false;

    ifs.read(reinterpret_cast<char*>(&mesh.mMaterialIndex), sizeof(_uint));
    if (!ifs.good())
        return false;

    // Vertices
    ifs.read(reinterpret_cast<char*>(&mesh.mNumVertices), sizeof(_uint));
    if (!ifs.good())
        return false;

    mesh.mVerticesInfo.clear();
    mesh.mVerticesInfo.resize(mesh.mNumVertices);

    for (_uint i = 0; i < mesh.mNumVertices; ++i)
    {
        if (!ReadVertex(ifs, mesh.mVerticesInfo[i]))
            return false;
    }

    // Indices
    ifs.read(reinterpret_cast<char*>(&mesh.mNumIndices), sizeof(_uint));
    if (!ifs.good())
        return false;

    mesh.mIndices.clear();
    mesh.mIndices.resize(mesh.mNumIndices);

    if (mesh.mNumIndices > 0)
    {
        ifs.read(reinterpret_cast<char*>(mesh.mIndices.data()), sizeof(_uint) * mesh.mNumIndices);
        if (!ifs.good())
            return false;
    }

    // Bones
    ifs.read(reinterpret_cast<char*>(&mesh.mNumBones), sizeof(_uint));
    if (!ifs.good())
        return false;

    mesh.mBones.clear();
    mesh.mBones.resize(mesh.mNumBones);

    for (_uint i = 0; i < mesh.mNumBones; ++i)
    {
        if (!ReadBone(ifs, mesh.mBones[i]))
            return false;
    }

    return true;
}

bool CMyModelImporter::ReadMaterial(std::ifstream& ifs, myMaterial& material)
{
    for (_uint i = 0; i < TEXTURE_TYPE_MAX; ++i)
    {
        ifs.read(reinterpret_cast<char*>(&material.mNumTextures[i]), sizeof(_uint));
        if (!ifs.good())
            return false;

        material.Textures[i].clear();
        material.Textures[i].resize(material.mNumTextures[i]);

        for (_uint j = 0; j < material.mNumTextures[i]; ++j)
        {
            if (!ReadString(ifs, material.Textures[i][j]))
                return false;
        }
    }

    return true;
}

bool CMyModelImporter::ReadNode(std::ifstream& ifs, myNode& node)
{
    if (!ReadString(ifs, node.mName))
        return false;

    if (!ReadMatrix(ifs, node.mTransformation))
        return false;

    ifs.read(reinterpret_cast<char*>(&node.mNumChildren), sizeof(_uint));
    if (!ifs.good())
        return false;

    node.mChildren.clear();
    node.mChildren.resize(node.mNumChildren);

    for (_uint i = 0; i < node.mNumChildren; ++i)
    {
        if (!ReadNode(ifs, node.mChildren[i]))
            return false;
    }

    return true;
}

bool CMyModelImporter::ReadVectorKey(std::ifstream& ifs, myVectorKey& key)
{
    if (!ReadFloat3(ifs, key.mValue))
        return false;

    ifs.read(reinterpret_cast<char*>(&key.mTime), sizeof(_float));
    if (!ifs.good())
        return false;

    return true;
}

bool CMyModelImporter::ReadQuatKey(std::ifstream& ifs, myQuatKey& key)
{
    if (!ReadFloat4(ifs, key.mValue))
        return false;

    ifs.read(reinterpret_cast<char*>(&key.mTime), sizeof(_float));
    if (!ifs.good())
        return false;

    return true;
}

bool CMyModelImporter::ReadNodeAnim(std::ifstream& ifs, myNodeAnim& nodeAnim)
{
    if (!ReadString(ifs, nodeAnim.mNodeName))
        return false;

    // Scaling Keys
    ifs.read(reinterpret_cast<char*>(&nodeAnim.mNumScalingKeys), sizeof(_uint));
    if (!ifs.good())
        return false;

    nodeAnim.mScalingKeys.clear();
    nodeAnim.mScalingKeys.resize(nodeAnim.mNumScalingKeys);

    for (_uint i = 0; i < nodeAnim.mNumScalingKeys; ++i)
    {
        if (!ReadVectorKey(ifs, nodeAnim.mScalingKeys[i]))
            return false;
    }

    // Rotation Keys
    ifs.read(reinterpret_cast<char*>(&nodeAnim.mNumRotationKeys), sizeof(_uint));
    if (!ifs.good())
        return false;

    nodeAnim.mRotationKeys.clear();
    nodeAnim.mRotationKeys.resize(nodeAnim.mNumRotationKeys);

    for (_uint i = 0; i < nodeAnim.mNumRotationKeys; ++i)
    {
        if (!ReadQuatKey(ifs, nodeAnim.mRotationKeys[i]))
            return false;
    }

    // Position Keys
    ifs.read(reinterpret_cast<char*>(&nodeAnim.mNumPositionKeys), sizeof(_uint));
    if (!ifs.good())
        return false;

    nodeAnim.mPositionKeys.clear();
    nodeAnim.mPositionKeys.resize(nodeAnim.mNumPositionKeys);

    for (_uint i = 0; i < nodeAnim.mNumPositionKeys; ++i)
    {
        if (!ReadVectorKey(ifs, nodeAnim.mPositionKeys[i]))
            return false;
    }

    return true;
}

bool CMyModelImporter::ReadAnimation(std::ifstream& ifs, myAnimation& anim)
{
    if (!ReadString(ifs, anim.mName))
        return false;

    ifs.read(reinterpret_cast<char*>(&anim.mDuration), sizeof(_float));
    if (!ifs.good())
        return false;

    ifs.read(reinterpret_cast<char*>(&anim.mTickPerSecond), sizeof(_float));
    if (!ifs.good())
        return false;

    ifs.read(reinterpret_cast<char*>(&anim.mNumChannels), sizeof(_uint));
    if (!ifs.good())
        return false;

    anim.mChannels.clear();
    anim.mChannels.resize(anim.mNumChannels);

    for (_uint i = 0; i < anim.mNumChannels; ++i)
    {
        if (!ReadNodeAnim(ifs, anim.mChannels[i]))
            return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////

bool CMyModelImporter::LoadMySceneBinary(const std::string& strFilePath, myScene& outScene)
{
    std::ifstream ifs(strFilePath, std::ios::binary);
    if (!ifs.is_open())
    {
        MSG_BOX("Failed to open file");
        m_strError = "Failed to open file";
        return false;
    }

    // Header
    char szMagic[4] = {};
    _uint iVersion = 0;

    ifs.read(szMagic, sizeof(szMagic));
    if (!ifs.good())
    {
        MSG_BOX("Failed to read file header");
        m_strError = "Failed to read file header";
        return false;
    }

    if (memcmp(szMagic, "MYMD", 4) != 0)
    {
        MSG_BOX("Invalid mymodel magic");
        m_strError = "Invalid mymodel magic";
        return false;
    }

    ifs.read(reinterpret_cast<char*>(&iVersion), sizeof(_uint));
    if (!ifs.good())
    {
        MSG_BOX("Failed to read version");
        m_strError = "Failed to read version";
        return false;
    }

    if (iVersion != 2)
    {
        MSG_BOX("Unsupported mymodel version");
        m_strError = "Unsupported mymodel version";
        return false;
    }

    // Meshes
    ifs.read(reinterpret_cast<char*>(&outScene.mNumMeshes), sizeof(_uint));
    if (!ifs.good())
    {
        MSG_BOX("Failed to read mesh count");
        m_strError = "Failed to read mesh count";
        return false;
    }

    outScene.mMeshes.clear();
    outScene.mMeshes.resize(outScene.mNumMeshes);

    for (_uint i = 0; i < outScene.mNumMeshes; ++i)
    {
        if (!ReadMesh(ifs, outScene.mMeshes[i]))
        {
            MSG_BOX("Failed to read mesh");
            m_strError = "Failed to read mesh";
            return false;
        }
    }

    // Materials
    ifs.read(reinterpret_cast<char*>(&outScene.mNumMaterials), sizeof(_uint));
    if (!ifs.good())
    {
        MSG_BOX("Failed to read material count");
        m_strError = "Failed to read material count";
        return false;
    }

    outScene.mMaterials.clear();
    outScene.mMaterials.resize(outScene.mNumMaterials);

    for (_uint i = 0; i < outScene.mNumMaterials; ++i)
    {
        if (!ReadMaterial(ifs, outScene.mMaterials[i]))
        {
            MSG_BOX("Failed to read material");
            m_strError = "Failed to read material";
            return false;
        }
    }

    // Root Node
    if (!ReadNode(ifs, outScene.mRootNode))
    {
        MSG_BOX("Failed to read root node");
        m_strError = "Failed to read root node";
        return false;
    }

    // Animations
    ifs.read(reinterpret_cast<char*>(&outScene.mNumAnimations), sizeof(_uint));
    if (!ifs.good())
    {
        MSG_BOX("Failed to read animation count");
        m_strError = "Failed to read animation count";
        return false;
    }

    outScene.mAnimations.clear();
    outScene.mAnimations.resize(outScene.mNumAnimations);

    for (_uint i = 0; i < outScene.mNumAnimations; ++i)
    {
        if (!ReadAnimation(ifs, outScene.mAnimations[i]))
        {
            MSG_BOX("Failed to read animation");
            m_strError = "Failed to read animation";
            return false;
        }
    }

    m_strError.clear();
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void CMyModelImporter::Free()
{
    __super::Free();
}