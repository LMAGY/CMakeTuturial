//
// Created by liuyu on 2023/6/1.
//

#ifndef MEMORY_POOL_MEMORYPOOL_HPP
#define MEMORY_POOL_MEMORYPOOL_HPP

#include <climits>
#include <cstddef>

template<typename T, size_t BlockSize = 4096 ,size_t MaxTotalSize = UINT_MAX>
class MemoryPool {
public:
    typedef T *  pointer;

    template <typename U> struct rebind {
        typedef MemoryPool<U> other;
    };

    MemoryPool() noexcept {
        currentBlock_ = nullptr; //当前内存块
        currentSlot_ = nullptr; //当前内存块的当前可用内存起始位置
        lastSlot_ = nullptr;  //当前内存块的当前可用内存结束位置
        freeSlots_ = nullptr; //当前内存块的当前可用内存链表
    }

    ~MemoryPool() noexcept {
        slot_pointer_ curr = currentBlock_;
        while (curr != nullptr) {
            slot_pointer_ prev = curr->next;
            operator delete(reinterpret_cast<void *>(curr));
            curr = prev;
        }
    }

    pointer allocate(size_t n = 1, const T* hint = 0) //每次只会分配一个对象出来
    {
        if(freeSlots_ != nullptr)
        {
            pointer result = reinterpret_cast<pointer>(freeSlots_);
            freeSlots_ = freeSlots_->next;
            return result;
        }
        else
        {
            if(currentSlot_ >= lastSlot_)
            {
                data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(BlockSize));
                reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
                currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
                data_pointer_ body = newBlock + sizeof(slot_pointer_);
                uintptr_t result = reinterpret_cast<uintptr_t>(body);
                size_t bodyPadding = (alignof(slot_type_) - result) % alignof(slot_type_);
                currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
                lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + BlockSize - sizeof(slot_type_) + 1);
            }
            return reinterpret_cast<pointer>(currentSlot_++);
        }

    }

    void deallocate(pointer p, size_t n = 1){
        if (p != nullptr) {
            reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
            freeSlots_ = reinterpret_cast<slot_pointer_>(p);
        }

    }


    template<typename U, typename... Args>
    void construct(U *p, Args &&... args)
    {
        new(p) U(std::forward<Args>(args)...);
    }


    template<typename U>
    void destroy(U *p){
        p->~U();
    }

private:
    // 用于存储内存池中的对象槽,
    // 要么被实例化为一个存放对象的槽,
    // 要么被实例化为一个指向存放对象槽的槽指针
    union Slot_ {
        T element;
        Slot_* next;
    };

    //数据指针
    typedef char* data_pointer_;
    //对象槽
    typedef Slot_ slot_type_;
    //对象槽指针
    typedef Slot_* slot_pointer_;

    // 指向当前内存区块
    slot_pointer_ currentBlock_;
    // 指向当前内存区块的对象槽
    slot_pointer_ currentSlot_;
    // 指向当前内存区块的最后一个对象槽
    slot_pointer_ lastSlot_;
    // 指向当前内存区块中的空闲对象槽
    slot_pointer_ freeSlots_;


    //检查定义的内存池大小是否过小
    static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");


};

#endif //MEMORY_POOL_MEMORYPOOL_HPP
