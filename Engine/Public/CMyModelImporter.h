#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMyModelImporter final : public CBase
{
public:
	CMyModelImporter();
	virtual ~CMyModelImporter() = default;

public:
	const myScene* ReadFile(const char* pFilePath);
	void FreeScene();

private:
	myScene m_tMyScene{};
	string m_strError{};

private:
	bool ReadString(std::ifstream& ifs, std::string& str);
	bool ReadFloat2(std::ifstream& ifs, _float2& v);
	bool ReadFloat3(std::ifstream& ifs, _float3& v);
	bool ReadFloat4(std::ifstream& ifs, _float4& v);
	bool ReadMatrix(std::ifstream& ifs, _float4x4& mat);

	bool ReadVertex(std::ifstream& ifs, Engine::myVtxInfo& vtx);
	bool ReadVertexWeight(std::ifstream& ifs, Engine::myVertexWeight& weight);
	bool ReadBone(std::ifstream& ifs, Engine::myBone& bone);
	bool ReadMesh(std::ifstream& ifs, Engine::myMesh& mesh);
	bool ReadMaterial(std::ifstream& ifs, Engine::myMaterial& material);
	bool ReadNode(std::ifstream& ifs, Engine::myNode& node);

	bool ReadVectorKey(std::ifstream& ifs, Engine::myVectorKey& key);
	bool ReadQuatKey(std::ifstream& ifs, Engine::myQuatKey& key);
	bool ReadNodeAnim(std::ifstream& ifs, Engine::myNodeAnim& nodeAnim);
	bool ReadAnimation(std::ifstream& ifs, Engine::myAnimation& anim);

	bool LoadMySceneBinary(const std::string& strFilePath, Engine::myScene& outScene);

protected:
	virtual void Free() override;
};

NS_END