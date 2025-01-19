#include <bits/types/struct_timeval.h>
#include <vector>

struct Node {
  struct Data {
    int timeout;

    timeval arrival{};
    timeval completion{};
    timeval first_run{};

    int response_time;
    int turnaround_time;

    Data(int timeout)
        : timeout(timeout), response_time(-1), turnaround_time(0) {}
  };

  Data data;
  Node *next;

  Node(const int &timeout) : data(timeout), next(nullptr) {}
};

typedef std::vector<Node *> NodeVector;
