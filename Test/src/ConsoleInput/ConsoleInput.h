#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class ConsoleInput
{ 

	enum EventType
	{
		None,
		StartProc,
		Message,
		Quit
	};

public:
	ConsoleInput() {}
	void HandleInput();

private:
	EventType ParseInputString(std::string& inputString, std::vector<std::string>& arguments);\

	static std::map<std::string, EventType> m_eventsMap;

};