#ifndef __ANIMATION_H_
#define __ANIMATION_H_

class Animation
{
	public:
		Animation();
		~Animation();
	public:
		void Load(const char* animationData);
		void Unload();
	public:
		char animationName[30];
		double duration = 0;
		double ticksPerSecond = 0;
		unsigned numberOfChannels = 0u;
};

#endif // __ANIMATION_H_