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
		std::this_thread::sleep_for(std::chrono::seconds(1 + generator() % 5));
		std::cout << res << std::endl;
		g_mc.Send(res);
	}
}

int main(int argc, char **argv)
{
	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	
	g_mc.StartListening(NULL, "27015");
	std::thread calcThread(&TestCalculations, std::ref(generator));

	std::cin.get();
	g_done = true;
	calcThread.join();

	return 0;
}