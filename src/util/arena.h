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
        return allocate<T>(1);
    }
    
    template<class T>
    T* allocate(std::size_t count) {
        auto size = sizeof(T) * count;
        auto alignment = alignof(T);
        
        // todo: use std::align
        
        auto new_loc = current;
        auto remainder = new_loc % alignment;
        if(remainder > 0) {
            new_loc += (alignment - remainder);
        }
        
        auto new_offset = new_loc + size;
        if(new_offset >= end) {
            throw std::bad_alloc();
        }
        current = new_offset;
        
        return reinterpret_cast<T*>(new_loc);
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
