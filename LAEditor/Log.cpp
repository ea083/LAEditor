/*#include "Log.h"

//std::shared_ptr<spdlog::logger> Log::coreLogger;
//std::shared_ptr<spdlog::logger> Log::clientLogger;
//Console Log::console;

void Log::Init() {
	//std::shared_ptr<spdlog::logger> Log::coreLogger;
	//std::shared_ptr<spdlog::logger> Log::clientLogger;
	//Console Log::console;

	spdlog::set_pattern("%^[%T] %n: %v%$"); // timestamp, name of logger (client or core), message
	coreLogger = spdlog::stdout_color_mt("LAEDITOR");
	coreLogger->set_level(spdlog::level::trace);
	try {
		clientLogger = spdlog::basic_logger_mt("APP", "logs/logs.txt");
		clientLogger->set_level(spdlog::level::trace);
	}
	catch (const spdlog::spdlog_ex& ex) {
		//LAE_CORE_ERROR("Client log init failed: ");
	}
}

void Log::showConsole() {
	bool showConsole = true;
	//static Console console;
	console.Draw("Console", &showConsole);
}

void Log::debug() {
    if (ImGui::TreeNode("console")) {
        if (Utilities::startDebugTable("Variables", "Name", "Value")) {
            Utilities::nameVariableDebugTable(console.InputBuf, "console.InputBuf");
            for (auto item : console.Items) {
                Utilities::nameVariableDebugTable(item, "console.Items");
            }
            for (auto command : console.Commands) {
                Utilities::nameVariableDebugTable(command, "console.Commands");
            }
            for (auto history : console.History) {
                Utilities::nameVariableDebugTable(history, "console.History");
            }
            Utilities::nameVariableDebugTable(console.HistoryPos, "console.HistoryPos");
            Utilities::nameVariableDebugTable(console.AutoScroll, "console.AutoScroll");
            Utilities::nameVariableDebugTable(console.ScrollToBottom, "console.ScrollToBottom");
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}
*/