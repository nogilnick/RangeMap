# RangeMap
A C++ class for efficiently solving the _interval query_ problem.

__Interval Query__

Given a list _L_ of intervals of the form \[_a_, _b_) and a point _p_, determine the set of all intervals in _L_ that contain _p_.

## Usage
```cpp
// ...
#include "RangeMap.h"

int main() {
  // Interval arrays of start (S) and end (E) points
  int S[] = {0, 5, 10, 15};
  int E[] = {8, 7, 13, 25};
  
  RangeMap<int> rm;
  rm.Build(S, E, 4);
  
  auto qr = rm.Query(20);
  
  return 0;
}
