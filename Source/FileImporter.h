#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <map>

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;
struct aiAnimation;
class GameObject;

class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char* file, const char* folder);

	GameObject* ProcessNode(const std::map<unsigned,unsigned>& meshmap, const aiNode* node, const aiScene* scene, GameObject* parent);

	bool ImportFBX(const char* file, const char* folder);

	bool ImportScene(const aiScene& scene, const char* file);

	void ImportAnimation(const aiAnimation& animation, char* data);

	void ImportMesh(const aiMesh& mesh, char* data);

	void ImportBones(const aiMesh& mesh, char* data);

	void ImportSingleBone(const aiBone& bone, char* data);

	unsigned GetMeshSize(const aiMesh& mesh) const;

	unsigned GetSingleBoneSize(const aiBone& bone) const;

	unsigned GetBonesSize(const aiMesh& mesh) const;

	unsigned GetAnimationSize(const aiAnimation& animation) const;
};

#endif __FileImporter_h__

