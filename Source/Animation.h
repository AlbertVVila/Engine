#ifndef __ANIMATION_H_
#define __ANIMATION_H_

#include <string>

class Animation
{
	public:
		Animation();
		~Animation();
	public:
		void Load(const char* animationData);
		void Unload();
	public:

		std::string animationName;
		double duration = 0;
		double ticksPerSecond = 0;
		float numberFrames = 0.0f;
		unsigned numberOfChannels = 0u;

};

#endif // __ANIMATION_H_