#ifndef __STATEMACHINE_H_
#define __STATEMACHINE_H_

#include <vector>

class StateMachine
{
public:


	StateMachine();
	~StateMachine();

public:
	
	void AddClip(const std::string name, unsigned UID, bool loop);
	void AddNode(const std::string name,const std::string clipName);
	void AddTransition(const std::string origin, const std::string destiny, const std::string trigger, unsigned blend);

	unsigned FindClip(const std::string name);
	unsigned FindNode(const std::string name);
	unsigned FindTransition(const std::string origin, const std::string trigger);

	//Clips setters and getters

	std::string GetClipName(unsigned index);
	unsigned GetClipUID(unsigned index);
	bool GetClipLoop(unsigned index);
	void SetClipName(unsigned index, std::string name);
	void SetClipUID(unsigned index, unsigned UID);
	void SetClipLoop(unsigned index, bool loop);

	//Transitions setters and getters
	std::string GetTransitionOrigin(unsigned index);
	std::string GetTransitionDestiny(unsigned index);
	std::string GetTransitionTrigger(unsigned index);
	unsigned GetTransitionBlend(unsigned index);
	void SetTransitionOrigin(unsigned index, std::string origin);
	void SetTransitionDestiny(unsigned index, std::string destiny);
	void SetTransitionTrigger(unsigned index, std::string trigger);
	void SetTransitionBlend(unsigned index, unsigned blend);

	//Nodes setters and getters
	std::string GetNodeName(unsigned index);
	std::string GetNodeClip(unsigned index);
	void SetNodeName(unsigned index, std::string name);
	void SetNodeClip(unsigned index, std::string clipName);

	unsigned GetNodesSize();
	unsigned GetClipsSize();
	unsigned GetTransitionsSize();

	void RemoveNodeTransitions(std::string transitionName);

	void RemoveClip(unsigned UID);
	void RemoveNode(unsigned UID);
	void RemoveTransition(unsigned UID);

private:
	
	struct Clip
	{
		std::string name;
		unsigned UID = 0u;
		bool loop = false;

		Clip() { ; }
		Clip(std::string n, unsigned u, bool l) : name(n), UID(u), loop(l) { ; }
	};

	struct Transition
	{
		std::string origin;
		std::string destiny;
		std::string trigger;

		unsigned blend = 200u;

		Transition() { ; }
		Transition(std::string o, std::string d, std::string t, unsigned b) : 
			origin(o), destiny(d), trigger(t), blend(b) { ; }
	};

	struct Node
	{
		std::string name;
		std::string clipName;

		Node() { ; }
		Node(std::string n, std::string cn) : name(n), clipName(cn) { ; }
	};

	std::vector<Node> nodes;
	std::vector<Transition> transitions;
	std::vector<Clip> clips;
	unsigned defaultNode = 0u;
};

#endif // __STATEMACHINE_H_