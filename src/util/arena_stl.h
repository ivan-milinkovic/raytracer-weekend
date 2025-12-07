#ifndef arena_stl_h
#define arena_stl_h

#include "arena.h"

template <typename T>
class ArenaSTL {
private:
    Arena* arena;
    
public:
    using value_type = T;
    
    explicit ArenaSTL(Arena* arena) : arena(arena) { }
    
    T* allocate(std::size_t size) {
        return arena->allocate<T>(size);
    }
    
    void deallocate(T*, std::size_t) noexcept {
        // nothing, arena doesn't deallocate individual objects
    }
    
    template <typename U>
    bool operator==(const ArenaSTL<U>& other) const {
        return arena == other.arena;
    }
    
    template <typename U>
    bool operator!=(const ArenaSTL<U>& other) const {
        return arena != other.arena;
    }
};

#endif /* arena_stl_h */
