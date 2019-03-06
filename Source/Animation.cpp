#include "Application.h"

#include "Animation.h"
#include "Globals.h"

#include <assert.h>


Animation::Animation()
{
}


Animation::~Animation()
{
}

void Animation::Load(const char* animationData, unsigned uid)
{
	UID = uid;

	//TODO: COPY THE NAME OF THE ANIMATION

	memcpy(&numberFrames, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&framesPerSecond, animationData, sizeof(double));
	animationData += sizeof(double);

	durationInSeconds = numberFrames * 1/framesPerSecond;

	memcpy(&numberOfChannels, animationData, sizeof(int));
	animationData += sizeof(int);

	for (unsigned j = 0u; j < numberFrames; j++)
	{
		frame* newFrame = new frame();

		for (unsigned i = 0u; i < numberOfChannels; i++)
		{
			channel* newChannel = new channel();

			unsigned nameLength = *(int*)animationData;
			animationData += sizeof(int);

			char* name = new char[nameLength];

			memcpy(name, animationData, sizeof(char) * nameLength);
			animationData += sizeof(char) * nameLength;

		

			std::string cName(name); 	//Crashes sometimes
			newChannel->channelName = cName;

			math::float3 translation = math::float3::zero;
			math::Quat rotation = math::Quat::identity;
			math::float3 scaling = math::float3::one;

			memcpy(&translation, animationData, sizeof(float) * 3);
			animationData += sizeof(float) * 3;

			memcpy(&rotation, animationData, sizeof(Quat));
			animationData += sizeof(Quat);

			math::float4x4 transform = math::float4x4::FromTRS(translation, rotation, scaling);

			newChannel->channelTransform = transform;

			newFrame->channels.push_back(newChannel);
		}

		animationFrames.push_back(newFrame);
	}

	currentFrame = animationFrames.front();
}