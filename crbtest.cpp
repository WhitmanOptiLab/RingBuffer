#include "compound_rb.hpp"

#include <vector>
#include <cassert>
#include <iostream>

int main() {
    std::vector<double> delays = {1.0, 2.0, 3.0, 4.0};

    CompoundRingBuffer<double> history(4, 0.0, delays, std::vector<double>(4));

    for (int i = 1; i < 10; i++) {
        history.update( (double) i, std::vector<double>(4, (double)i));
        for (int j = 0; j < std::min(i, 4); j++) {
            size_t tid = history.bisect(i-(j+0.5));
            assert(tid == j+1);
            for (int n = 0; n < 4; n++) {
                if (tid <= delays[n]) {
                    assert(history[n][tid] == (double)(i-tid));
                }
            }
        }
    }

    return 0;
}