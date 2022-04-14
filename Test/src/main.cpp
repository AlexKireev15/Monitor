#include <Network.h>
#include <iostream>
#include <string>

#include <random>
#include <thread>

#include "MonitorConnection/MonitorConnection.h"

MonitorConnection		g_mc;
bool                    g_done = false;
bool					g_terminateCalculation = false;
std::string				g_resString;

void TestCalculations(std::mt19937& generator)
{
	while (!g_terminateCalculation && !g_done)
	{
		std::string res = "Some calculations done" + g_resString;
		std::this_thread::sleep_for(std::chrono::milliseconds(100 + generator() % 900));
		std::cout << res << std::endl;
		g_mc.Send(res);
	}
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << "Host and port required as arguments" << std::endl;
		return 1;
	}
	if (argc > 3)
	{
		g_resString = argv[3];
	}

	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

	std::mutex				g_mutex;
	std::condition_variable g_condition;
	bool					g_notify = false;

	g_mc.StartListening(argv[1], argv[2], [&g_mutex, &g_condition, &g_notify](auto e, auto str) 
		{
			if (e == Network::Connection::EventType::StartCalculation)
			{
				std::unique_lock<std::mutex> locker(g_mutex);
				g_terminateCalculation = false;
				g_notify = true;
				g_condition.notify_one();
			}
			else if(e == Network::Connection::EventType::TerminateCalculation)
			{
				std::unique_lock<std::mutex> locker(g_mutex);
				g_terminateCalculation = true;
				g_notify = true;
				g_condition.notify_one();
			}
			else if (e == Network::Connection::EventType::Exit)
			{
				std::unique_lock<std::mutex> locker(g_mutex);
				g_terminateCalculation = true;
				g_done = true;
				g_notify = true;
				g_condition.notify_one();
			}
			else if (e == Network::Connection::EventType::Message)
				std::cout << "Monitor said: " << str << std::endl;
		});
	std::thread calcThread;

	std::unique_lock<std::mutex> locker(g_mutex);
	while(!g_done)
	{
		if(!g_terminateCalculation && !calcThread.joinable())
			calcThread = std::thread(&TestCalculations, std::ref(generator));
		while (!g_notify)
		{
			g_condition.wait(locker);
		}
		g_notify = false;

		if(g_terminateCalculation && calcThread.joinable())
			calcThread.join();
	}

	return 0;
}