#ifndef __ANIMATION_H_
#define __ANIMATION_H_

#include <vector>
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"


struct Channel
{
	std::string channelName = "";
	math::float4x4 channelTransform = math::float4x4::identity;
};

struct Frame
{
	unsigned number = 0u;
	std::vector<Channel*> channels;
};

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
	double framesPerSecond = 0;
	double numberFrames = 0;
	unsigned numberOfChannels = 0u;
	unsigned durationInSeconds = 0u;

	unsigned UID = 0u;

	int currentFrameNumber = 0;
	Frame* currentFrame = nullptr;
	std::vector<Frame*> animationFrames;
};

#endif // __ANIMATION_H_