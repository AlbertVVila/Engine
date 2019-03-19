#ifndef __STATEMACHINE_H_
#define __STATEMACHINE_H_

#include <vector>

class StateMachine
{
public:

	struct State
	{
		std::string name;
		unsigned clipUID = 0u;
	};

	void AddState(State* state);
	void RemoveState(unsigned UID);
	unsigned FindStateIndexByUID(unsigned UID);

	StateMachine();
	~StateMachine();

private:
	std::vector<State*> states;
};

#endif // __STATEMACHINE_H_