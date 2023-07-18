#pragma once
#include "UIBaseState.h"
//#include "UIStateMachine.h"

class UIIdleState : public UIBaseState {
	void enter(UIStateMachine& stateMachine) override {
		std::cout << "Entering Idle State" << std::endl;
	}

	void update(UIStateMachine& stateMachine) override {
		std::cout << "Updating Idle State" << std::endl;
	}

	void exit(UIStateMachine& stateMachine) override {
		std::cout << "Exiting Idle State" << std::endl;
	}
};