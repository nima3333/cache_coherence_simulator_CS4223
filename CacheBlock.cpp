//
// Created by nima on 07/11/2020.
//

#include "CacheBlock.h"

CacheBlock::CacheBlock() {

}

CacheBlock::CacheBlock(State state, uint tag) {
    this->state = state;
    this->tag = tag;
}

void CacheBlock::changeState(State new_state) {
    this->state = new_state;
}
