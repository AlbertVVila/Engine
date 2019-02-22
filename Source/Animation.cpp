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

void Animation::Load(const char* animationData)
{
	memcpy(&duration, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&ticksPerSecond, animationData, sizeof(double));
	animationData += sizeof(double);

	memcpy(&numberOfChannels, animationData, sizeof(int));
	animationData += sizeof(int);
}