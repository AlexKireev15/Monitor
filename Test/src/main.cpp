#include <Network.h>
#include <iostream>
#include <random>
#include <mutex>

#include "MonitorConnection/MonitorConnection.h"
#include "TestWorker/TestWorker.h"

int main(int argc, char ** argv)
{
	std::mt19937 generator((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
	MonitorConnection monitorConnection;
	TestWorker worker;
	bool isExit = false;
	bool isTerminateCalculation = false;
	bool mainNotified = false;

	std::mutex mainMutex;
	std::condition_variable mainCondition;

	auto work = [&isExit, &isTerminateCalculation, &monitorConnection](std::mt19937& generator, std::string args)
	{
		for (int i = 0; i < 20 && !isTerminateCalculation && !isExit; ++i)
		{
			std::string res = "Some calculations done. Job arguments: " + args;
			std::this_thread::sleep_for(std::chrono::milliseconds(100 + generator() % 900));
			std::cout << res << std::endl;
			monitorConnection.Send(res);
		}
		monitorConnection.Send("Work done");
		std::cout << "Work done" << std::endl;
	};

	monitorConnection.StartListening(NULL, "27015", 
	[&work, &worker, &generator, &isTerminateCalculation, &isExit, &mainNotified, &mainCondition](auto e, auto str)
	{
		std::function<void(void)> job;
		switch (e)
		{
		case Network::Connection::EventType::Message:
			std::cout << "Monitor said: " << str << std::endl;
			break;
		case Network::Connection::EventType::StartCalculation:
			isTerminateCalculation = false;
			job = [&work, &generator, str]() { work(generator, str); };
			worker.AddJob(job);
			break;
		case Network::Connection::EventType::TerminateCalculation:
			isTerminateCalculation = true;
			break;

		case Network::Connection::EventType::Exit:
			isExit = true;
			worker.Stop();
			mainNotified = true;
			mainCondition.notify_one();
			break;
		case Network::Connection::EventType::None:
		default:
			break;
		}
	});
	
	std::unique_lock<std::mutex> locker(mainMutex);
	while (!mainNotified)
	{
		mainCondition.wait(locker);
	}
	mainNotified = false;
}
