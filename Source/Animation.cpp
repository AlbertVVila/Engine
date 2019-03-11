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

	for (unsigned i = 0u; i < numberOfChannels; i++)
	{
		Channel* newChannel = new Channel(); //TODO:The classes or structs must begin with uppercase

		char name[MAX_BONE_NAME_LENGTH]; //crash fix - Here we don't need to care of namelength. The /0 marks the end of the string. Notice the strings are length + 1 ALWAYS

		memcpy(name, animationData, sizeof(char) * MAX_BONE_NAME_LENGTH);
		animationData += sizeof(char)* MAX_BONE_NAME_LENGTH;

		newChannel->channelName = std::string(name);

		memcpy(&newChannel->numPositionKeys, animationData, sizeof(int));
		animationData += sizeof(int);

		memcpy(&newChannel->numRotationKeys, animationData, sizeof(int));
		animationData += sizeof(int);

		for (unsigned j = 0u; j < newChannel->numPositionKeys; j++)
		{
			math::float3 position = math::float3::one;
			memcpy(&position, animationData, sizeof(float) * 3);
			animationData += sizeof(float) * 3;
			newChannel->positionSamples.push_back(position);
		}

		for (unsigned k = 0u; k < newChannel->numRotationKeys; k++)
		{
			math::Quat rotation = math::Quat::identity;
			memcpy(&rotation, animationData, sizeof(math::Quat));
			animationData += sizeof(math::Quat);
			newChannel->rotationSamples.push_back(rotation);
		}

		channels.push_back(newChannel);
	}
}

unsigned Animation::GetNumPositions(unsigned indexChannel) const
{
	return channels[indexChannel]->numPositionKeys;
}

unsigned Animation::GetNumRotations(unsigned indexChannel) const
{
	return channels[indexChannel]->numRotationKeys;
}

const math::float3 Animation::GetPosition(unsigned indexChannel, unsigned indexPosition) const
{
	if(channels[indexChannel]->numPositionKeys > 1)
		return channels[indexChannel]->positionSamples[indexPosition];
	else
		return channels[indexChannel]->positionSamples[0];
}

const math::Quat Animation::GetRotation(unsigned indexChannel, unsigned indexPosition) const
{
	if (channels[indexChannel]->numRotationKeys > 1)
		return channels[indexChannel]->rotationSamples[indexPosition];
	else
		return channels[indexChannel]->rotationSamples[0];
}

unsigned Animation::GetIndexChannel(std::string name) const
{
	for (unsigned i = 0u; i < numberOfChannels; i++)
	{
		if (strcmp(name.c_str(), channels[i]->channelName.c_str()) == 0)
		{
			return i;
		}
	}
	return 999u;
}