#ifndef __BONE_H__
#define __BONE_H__

class Bone
{
	public:
		Bone();
		~Bone();

	public:
		void Load(const char* meshData, unsigned uid);
		void Unload();
	public:
		char* boneName = nullptr;
};

#endif // __BONE_H__
