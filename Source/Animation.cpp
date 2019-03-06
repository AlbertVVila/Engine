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

	memcpy(&duration, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&ticksPerSecond, animationData, sizeof(double));
	animationData += sizeof(double);

	numberFrames = duration * ticksPerSecond;

	memcpy(&numberOfChannels, animationData, sizeof(int));
	animationData += sizeof(int);

	for (unsigned i = 0u; i < numberOfChannels; i++)
	{
		channel* newChannel = new channel();

		unsigned nameLength = *(int*)animationData;
		animationData += sizeof(int);

		char* name = new char[nameLength];
	
		memcpy(name, animationData, sizeof(char) * nameLength);
		animationData += sizeof(char) * nameLength;

		std::string cName(name);
		newChannel->channelName = cName;

		memcpy(&newChannel->channelTranslation, animationData, sizeof(float) * 3);
		animationData += sizeof(float) * 3;

		memcpy(&newChannel->channelRotation, animationData, sizeof(float) * 3);
		animationData += sizeof(float) * 3;

		channels.push_back(newChannel);

	}
	currentChannel = channels.front();
}