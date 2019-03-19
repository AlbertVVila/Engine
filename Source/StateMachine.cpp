#include "StateMachine.h"



StateMachine::StateMachine()
{
}


StateMachine::~StateMachine()
{
}

unsigned StateMachine::FindStateIndexByUID(unsigned UID)
{
	for (unsigned i = 0u; i < states.size(); ++i)
	{
		if (UID == states[i]->clipUID)
			return i;
	}
	return 999u; //TODO use a propper sentinel
}

void StateMachine::AddState(State* state)
{
	states.push_back(state);
}

void StateMachine::RemoveState(unsigned UID)
{
	unsigned stateIndex = FindStateIndexByUID(UID);

	if(stateIndex != 999u)
		states.erase(states.begin()+stateIndex-1);
}