#include <iostream>
#include <string>
#include <thread>

void ThreadProc(int index)
{
	std::cout << "Thread " << index << " executing" << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cout << "Invalid arguments count" << std::endl;

		return 1;
	}

	int numberOfThreads = std::atoi(argv[1]);

	for (int i = 0; i < numberOfThreads; i++)
	{
		std::thread thread(&ThreadProc, i + 1);
		
		thread.join();
	}

	return 0;
}
