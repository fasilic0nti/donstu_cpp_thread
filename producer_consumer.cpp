#include <fstream>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>

std::mutex              m;
std::condition_variable cv;

int  data  = 0;
bool ready = false;
bool done  = false;

std::ofstream logFile("output.log", std::ios::app);
std::mutex    logMutex;

void writeLine(const std::string& msg) {
  std::lock_guard<std::mutex> lock(logMutex);
  logFile << msg << "\n";
  logFile.flush();
}

void producer() {
  for (int i = 0; i < 10; ++i) {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, []{ return !ready; });
    data  = i;
    ready = true;
    writeLine("producer: produced " + std::to_string(i));
    cv.notify_one();
  }
  std::lock_guard<std::mutex> lock(m);
  done = true;
  cv.notify_one();
}

void consumer() {
  while (true) {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, []{ return ready || done; });
    if (done && !ready) break;
    writeLine("consumer: consumed " + std::to_string(data));
    ready = false;
    cv.notify_one();
  }
}

int main() {
  std::thread p(producer);
  std::thread c(consumer);
  p.join();
  c.join();
  writeLine("main: done");
  return 0;
}
