//
//  Arena.h
//  RaytracerWeekend
//
//  Created by Ivan Milinkovic on 22. 11. 2025.
//

#ifndef arena_h
#define arena_h

class Arena {

public:
    Arena(size_t size) {
        this->size = size;
        this->memory = new uint8_t[size];
        this->current = reinterpret_cast<uintptr_t>(memory);
        this->end = reinterpret_cast<uintptr_t>(memory + size);
    }
    
    ~Arena() {
        size = 0;
        current = 0;
        delete memory;
        memory = nullptr;
    }
    
    template<class T>
    T* allocate() {
        auto size = sizeof(T);
        auto alignment = alignof(T);
        
        // todo: use std::align
        
        auto new_loc = current + size;
        auto remainder = new_loc % alignment;
        if(remainder > 0) {
            new_loc += (alignment - remainder);
        }
        
        if(new_loc >= end) {
            throw std::bad_alloc();
        }
        
        current = new_loc;
        
        return reinterpret_cast<T*>(current);
    }
    
    void reset() {
        current = reinterpret_cast<uintptr_t>(memory);
    }
    
private:
    uint8_t* memory;
    size_t size;
    uintptr_t current;
    uintptr_t end;
};

#endif /* arena_h */
