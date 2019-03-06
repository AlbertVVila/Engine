#ifndef __ANIMATION_H_
#define __ANIMATION_H_

#include <vector>
#include "Math/float3.h"
#include "Math/Quat.h"

class Animation
{
public:
	Animation();
	~Animation();
public:
	void Load(const char* animationData, unsigned uid);
	void Unload();
public:

	std::string animationName;
	double duration = 0;
	double ticksPerSecond = 0;
	float numberFrames = 0.0f;
	unsigned numberOfChannels = 0u;

	unsigned UID = 0u;

	struct channel
	{
		std::string channelName;
		math::float3 channelTranslation = math::float3::zero;
		math::Quat channelRotation = math::Quat::identity;
	};
		
	int currentFrame = 0;
	channel* currentChannel;

	std::vector<channel*> channels;
};

#endif // __ANIMATION_H_