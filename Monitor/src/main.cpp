#include "GUI/ConnectionWindow.h"

int main(int argc, char ** argv)
{
    std::string host = "localhost", port = "27015";
	
	//GUI::ShowConnectionWindow(host, port);
	auto dcsConnection = std::make_shared<DCSConnection>();
	GUI::ConnectionWindow connectionWindow(host, port, dcsConnection);
    
	ImGui::SFML::Shutdown();
	return 0;
}