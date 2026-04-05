#ifndef Engine_My_Format_h__
#define Engine_My_Format_h__

#define TEXTURE_TYPE_MAX 27

namespace Engine
{
	struct myVtxInfo
	{
		_float3 mVertex{};
		_float3 mNormal{};
		_float2 mTexcoord{};

		_float3 mTangent{};
		_float3 mBitangent{};
	};

	struct myVertexWeight
	{
		_uint		mVertexId{};
		_float		mWeight{};
	};

	struct myBone {
		string					mName;

		_uint					mNumWeights{};
		vector<myVertexWeight>  mWeights;

		_float4x4				mOffsetMatrix{};
	};

	struct myMesh
	{
		string				mName{};

		_uint				mMaterialIndex{};

		_uint				mNumVertices{};
		vector<myVtxInfo>	mVerticesInfo;

		_uint				mNumIndices{};
		vector<_uint>		mIndices;

		_uint				mNumBones{};
		vector<myBone>		mBones;
	};

	struct myMaterial
	{
		_uint				mNumTextures[TEXTURE_TYPE_MAX]{};
		vector<string>		Textures[TEXTURE_TYPE_MAX];
	};

	struct myNode
	{
		string				mName;
		_float4x4			mTransformation{};

		_uint				mNumChildren{};
		vector<myNode>		mChildren;
	};

	struct myVectorKey 
	{
		_float3 mValue{};
		_float mTime{};
	};

	struct myQuatKey
	{
		_float4 mValue{};
		_float mTime{};
	};

	struct myNodeAnim 
	{
		string				mNodeName;

		_uint mNumScalingKeys{};
		vector<myVectorKey> mScalingKeys;

		_uint mNumRotationKeys{};
		vector<myQuatKey> mRotationKeys;

		_uint mNumPositionKeys{};
		vector<myVectorKey> mPositionKeys;
	};

	struct myAnimation 
	{
		string              mName;

		_float				mDuration{};
		_float				mTickPerSecond{};

		_uint				mNumChannels{};
		vector< myNodeAnim> mChannels;
	};

	struct myScene
	{
		_uint				mNumMeshes{};
		vector<myMesh>		mMeshes;

		_uint				mNumMaterials{};
		vector<myMaterial>	mMaterials;

		myNode				mRootNode;

		_uint				mNumAnimations{};
		vector<myAnimation> mAnimations;
	};
}

#endif // Engine_My_Format_h__
