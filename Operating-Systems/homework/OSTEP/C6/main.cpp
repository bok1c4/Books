#include <iostream>
#include <sched.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void measureSystemCallCost() {
  const int iterations = 1000000;
  timeval start{}, end{};

  // Start timer
  gettimeofday(&start, nullptr);

  for (int i = 0; i < iterations; i++) {
    read(0, nullptr, 0); // Simple system call
  }

  // End timer
  gettimeofday(&end, nullptr);

  // Calculate elapsed time in microseconds
  long elapsed =
      (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
  std::cout << "Average system call cost: " << (double)elapsed / iterations
            << " microseconds\n";
}

void measureContextSwitchCost() {
  const int iterations = 100000;
  int pipe1[2], pipe2[2];

  // Create pipes
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    std::cerr << "Pipe creation failed\n";
    return;
  }

  pid_t pid = fork();
  if (pid < 0) {
    std::cerr << "Fork failed\n";
    return;
  }

  if (pid == 0) {
    // Child process
    char msg;
    for (int i = 0; i < iterations; i++) {
      read(pipe1[0], &msg, 1);  // Read from parent
      write(pipe2[1], &msg, 1); // Write to parent
    }
    exit(0);
  } else {
    // Parent process
    char msg = 'x';
    timeval start{}, end{};

    // Start timer
    gettimeofday(&start, nullptr);

    for (int i = 0; i < iterations; i++) {
      write(pipe1[1], &msg, 1); // Write to child
      read(pipe2[0], &msg, 1);  // Read from child
    }

    // End timer
    gettimeofday(&end, nullptr);

    // Wait for child process to finish
    wait(nullptr);

    // Calculate elapsed time in microseconds
    long elapsed =
        (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    std::cout << "Average context switch cost: "
              << (double)elapsed / (iterations * 2) << " microseconds\n";
  }
}

int main() {
  std::cout << "Starting performance measurements...\n";

  measureSystemCallCost();
  measureContextSwitchCost();

  return 0;
}

