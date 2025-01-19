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

// response_time = first_run - arrival
// turnaround_time = completion - arrival

#include <bits/types/struct_timeval.h>
#include <ctime>
#include <iostream>
#include <queue>
#include <sys/select.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

void task1() {

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

void task2() {
  struct Node {
    struct Data {
      int timeout;

      timeval arrival{};
      timeval completion{};
      timeval first_run{};

      int response_time;
      int turnaround_time;

      Data(int timeout)
          : timeout(timeout), response_time(0), turnaround_time(0) {}
    };

    Data data;
    Node *next;

    Node(const int &timeout) : data(timeout), next(nullptr) {}
  };

  Node *head = nullptr;
  Node *tail = nullptr;

  for (int timeout : {100, 200, 300}) {
    Node *newNode = new Node(timeout);

    if (!head) {
      head = newNode;
    } else {
      tail->next = newNode;
    }
    tail = newNode;
  }

  Node *current = head;
  timeval current_time;

  while (current) {
    gettimeofday(&current->data.arrival, nullptr);

    usleep(current->data.timeout * 1000);
    gettimeofday(&current->data.first_run, nullptr);

    usleep(current->data.timeout * 1000);
    gettimeofday(&current->data.completion, nullptr);

    current->data.response_time =
        (current->data.first_run.tv_sec - current->data.arrival.tv_sec) * 1000 +
        (current->data.first_run.tv_usec - current->data.arrival.tv_usec) /
            1000;

    current->data.turnaround_time =
        (current->data.completion.tv_sec - current->data.arrival.tv_sec) *
            1000 +
        (current->data.completion.tv_usec - current->data.arrival.tv_usec) /
            1000;

    std::cout << "-------------------------\n";
    std::cout << "Job Timeout: " << current->data.timeout << "ms\n";
    std::cout << "Arrival Time: " << current->data.arrival.tv_sec << "s "
              << current->data.arrival.tv_usec << "us\n";
    std::cout << "First Run Time: " << current->data.first_run.tv_sec << "s "
              << current->data.first_run.tv_usec << "us\n";
    std::cout << "Completion Time: " << current->data.completion.tv_sec << "s "
              << current->data.completion.tv_usec << "us\n";
    std::cout << "Response Time: " << current->data.response_time << "ms\n";
    std::cout << "Turnaround Time: " << current->data.turnaround_time << "ms\n";

    current = current->next;
  }

  current = head;
  while (current) {
    Node *temp = current;
    current = current->next;
    delete temp;
  }

  return;
}

int main(int argc, char *argv[]) {
  task1();
  task2();
  return 0;
}
