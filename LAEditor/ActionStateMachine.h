#include <iostream>
#include <string>
#include "Utilities.h"

namespace ASM {
	typedef int ASMInputFlags;

	enum ASMInputFlags_ {
		none = 0,
		xKey = 1 << 0,
		yKey = 1 << 1,
		zKey = 1 << 2,
		qKey = 1 << 3,
		gKey = 1 << 4,
	};

	class StateMachine;
	class IdleState;
	class GrabState;

	class State {
	public:
		virtual ~State() {}
		virtual void enter(StateMachine& stateMachine, Utilities::AppPointers appPointers) = 0;
		virtual void update(StateMachine& stateMachine, Utilities::AppPointers appPointers) = 0;
		virtual void exit(StateMachine& stateMachine, Utilities::AppPointers appPointers) = 0;
		virtual std::string getID() {
			return "Default State";
		}

		void setFlags(ASMInputFlags inputFlags) {
			this->inputFlags = inputFlags;
		}
		ASMInputFlags inputFlags = 0;
	};

	
	class StateMachine {
	public:
		StateMachine() : appPointers() {}

		void init(std::shared_ptr<State> newState, Utilities::AppPointers appPointers) {
			setState(newState);
			this->appPointers = appPointers;
		}

		void setState(std::shared_ptr<State> newState) {
			if (currentStateU) {
				if (newState->getID() == currentStateU->getID()) {
					std::cout << "WARNING::ACTIONSTATEMACHINE::Trying to enter the current state" << std::endl;
					std::cout << "\tCurrent State ID: " << currentStateU->getID();
					std::cout << " New State ID: " << newState->getID() << std::endl;
					return;
				}
				currentStateU->exit(*this, appPointers);
			}
			currentStateU = newState;
			if (currentStateU)
				currentStateU->enter(*this, appPointers);

		}

		void setFlags(ASMInputFlags inputFlags) {
			currentStateU->setFlags(inputFlags);
		}

		void update() {
			if (currentStateU)
				currentStateU->update(*this, appPointers);
		}

		void debug() {
			static ImGuiTableFlags flags =
				ImGuiTableFlags_Borders |
				ImGuiTableFlags_RowBg |
				ImGuiTableFlags_SizingFixedFit |
				ImGuiTableFlags_ScrollY |
				ImGuiTableFlags_ScrollX;
			ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 10);

			if (ImGui::TreeNode("Action State Machine")) {
				if(Utilities::startDebugTable("Variables", "Name", "Value")) {
					Utilities::nameVariableDebugTable(currentStateU->getID(), "currentState ID");
					ImGui::EndTable();
				}
				if (Utilities::startDebugTable("Pointers", "Name", "Value")) {
					Utilities::nameVariableDebugTable(appPointers.mouse, "mouse pointer");
					Utilities::nameVariableDebugTable(appPointers.model, "model pointer");
					Utilities::nameVariableDebugTable(appPointers.camera, "camera pointer");
					Utilities::nameVariableDebugTable(appPointers.window, "window pointer");
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
	private:
		std::shared_ptr<State> currentStateU;
		Utilities::AppPointers appPointers;
	};

	class IdleState : public State {
	public:
		void enter(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {

		}
		void update(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {

		}
		void exit(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {

		}
		std::string getID() override {
			return "IdleState";
		}
	};

	class GrabState : public State {
	public:
		void enter(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {
			if (appPointers.model->getNumSelectedVertices() == 0) {
				stateMachine.setState(std::make_shared<ASM::IdleState>());
			}
			else {
				(appPointers.mouse)->setKeepCursorInFrame(true);
				console_index = appPointers.log->logConsole("Grabbing: x: 0 y: 0");
				orig_position = appPointers.model->getSelectedVerticesMedian();
			}
		}
		void update(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {
			Mouse* mouse = appPointers.mouse;
			Model* model = appPointers.model;
			Camera* camera = appPointers.camera;
			if (Utilities::getCurrentTimeInMS() - mouse->getMovingTime() < 5) {
				const float cameraDistMultiplier = glm::distance(model->getSelectedVerticesMedian(), camera->Position);
				const float curr_x_offset = mouse->getOffset().y * 0.005;
				const float curr_y_offset = mouse->getOffset().x * 0.005;
				model->translateSelectedVertices(curr_x_offset, camera->Up);
				model->translateSelectedVertices(curr_y_offset, camera->Right);
				updateConsoleOutput(appPointers);
			}

			if (inputFlags & xKey) {
				stateMachine.setState(std::make_shared<ASM::IdleState>());
			}
		}
		void exit(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {
			(appPointers.mouse)->setKeepCursorInFrame(false);
		}
		std::string getID() override {
			return "GrabState";
		}
	private:
		int console_index = 0;
		glm::vec3 orig_position = glm::vec3(0.0f);

		void updateConsoleOutput(Utilities::AppPointers appPointers) {
			const glm::vec3 curr_pos = appPointers.model->getSelectedVerticesMedian();
			const glm::vec3 offset = curr_pos - orig_position;
			appPointers.log->updateConsole(console_index, "Grabbing x: " + std::to_string(offset.x) +
				" y: " + std::to_string(offset.y) +
				" z: " + std::to_string(offset.z));
		}
	};
}