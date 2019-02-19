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
		//TODO: set up bone structure
};

#endif // __BONE_H__
