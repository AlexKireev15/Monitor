#include <Network.h>
#include <iostream>
#include <string>

#include <random>
#include <thread>

#include "MonitorConnection/MonitorConnection.h"

MonitorConnection		g_mc;
bool                    g_done = false;

void TestCalculations(std::mt19937& generator)
{
	while (!g_done)
	{
		std::string res = "Some calculations done";
		std::this_thread::sleep_for(std::chrono::milliseconds(100 + generator() % 900));
		std::cout << res << std::endl;
		g_mc.Send(res);
	}
}

int main(int argc, char **argv)
{
	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());

	std::mutex				g_mutex;
	std::condition_variable g_condition;
	bool					g_notify = false;

	g_mc.StartListening(NULL, "27015", [&g_mutex, &g_condition, &g_notify](auto e, auto str) 
		{
			if(e == Network::Connection::EventType::TerminateCalculation)
			{
				std::unique_lock<std::mutex> locker(g_mutex);
				g_notify = true;
				g_condition.notify_one();
			}
			else if (e == Network::Connection::EventType::Message)
				std::cout << "Monitor said: " << str << std::endl;
		});


	std::thread calcThread(&TestCalculations, std::ref(generator));

	std::unique_lock<std::mutex> locker(g_mutex);
	while (!g_notify)
	{
		g_condition.wait(locker);
	}
	g_notify = false;

	g_done = true;
	calcThread.join();

	return 0;
}