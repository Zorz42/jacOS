#pragma once
#include "memory/memory.hpp"
#include "text/text.hpp"

#define _BLOCK_SIZE 16

template<class Type>
class Array {
    Type *data;
    unsigned int size, physical_size;
    
    void resize(unsigned int new_size) {
        if(new_size > physical_size) {
            unsigned int num_new_blocks = (new_size - physical_size - 1) / _BLOCK_SIZE + 1;
            physical_size += num_new_blocks * _BLOCK_SIZE;
            Type* new_data = new Type[physical_size];
            for(int i = 0; i < size; i++)
                new_data[i] = data[i];
            delete data;
            data = new_data;
        }
        
        if(new_size > size)
            for(int i = size; i < new_size; i++)
                data[i] = Type();
        else if(new_size < size)
            for(int i = new_size; i < size; i++)
                data[i].~Type();
        
        size = new_size;
    }
public:
    Array() {
        size = 0;
        physical_size = 0;
        data = new Type[0];
    }
    
    Type& operator[](int index) {
        return data[index];
    }
    
    Array(Array<Type>& array) {
        size = array.size;
        physical_size = array.physical_size;
        data = new Type[physical_size];
        for(int i = 0; i < size; i++)
            data[i] = array[i];
    }
    
    unsigned int getSize() {
        return size;
    }
    
    void erase(unsigned int start, unsigned int end) {
        for(int i = start; i < size - (end - start); i++) {
            data[i] = data[i + end - start];
        }
        resize(size - (end - start));
    }
    
    void erase(unsigned int index) {
        erase(index, index + 1);
    }
    
    void insert(Array<Type> array, unsigned int index) {
        resize(size + array.getSize());
        for(int i = size - 1; i >= index + array.getSize(); i--)
            data[i] = data[i - array.getSize()];
        for(int i = 0; i < array.getSize(); i++)
            data[index + i] = array[i];
    }
    
    void insert(const Type& item, unsigned int index) {
        resize(size + 1);
        
        for(int i = size - 1; i > index; i--)
            data[i] = data[i - 1];
        
        data[index] = item;
    }
    
    void push(const Type& item) {
        insert(item, size);
    }
    
    void push() {
        push(Type());
    }
    
    void pop() {
        erase(size - 1);
    }
    
    unsigned int find(const Type& item) {
        for(int i = 0; i < size; i++)
            if(item == data[i])
                return i;
        return size;
    }
    
    bool contains(const Type& item) {
        return find(item) != size;
    }
    
    void clear() {
        erase(0, size);
    }
    
    ~Array() {
        delete data;
    }
};
