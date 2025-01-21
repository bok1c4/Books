// 1. Run a few randomly-generated problems with just two jobs and
// two queues; compute the MLFQ execution trace for each. Make
// your life easier by limiting the length of each job and turning off
// I/Os.
//
// 2. How would you run the scheduler to reproduce each of the exam-
// ples in the chapter?
//
// 3. How would you configure the scheduler parameters to behave just
// like a round-robin scheduler?
//
// 4. Craft a workload with two jobs and scheduler parameters so that
// one job takes advantage of the older Rules 4a and 4b (turned on
// with the -S flag) to game the scheduler and obtain 99% of the CPU
// over a particular time interval.
//
// 5. Given a system with a quantum length of 10 ms in its highest queue,
// how often would you have to boost jobs back to the highest priority
// level (with the -B flag) in order to guarantee that a single long-
// running (and potentially-starving) job gets at least 5% of the CPU?

// mlfq is priority queue scheduler
// mlfq has queues based on priorities
// and we are pushing the jobs based on the priority of the job

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include <vector>

struct Job {
  int id;
  int arrival_time;
  int elapsed_time;
  int remaining_time;
  int priority;

  bool operator<(const Job &other) const { return priority > other.priority; }
};

void generate_jobs(std::queue<Job> queues[]) {
  for (int i = 0; i < 2; i++) {
    Job new_job;
    new_job.id = i;
    new_job.priority = i;
    new_job.arrival_time = std::time(0);
    new_job.elapsed_time = 0;
    new_job.remaining_time = 10 + (std::rand() % 91);

    queues[i].push(new_job);
  }
}

void task_1() {
  std::queue<Job> queues[2];
  generate_jobs(queues);

  for (int i = 0; i < 2; i++) {
    while (!queues[i].empty()) {
      Job job = queues[i].front();
      queues[i].pop();
      std::cout << "Job ID: " << job.id << ", Priority: " << job.priority
                << ", Arrival Time: " << job.arrival_time
                << ", Remaining Time: " << job.remaining_time << "ms\n";
    }
  }
}

int main() {
  std::srand(std::time(0));
  task_1();
  return 0;
}
