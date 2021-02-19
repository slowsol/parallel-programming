#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>

void ThreadProc(int index)
{
	std::cout << "Thread " + std::to_string(index) + " executing\n";
}

int main(int argc, char* argv[])
{
	if (argc != 2) 
	{
		std::cout << "Invalid arguments count" << std::endl;

		return 1;
	}

	std::vector<std::thread> threads;
	
	int numberOfThreads = std::atoi(argv[1]);

	for (int i = 0; i < numberOfThreads; i++)
	{
		threads.emplace_back(std::thread(&ThreadProc, i + 1));
	}

	for (auto& thread : threads)
	{
		thread.join();
	}

	return 0;
}
