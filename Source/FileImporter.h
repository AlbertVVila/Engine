#ifndef __FileImporter_h__
#define __FileImporter_h__

#include <map>
#include "Math/float4x4.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiBone;
struct aiAnimation;
class GameObject;
class Animation;
class Bone;
class Mesh;

class FileImporter {
public:
	FileImporter();
	~FileImporter();

	void ImportAsset(const char* file, const char* folder);

	bool ImportFBX(const char* file, const char* folder);

	bool ImportScene(const aiScene& aiscene, const char* file);

	void ImportAnimation(const aiAnimation& animation, char* data);

	void RewriteAnimationData(Animation* anim, char* data);

	void ImportMesh(const aiMesh& mesh, char* data);

	void ImportBones(aiBone** bones, unsigned numBones, char* data) const;

	unsigned GetMeshSize(const aiMesh& mesh) const;

	unsigned GetAnimationSize(const aiAnimation& animation) const;

};

#endif __FileImporter_h__

