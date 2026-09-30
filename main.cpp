#include <iostream>
#include <vector>
#include <thread>
#include <sstream>
#include "threadfuncs.h"
#include <unistd.h>
int main() {
  std::thread::id a = std::this_thread::get_id();
  std::thread::id b = std::this_thread::get_id();
  bool same = (a == b);
  about();

  // Open log file
  Logger logger("output.log");

  std::ostringstream oss; 
  oss << "main: pid = " << ::getpid() << ", tid = " << getThreadID() <<", opened file: 'output.log'";
  logger.writeLine(oss.str());

  

  // args for threads
  std::vector<ThreadArgs> args(COUNT_THREADS);
  for (int i = 0; i < COUNT_THREADS; ++i) {
	std::ostringstream oss;
	oss << "T" << i;
	args[i].id = i + 1;
	args[i].tag = oss.str();}

  // thread are starting
  std::vector<std::thread> threads;
  threads.reserve(COUNT_THREADS);

  for (int i = 0; i < COUNT_THREADS; ++i) {
    threads.emplace_back(funcThread, std::cref(args[i]), std::ref(logger));
  }

  // wait for stop all threads
  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  // close file automatically
  std::cout << "main: all threads finished, file closed\n";
  return 0;
}
