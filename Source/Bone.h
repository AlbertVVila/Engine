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
		char boneName[30];
		unsigned UID = 0;
};

#endif // __BONE_H__
