#include "../ConsoleInput.h"

#include <Network.h>

#include <regex>


std::map<std::string, ConsoleInput::EventType> ConsoleInput::m_eventsMap = 
{
	{"QUIT", ConsoleInput::EventType::Quit},
	{"Q", ConsoleInput::EventType::Quit},
	{"EXIT", ConsoleInput::EventType::Quit},
	{"M", ConsoleInput::EventType::Message},

};

void ConsoleInput::HandleInput()
{
	bool quit = false;
	while (!quit)
	{
		std::string inStr;
		std::getline(std::cin, inStr);
		//std::cin >> inStr;
		EventType e = EventType::None;
		std::vector<std::string> arguments;
		e = ParseInputString(inStr, arguments);
		{
			switch (e)
			{
			case Message:
				if (!arguments.empty()) 
				{
					//Network::NetworkController::GetInstance()->Broadcast(arguments[0]);
				}
				break;

			case StartProc:
				break;

			case Quit:
				quit = true;
				break;

			case None:
			default:
				break;
			}
		}
	}
}

ConsoleInput::EventType ConsoleInput::ParseInputString(std::string& inputString, std::vector<std::string>& arguments)
{
	std::regex regex("\\ ");
	std::vector<std::string> out
	(
		std::sregex_token_iterator(inputString.begin(), inputString.end(), regex, -1),
		std::sregex_token_iterator()
	);

	for (auto idx = 0u; idx < out[0].length(); ++idx)
		out[0][idx] = std::toupper(out[0][idx], std::locale());

	if (out.empty() || m_eventsMap.find(out[0]) == m_eventsMap.end())
		return EventType::None;

	arguments.clear();

	if(out.size() > 1)
		arguments.assign(++out.begin(), out.end());

	return m_eventsMap[out[0]];
}
