#include "BMP.h"
#include "BMPUtils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <future>
#include <mutex>
#include <stdint.h>
#include <sched.h>
#include <cpuid.h>
#include "os_mac.cc"
#include "os_mac.h"

#define CPUID(INFO, LEAF, SUBLEAF) __cpuid_count(LEAF, SUBLEAF, INFO[0], INFO[1], INFO[2], INFO[3])

unsigned int GetCurrentCPUNumber() {
   int CPUInfo[4];   
   CPUID(CPUInfo, 1, 0);                          
   // CPUInfo[1] is EBX, bits 24-31 are APIC ID
   if ((CPUInfo[3] & (1 << 9)) == 0) return -1;  // no APIC on chip
   return (unsigned)CPUInfo[1] >> 24;
}

struct Args
{
	BMP* unblurred;
	BMP* blurred;

	int start;
	int end;
};

void ThreadProc(Args* args) {
    BMPUtils::BlurByWidth(args->unblurred, args->blurred, args->start, args->end);
}

int main(int argc, const char * argv[]) 
{
    if (argc != 5)
    {
        std::cout << "Invalid arguments count." << std::endl;

        return 1;
    }

    std::string inFileName = argv[1];
    std::string outFileName = argv[2];

    int threadsCount = std::stoi(argv[3]);
    int coresCount = std::stoi(argv[4]);

    std::cout << "Reading from " + inFileName + "..." << std::endl;

    BMP bmpToBlur = BMPUtils::ReadFromPath(inFileName);

    BMP blurred = BMP(bmpToBlur);

    uint32_t width = (bmpToBlur.GetWidth() - 1) / threadsCount;

	std::vector<Args> argsToPass;

	int minWidth = 0;

    auto blurringStartTime = std::chrono::high_resolution_clock::now();

    std::cout << "Preparing to blur..." << std::endl;

	for (int i = 0; i < threadsCount; i++)
	{
		Args args;

		args.unblurred = &bmpToBlur;
		args.blurred = &blurred;
		args.start = minWidth;
		args.end = i == threadsCount - 1 ? bmpToBlur.GetWidth() - 1 : minWidth + width;

		argsToPass.push_back(args);

		minWidth += width;
	}

    std::mutex iomutex;
    std::vector<std::thread> threads(threadsCount);
    for (unsigned i = 0; i < threadsCount; ++i) 
    {
        threads[i] = std::thread([&iomutex, i, coresCount, &argsToPass] 
        {
            {
                // Use a lexical scope and lock_guard to safely lock the mutex only for
                // the duration of std::cout usage.
                std::lock_guard<std::mutex> iolock(iomutex);
                std::cout << "Thread #" << i << ": on CPU " << GetCurrentCPUNumber() << std::endl;
            }

            ThreadProc(&argsToPass[i]);
        });

        // Create a cpu_set_t object representing a set of CPUs. Clear it and mark
        // only CPU i as set.
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        const int coreNumber = i % coresCount;
        CPU_SET(i, &cpuset);

        const int err = mac_setaffinity(&cpuset);

        if (err != 0)
        {
            std::cerr << "Error calling pthread_setaffinity_np: " << err <<  "\n";
        }
    }

    for (auto& t : threads) 
    {
        t.join();
    }

    std::cout << "Successfully blurred." << std::endl;

    auto blurringFinishTime = std::chrono::high_resolution_clock::now();

    std::cout << "Writing in " + outFileName << std::endl;

    BMPUtils::WriteToPath(&blurred, outFileName);

    std::cout << "Successfully write." << std::endl;

    auto blurringTime = duration_cast<std::chrono::milliseconds>(blurringFinishTime - blurringStartTime);

    std::cout << "Blurring is done in " << blurringTime.count() << " ms." << std::endl;
}