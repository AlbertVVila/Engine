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

	memcpy(&numberFrames, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&framesPerSecond, animationData, sizeof(double));
	animationData += sizeof(double);

	durationInSeconds = numberFrames * 1/framesPerSecond;

	memcpy(&numberOfChannels, animationData, sizeof(int));
	animationData += sizeof(int);

	
	for (unsigned j = 0u; j < numberFrames; j++)
	{
		frame* newFrame = new frame();//TODO:The classes or structs must begin with uppercase
		newFrame->channels.resize(numberOfChannels); //push_back is not efficient

		for (unsigned i = 0u; i < numberOfChannels; i++)
		{
			channel* newChannel = new channel(); //TODO:The classes or structs must begin with uppercase

			char name[MAX_BONE_NAME_LENGTH]; //crash fix - Here we don't need to care of namelength. The /0 marks the end of the string. Notice the strings are length + 1 ALWAYS

			memcpy(name, animationData, sizeof(char) * MAX_BONE_NAME_LENGTH);
			animationData += sizeof(char)* MAX_BONE_NAME_LENGTH;

			newChannel->channelName = std::string(name);

			math::float3 translation = math::float3::zero;
			math::Quat rotation = math::Quat::identity;
			math::float3 scaling = math::float3::one;

			memcpy(&translation, animationData, sizeof(float) * 3);
			animationData += sizeof(float) * 3;

			memcpy(&rotation, animationData, sizeof(math::Quat));
			animationData += sizeof(math::Quat);

			math::float4x4 transform = math::float4x4::FromTRS(translation, rotation, scaling);
			newChannel->channelTransform = transform;

			newFrame->channels[i] = newChannel; 
		}

		animationFrames.push_back(newFrame);
	}

	currentFrame = animationFrames.front();
}