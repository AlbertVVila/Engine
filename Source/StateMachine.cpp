#include "StateMachine.h"
#include "Globals.h"


StateMachine::StateMachine()
{
}


StateMachine::~StateMachine()
{
}

void StateMachine::AddClip(const std::string name, Animation* anim, const bool loop)
{
	clips.push_back(Clip(name, anim, loop));
}

void StateMachine::AddNode(const std::string name, const std::string clipName)
{
	nodes.push_back(Node(name, clipName));
}

void StateMachine::AddTransition(const std::string origin, const std::string destiny, const std::string trigger, unsigned blend)
{
	transitions.push_back(Transition(origin, destiny, trigger, blend));
}

unsigned StateMachine::FindClip(std::string name)
{
	unsigned i;

	for (i = 0u; i < clips.size(); ++i)
	{
		if (strcmp(clips[i].name.c_str(), name.c_str()) == 0)
		{
			break;
		}
	}
	return i;
}

unsigned StateMachine::FindNode(std::string name)
{
	unsigned i;

	for (i = 0u; i < nodes.size(); ++i)
	{
		if (strcmp(nodes[i].name.c_str(), name.c_str()) == 0)
		{
			break;
		}
	}
	return i;
}

unsigned StateMachine::FindTransition(std::string origin, std::string trigger)
{
	unsigned i;

	for (i = 0u; i < transitions.size(); ++i)
	{
		if (strcmp(transitions[i].origin.c_str(), origin.c_str()) == 0 && strcmp(transitions[i].trigger.c_str(), trigger.c_str()) == 0)
		{
			break;
		}
	}
	return i;
}

std::string StateMachine::GetClipName(unsigned index)
{
	return clips[index].name;
}

Animation* StateMachine::GetClipAnimation(unsigned index)
{
	return clips[index].anim;
}

bool StateMachine::GetClipLoop(unsigned index)
{
	return clips[index].loop;
}

std::string StateMachine::GetTransitionOrigin(unsigned index)
{
	return transitions[index].origin;
}

std::string StateMachine::GetTransitionDestiny(unsigned index)
{
	return transitions[index].destiny;
}

std::string StateMachine::GetTransitionTrigger(unsigned index)
{
	return transitions[index].trigger;
}

unsigned StateMachine::GetTransitionBlend(unsigned index)
{
	return transitions[index].blend;
}

std::string StateMachine::GetNodeName(unsigned index)
{
	return nodes[index].name;
}

std::string StateMachine::GetNodeClip(unsigned index)
{
	return nodes[index].clipName;
}

unsigned StateMachine::GetNodesSize()
{
	return nodes.size();
}

unsigned StateMachine::GetClipsSize()
{
	return clips.size();
}

unsigned StateMachine::GetTransitionsSize()
{
	return transitions.size();
}

void StateMachine::SetClipName(unsigned index, std::string name)
{
	clips[index].name = name;
}

void StateMachine::SetClipAnimation(unsigned index, Animation* anim)
{
	clips[index].anim = anim;
}

void StateMachine::SetClipLoop(unsigned index, bool loop)
{
	clips[index].loop = loop;
}

void StateMachine::SetTransitionOrigin(unsigned index, std::string origin)
{
	transitions[index].origin = origin;
}

void StateMachine::SetTransitionDestiny(unsigned index, std::string destiny)
{
	transitions[index].destiny = destiny;
}

void StateMachine::SetTransitionTrigger(unsigned index, std::string trigger)
{
	transitions[index].trigger = trigger;
}

void StateMachine::SetTransitionBlend(unsigned index, unsigned blend)
{
	transitions[index].blend = blend;
}

void StateMachine::SetNodeName(unsigned index, std::string name)
{
	nodes[index].name = name;
}

void StateMachine::SetNodeClip(unsigned index, std::string clipName)
{
	nodes[index].clipName = clipName;
}

void StateMachine::RemoveClip(unsigned UID)
{

}

void StateMachine::RemoveNode(unsigned index)
{
	RemoveNodeTransitions(nodes[index].name);
	nodes.erase(nodes.begin() + index);

	if (nodes.empty())
	{
		defaultNode = 0u;
	}
	else
	{
		if (defaultNode > nodes.size() - 1)
		{
			defaultNode = nodes.size()-1;
		}

	}
}

void StateMachine::RemoveTransition(unsigned index)
{
	transitions.erase(transitions.begin() + index);
}

void StateMachine::RemoveNodeTransitions(std::string nodeName)
{
	std::vector<Transition>::iterator it = transitions.begin();

	while (it != transitions.end())
	{
		if (it->origin == nodeName || it->destiny == nodeName)
		{
			it = transitions.erase(it);
		}
		else
		{
			++it;
		}
	}
}