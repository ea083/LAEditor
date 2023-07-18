#pragma once
#include "UIBaseState.h"
//#include "UIStateMachine.h"

class UIGrabState : public UIBaseState {
	void enter(UIStateMachine& stateMachine) override {
		std::cout << "Entering Grab State" << std::endl;
	}

	void update(UIStateMachine& stateMachine) override {
		std::cout << "Updating Grab State" << std::endl;
	}

	void exit(UIStateMachine& stateMachine) override {
		std::cout << "Exiting Grab State" << std::endl;
	}
};