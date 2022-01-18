#include "GUI/GUI.h"

int main(int argc, char ** argv)
{
    std::string host = "localhost", port = "27015";
	
	GUI::ShowConnectionWindow(host, port);
    
	ImGui::SFML::Shutdown();
	return 0;
}