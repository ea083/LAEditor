#pragma once
#include "UIStateMachine.h"

class UIStateMachine;

class UIBaseState {
public:
	virtual void enter(UIStateMachine& stateMachine) = 0;
	virtual void update(UIStateMachine& stateMachine) = 0;
	virtual void exit(UIStateMachine& stateMachine) = 0;
};