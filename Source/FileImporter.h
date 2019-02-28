#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <map>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;
struct aiAnimation;
class GameObject;
class Bone;

class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char* file, const char* folder);

	void ProcessNode(const std::map<unsigned,unsigned>& meshmap, 
		const aiNode* node, const aiScene* scene, GameObject* parent, std::vector<std::string*>* boneNames);

	bool ImportFBX(const char* file, const char* folder);

	bool ImportScene(const aiScene& aiscene, const char* file,
		std::vector<unsigned>& meshesUID, std::vector<unsigned>& animationsUID,
		std::vector<unsigned>& bonesUID);

	void ImportAnimation(const aiAnimation& animation, char* data);

	void ImportMesh(const aiMesh& mesh, char* data);

	std::vector<unsigned> ImportBones(const aiMesh& mesh, std::vector<Bone*> rBones,
		std::vector<unsigned>& bonesUID, std::map<std::string*, unsigned>& boneMap,
		std::vector<std::string*>* boneNames, unsigned meshUID);

	void ImportSingleBone(const aiBone& bone, char* data);

	unsigned GetMeshSize(const aiMesh& mesh) const;

	unsigned GetSingleBoneSize(const aiBone& bone) const;

	unsigned GetBonesSize(const aiMesh& mesh) const;

	unsigned GetAnimationSize(const aiAnimation& animation) const;
};

#endif __FileImporter_h__

