#ifndef __ModuleResourceManager_h__
#define __ModuleResourceManager_h__

#include "Module.h"
#include <map>
#include <list>

struct Texture;
struct Shader;
class Material;
class Bone;
class Mesh;
class Animation;

//TODO: Divide into subclasses for each resource type
class ModuleResourceManager : public Module
{
	public:
		ModuleResourceManager();
		~ModuleResourceManager();

		Texture* GetTexture(std::string filename) const;
		void AddTexture(Texture * texture);
		void DeleteTexture(std::string filename);

		Shader* GetProgram(std::string filename) const;
		std::list<Shader*>GetAllPrograms() const;
		void AddProgram(Shader* shader);
		void DeleteProgram(std::string filename);

		Material* GetMaterial(std::string filename) const;
		void AddMaterial(Material* material);
		void DeleteMaterial(std::string filename);

		Mesh* GetMesh(unsigned uid) const;
		void AddMesh(Mesh* mesh);
		void DeleteMesh(unsigned uid);

		Bone* GetBone(unsigned uid) const;
		void AddBone(Bone* bone);
		std::list<Bone*>GetAllBones() const;
		void DeleteBone(unsigned bone);

		Animation* GetAnim(unsigned uid) const;
		void AddAnim(Animation* bone);
		std::list<Animation*>GetAllAnims() const;
		void DeleteAnim(unsigned bone);

	private:
		std::map<std::string, std::pair<unsigned, Texture*>> textureResources; //filename , times used, texture pointer
		std::map<std::string, std::pair<unsigned, Shader*>> shaderResources; //filename , times used, shader
		std::map<std::string, std::pair<unsigned, Material*>> materialResources; //filename , times used, material
		std::map<unsigned, std::pair<unsigned, Mesh*>> meshResources; // uid, times used, mesh
		std::map<unsigned, std::pair<unsigned, Bone*>> boneResources; // uid, times used, bones
		std::map<unsigned, std::pair<unsigned, Animation*>> animResources; //uid, times used, animations
};

#endif __ModuleResourceManager_h__