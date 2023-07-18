#pragma once
#include "Utilities.h"
#include <iostream>
#include "UIBaseState.h"
#include "UIIdleState.h"


class UIStateMachine {
private:
	UIBaseState* currentState;
public:
	UIStateMachine() {
		setCurrentState(new UIIdleState());
	}
	UIStateMachine(UIBaseState* initialState){
		setCurrentState(initialState);
	}

	void setCurrentState(UIBaseState* state) {
		if (currentState != nullptr)
			currentState->exit(*this);
		currentState = state;
		currentState->enter(*this);
	}

	void update() {
		currentState->update(*this);
	}

	UIBaseState* getCurrentState() const {
		return currentState;
	}
};