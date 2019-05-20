#ifndef __RESOURCEANIMATION_H_
#define __RESOURCEANIMATION_H_

#include "Resource.h"

#include <vector>
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Math/Quat.h"

struct Event
{
	int key;
	float time;
	std::string name;

	Event(int k, float t, const std::string& n) : key(k), time(t), name(n) {}

	bool operator < (const Event& evt) const
	{
		return (key < evt.key);
	}
};

struct Channel
{
	std::string channelName = "";
	unsigned numPositionKeys = 0u;
	unsigned numRotationKeys = 0u;
	std::vector<math::float3> positionSamples;
	std::vector<math::Quat> rotationSamples;
};

class ResourceAnimation : public Resource
{
public:
	ResourceAnimation(unsigned uid);
	ResourceAnimation(const ResourceAnimation& resource);
	virtual ~ResourceAnimation();

public:
	bool LoadInMemory() override;		// Will load in memory all Resourceanims stored
	void DeleteFromMemory() override;	// Will delete all ResourceAnims stored in memory
	void Delete() override;
	void SaveMetafile(const char* file) const override;
	void LoadConfigFromMeta() override;

	void SetAnimation(const char* animData);
	unsigned GetAnimationSize();
	void SaveAnimationData(char* data);
	void SaveNewAnimation();

	unsigned GetNumPositions(unsigned indexChannel) const;
	const math::float3 GetPosition(unsigned indexChannel, unsigned indexPosition) const;

	unsigned GetNumRotations(unsigned indexChannel) const;
	const math::Quat GetRotation(unsigned indexChannel, unsigned indexPosition) const;

	unsigned GetIndexChannel(std::string name) const;
	unsigned GetNumberChannels() const;
	unsigned GetNumberFrames() const;
	unsigned GetFPS() const;

public:

	double duration = 0; //frames
	double framesPerSecond = 0;
	int numberFrames = 0;
	unsigned numberOfChannels = 0u;
	float durationInSeconds = 0u;

	int currentSample = 0u;
	int currentFrame = 0u;
	int totalEvents = 0;

	std::vector<Channel*> channels;
	std::vector<Event*> events;
};

#endif // __RESOURCEANIMATION_H_