//================================================================================
// Author: Nicholas T. Smith
// File:   RangeMap.h
// Desc:   Class for determining intervals that contain a query point
//================================================================================
#ifndef RANGE_MAP_H
#define RANGE_MAP_H
#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <set>
#include <vector>

// Macro to help ensure pre-allocation sizes are correct
#define PUSHBACK(X, Y) assert(X.capacity() > X.size()); X.push_back(Y)
// Hack to get -std::numeric_limits<T>::infinity() to compile for integral types
#define NEG_INFTY ((1 - 2 * std::numeric_limits<T>::has_infinity) * std::numeric_limits<T>::infinity())
#define POS_INFTY std::numeric_limits<T>::infinity()

/* Class for solving the following problem:
 * Given a list L of intervals of the form [a, b) and a point p,
 * determine the set of all intervals in L that contain p. */
template <typename T>
class RangeMap {
    std::vector<T> Tab;                         // Internal table for searching intervals
    std::vector<std::vector<size_t> > IList;    // Internal list containing sets
    const std::vector<size_t> Empty;            // Convenience member for returning empty sets

public:
    /* Builds the RangeMap from a list of intervals like [a, b)
     * S:   An array of interval starting values
     * E:   An array of interval closing values
     * N:   The number of elements in S and E */
    void Build(const T* S, const T* E, const size_t N) {
        if (nullptr == S || nullptr == E || N == 0)
            return;
        // Argsort filtering any empty intervals
        std::vector<size_t> SS;
        std::vector<size_t> SE;
        SS.reserve(N);
        SE.reserve(N);
        for (size_t i = 0; i < N; ++i) {
            if (S[i] != E[i]) { // [a, a) is an empty interval
                PUSHBACK(SS, i);
                PUSHBACK(SE, i);
            }
        }
        const size_t NF = SS.size();    // Number of filtered values
        // Argsort starting and ending intervals
        std::sort(SS.begin(), SS.end(), [&S](size_t i1, size_t i2) { return S[i1] < S[i2]; });
        std::sort(SE.begin(), SE.end(), [&E](size_t i1, size_t i2) { return E[i1] < E[i2]; });
        // Clear and reserve space
        Tab.clear();
        IList.clear();
        Tab.reserve(NF * 2 + 2);     // 1 for each start/end + 2 for -inf and +inf
        IList.reserve(NF * 2 + 2);   // 1 element for each above
        constexpr T MINV = std::numeric_limits<T>::has_infinity ? NEG_INFTY : std::numeric_limits<T>::min();
        constexpr T MAXV = std::numeric_limits<T>::has_infinity ? POS_INFTY : std::numeric_limits<T>::max();
        // Record number of intervals satisfied at each start and end point
        size_t i1 = 0, i2 = 0;  // Start point index, end point index
        std::set<size_t> AS;    // Active set
        // Starting point interval; catch everything below lowest start point
        if (0 == NF || S[SS[0]] > MINV) {
            PUSHBACK(Tab, MINV);
            PUSHBACK(IList, Empty);
        }
        while ((i1 < NF) || (i2 < NF)) {    // Loop over each value start and end point
            // The next value to merge into the table
            const T& v = ((i1 >= NF) || ((i2 < NF) && (S[SS[i1]] >= E[SE[i2]]))) ? E[SE[i2]] : S[SS[i1]];
            // Update the active set with intervals opening and closing at this point
            while ((i1 < NF) && (S[SS[i1]] == v))
                AS.insert(SS[i1++]);    // These intervals are opening
            while ((i2 < NF) && (E[SE[i2]] == v))
                AS.erase(SE[i2++]);     // These intervals are closing
            // Active set guaranteed to have changed; record new interval ending here
            PUSHBACK(Tab, v);
            PUSHBACK(IList, std::vector<size_t>(AS.begin(), AS.end()));
            assert(Tab.size() < 2 || Tab[Tab.size() - 1] != Tab[Tab.size() - 2]);
            assert(IList.size() < 2 || IList[IList.size() - 1] != IList[IList.size() - 2]);
        }
        // Endpoint interval; catch everything above largest end point
        if (0 == NF || E[SE[NF - 1]] < MAXV) {
            PUSHBACK(Tab, MAXV);
            PUSHBACK(IList, Empty);
        }
    }

    /* Given a query point, returns all intervals containing the point
     * p:       The query point
     * Return:  A vector of all intervals containing the point */
    const std::vector<size_t>& Query(const T& p) const {
        auto It = std::lower_bound(Tab.begin(), Tab.end(), p);
        // IList[x] contains the result, where x is the largest index such that Tab[x] <= p
        return (It != Tab.end()) ? IList[(It - Tab.begin()) - (*It > p)] : Empty;
    }
};

#endif