#include <iostream>
#include <string>
#include "Utilities.h"
#include <glm/gtx/projection.hpp>

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
				console_index = appPointers.log->logConsole("Grabbing x: 0 y: 0 z: 0");
				orig_position = appPointers.model->getSelectedVerticesMedian();
			}
		}
		void update(StateMachine& stateMachine, Utilities::AppPointers appPointers) override {
			Mouse* mouse = appPointers.mouse;
			Model* model = appPointers.model;
			Camera* camera = appPointers.camera;
			if (Utilities::getCurrentTimeInMS() - mouse->getMovingTime() < 5) {
				const float cameraDistMultiplier = glm::distance(model->getSelectedVerticesMedian(), camera->Position);

				const float curr_x_offset = mouse->getOffset().y * 0.015;
				const float curr_y_offset = mouse->getOffset().x * 0.015;

				if (grabbing_axis == glm::vec3(1.0f)) { // grabbing in screen space
					model->translateSelectedVertices(curr_x_offset, camera->Up);
					model->translateSelectedVertices(curr_y_offset, camera->Right);
				}
				else { // grabbing on some axis
					if (use_camera_up)
						model->translateSelectedVertices(curr_y_offset, grabbing_axis);
					else
						model->translateSelectedVertices(curr_x_offset, grabbing_axis);
				}
				updateConsoleOutput(appPointers);
			}

			if (inputFlags & xKey) {
				// need to set it to just x axis
				grabbing_axis = glm::vec3(1.0f, 0.0f, 0.0f);
				// find camera axis that is closest to axis
				const float up_dot_product = glm::abs(glm::dot(camera->Up, grabbing_axis));
				const float right_dot_product = glm::abs(glm::dot(camera->Right, grabbing_axis));
				if (up_dot_product > right_dot_product) {
					use_camera_up = true;
				}
				else {
					use_camera_up = false;
				}
				// need to project to axis
				//appPointers.log->logConsole("projection of 2.0, 1.0, 3.0 on to 1.0, 0.0, 0.0");
				//glm::vec3 projection = glm::proj(glm::vec3(2.0f, 1.0f, 3.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				//appPointers.log->logConsole("x: " + std::to_string(projection.x) + " y: " + std::to_string(projection.y) + " z: " + std::to_string(projection.z));
				//appPointers.model->setSelectedVerticesMedianPos(glm::vec3(0.0f));
				constrain_to_axis(appPointers);
			}

			if (inputFlags & qKey) {
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

		glm::vec3 grabbing_axis = glm::vec3(1.0f);
		bool use_camera_up = false;

		void updateConsoleOutput(Utilities::AppPointers appPointers) {
			const glm::vec3 curr_pos = appPointers.model->getSelectedVerticesMedian();
			const glm::vec3 offset = curr_pos - orig_position;
			appPointers.log->updateConsole(console_index, "Grabbing x: " + std::to_string(offset.x) +
				" y: " + std::to_string(offset.y) +
				" z: " + std::to_string(offset.z));
		}

		void constrain_to_axis(Utilities::AppPointers appPointers) {
			const glm::vec3 curr_pos = appPointers.model->getSelectedVerticesMedian();
			const glm::vec3 curr_translation = curr_pos - orig_position;
			const glm::vec3 curr_axis_translation = glm::proj(curr_translation, grabbing_axis);
			const glm::vec3 new_pos = orig_position + curr_axis_translation;
			appPointers.model->setSelectedVerticesMedianPos(new_pos);
		}
	};
}