// Questions
// 1. Compute the response time and turnaround time when running
// three jobs of length 200 with the SJF and FIFO schedulers.
//
// 2. Now do the same but with jobs of different lengths: 100, 200, and
// 300.
//
// 3. Now do the same, but also with the RR scheduler and a time-slice
// of 1.
//
// 4. For what types of workloads does SJF deliver the same turnaround
// times as FIFO?
//
// 5. For what types of workloads and quantum lengths does SJF deliver
// the same response times as RR?
//
// 6. What happens to response time with SJF as job lengths increase?
// Can you use the simulator to demonstrate the trend?
//
// 7. What happens to response time with RR as quantum lengths in-
// crease? Can you write an equation that gives the worst-case re-
// sponse time, given N jobs?

#include <bits/types/struct_timeval.h>
#include <ctime>
#include <iostream>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include <thread>

void task1() {

  // response_time = first_run - arrival
  // turnaround_time = completion - arrival

  std::queue<int> q;

  int timeout = 200;
  timeval arrival{}, completion{};
  timeval first_run{};

  for (int i = 0; i < 3; i++) {
    q.push(i);
    gettimeofday(&arrival, nullptr);
    gettimeofday(&first_run, nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(timeout));

    gettimeofday(&completion, nullptr);

    // Calculate response time and turnaround time
    long response_time = (first_run.tv_sec - arrival.tv_sec) * 1000 +
                         (first_run.tv_usec - arrival.tv_usec) / 1000;
    long turnaround_time = (completion.tv_sec - arrival.tv_sec) * 1000 +
                           (completion.tv_usec - arrival.tv_usec) / 1000;

    std::cout << "Job " << i << ":\n";
    std::cout << "  Arrival time: " << arrival.tv_sec << "." << arrival.tv_usec
              << "\n";
    std::cout << "  Completion time: " << completion.tv_sec << "."
              << completion.tv_usec << "\n";
    std::cout << "  Response time: " << response_time << " ms\n";
    std::cout << "  Turnaround time: " << turnaround_time << " ms\n";
  }

  return;
}

int main(int argc, char *argv[]) {
  task1();
  return 0;
}
