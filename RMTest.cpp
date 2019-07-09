//================================================================================
// Author: Nicholas T. Smith
// File:   RMTest.h
// Desc:   Unit tests for RangeMap
//================================================================================
#include <chrono>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <cstdlib>
#include "RangeMap.h"

using namespace std;

#define RUN_TEST(T) rv = RunTest<T>(MAXA, nt, tSum1, tSum2); cout << "Test:    " #T << endl; cout << "Result:  " << (rv ? "PASS" : "FAIL") << endl; cout << "Time Elapsed:\n\tBrute Force: " << tSum2.count() << "\n\tRangeMap: " << tSum1.count() << endl

/* Brute force approach for determining intervals that contain
 * a query point.
 * x:   Query point
 * S:   Interval starts
 * E:   Interval endpoints
 * N:   Number of intervals
 * Ret: Vector of indices of intervals containing the query point*/
template <typename T>
vector<size_t> SlowCheck(const T x, const T* S, const T* E, const size_t N) {
    vector<size_t> s;
    for (size_t i = 0; i < N; ++i) {
        if ((S[i] <= x) && (x < E[i]))
            s.push_back(i);
    }
    return s;
}

template <typename T>
bool RunTest(const int MAXA, const int nt, std::chrono::duration<double>& tSum1, std::chrono::duration<double>& tSum2) {
    // Timings
    tSum1 = std::chrono::duration<double> {};
    tSum2 = std::chrono::duration<double> {};
    std::chrono::time_point<std::chrono::system_clock> st;
    for (int k = 0; k < nt; ++k) {      // Loop over each random test case
        int ni = (rand() % 99) + 1;     // Generate a random test case
        vector<T> S(ni);
        vector<T> E(ni);
        for (int j = 0; j < ni; ++j) {
            int a = (rand() % MAXA);
            int b = (rand() % (MAXA - a)) + a;
            S[j] = a;
            E[j] = b;
        }
        // Build a RangeMap
        RangeMap<T> rm;
        rm.Build(S.data(), E.data(), ni);

        // Get min and max values for testing ranges
        T n = S[0], x = E[0];
        for (int i = 1; i < ni; ++i) {
            if (S[i] < n)
                n = S[i];
            if (E[i] > x)
                x = E[i];
        }
        // Test absolute maximum value
        {
            T maxv = numeric_limits<T>::max();

            st = std::chrono::system_clock::now();
            auto s1 = rm.Query(maxv);
            tSum1 += (std::chrono::system_clock::now() - st);

            st = std::chrono::system_clock::now();
            auto s2 = SlowCheck<T>(maxv, S.data(), E.data(), ni);
            tSum2 += (std::chrono::system_clock::now() - st);

            if (s1 != s2)
                return false;
        }
        // Test absolute minimum value
        {
            T minv = numeric_limits<T>::min();

            st = std::chrono::system_clock::now();
            auto s1 = rm.Query(minv);
            tSum1 += (std::chrono::system_clock::now() - st);

            st = std::chrono::system_clock::now();
            auto s2 = SlowCheck<T>(minv, S.data(), E.data(), ni);
            tSum2 += (std::chrono::system_clock::now() - st);

            if (s1 != s2)
                return false;
        }
        // Test from (min - 1) to (max + 1)
        for (T i = (n - 1); i <= (x + 1); ++i) {
            st = std::chrono::system_clock::now();
            auto s1 = rm.Query(i);
            tSum1 += (std::chrono::system_clock::now() - st);

            st = std::chrono::system_clock::now();
            auto s2 = SlowCheck<T>(i, S.data(), E.data(), ni);
            tSum2 += (std::chrono::system_clock::now() - st);

            if (s1 != s2)
                return false;
        }
    }
    return true;
}

int main() {
    // Maximum value in interval
    const int MAXA = 1000;
    const int nt = 333;
    bool rv;
    std::chrono::duration<double> tSum1, tSum2;
    srand(time(nullptr));

    cout << "Test Count: " << nt << endl;

    RUN_TEST(int);
    RUN_TEST(double);
    RUN_TEST(unsigned int);

    return 0;
}