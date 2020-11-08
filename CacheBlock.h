//
// Created by nima on 07/11/2020.
//

#ifndef MULTICORE_CACHEBLOCK_H
#define MULTICORE_CACHEBLOCK_H

#include <bitset>

using namespace std;

typedef bitset<2> State;
typedef unsigned int uint;

/*State in 2 bits
- 0 : Invalid
- 1 : Exclusive
- 2 : Shared
- 3 : Modified
 */

class CacheBlock {
public:
    CacheBlock();
    CacheBlock(State state, uint tag);
    State state;
    uint tag{};
    bool operator == (const CacheBlock& s) const { return state == s.state && tag == s.tag; }
    bool operator != (const CacheBlock& s) const { return !operator==(s); }
    void changeState(State new_state);
};


#endif //MULTICORE_CACHEBLOCK_H
