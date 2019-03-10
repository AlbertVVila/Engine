#include "Application.h"

#include "Animation.h"
#include "Globals.h"

#include <assert.h>


Animation::Animation()
{
}


Animation::~Animation()
{
	RELEASE_ARRAY(animationFrames);
}

void Animation::Load(const char* animationData, unsigned uid)
{
	UID = uid;

	double mDuration;
	memcpy(&mDuration, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&framesPerSecond, animationData, sizeof(double));
	animationData += sizeof(double);

	durationInSeconds = mDuration * (1/framesPerSecond);

	memcpy(&numberOfChannels, animationData, sizeof(int));
	animationData += sizeof(int);
	
	numberFrames = (int)mDuration;

	animationFrames = new frame*[numberFrames];

	channel newChannel; //TODO:The classes or structs must begin with uppercase

	for (unsigned j = 0u; j < numberFrames; j++)
	{
		animationFrames[j] = new frame();
		animationFrames[j]->channels = new channel*[numberOfChannels];
		for (unsigned i = 0u; i < numberOfChannels; i++)
		{
			memcpy(&newChannel, animationData, sizeof(channel));
			newChannel.channelTransform = math::float4x4::identity;
			memcpy(&newChannel.channelTransform[0][0], newChannel.rawTransform, sizeof(newChannel.rawTransform));
			animationData += sizeof(channel);
			animationFrames[j]->channels[i] = new channel();
			sprintf_s(animationFrames[j]->channels[i]->channelName, newChannel.channelName);
			animationFrames[j]->channels[i]->channelTransform = math::float4x4(newChannel.channelTransform);
		}
		animationFrames[j]->time = j;
	}

	currentFrame = animationFrames[0];
}