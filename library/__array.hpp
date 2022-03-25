#pragma once

#define _BLOCK_SIZE 16

template<class Type>
class Array {
    Type *data;
    unsigned int size, physical_size;
public:
    Array() {
        size = 0;
        physical_size = 0;
        data = nullptr;
    }
    
    Type& operator[](int index) const {
        return data[index];
    }
    
    void operator=(const Array<Type>& array) {
        size = array.size;
        physical_size = array.physical_size;
        data = new Type[physical_size];
        for(int i = 0; i < size; i++)
            data[i] = array[i];
    }
    
    Array(const Array<Type>& array) {
        *this = array;
    }
    
    void reserve(unsigned int reserved_size) {
        reserved_size = (reserved_size + _BLOCK_SIZE - 1) / _BLOCK_SIZE * _BLOCK_SIZE;
        
        if(reserved_size > physical_size) {
            unsigned int num_new_blocks = reserved_size - physical_size;
            physical_size += num_new_blocks;
            Type* new_data = new Type[physical_size];
            for(int i = 0; i < size; i++)
                new_data[i] = data[i];
            delete data;
            data = new_data;
        }
    }
    
private:
    void resize(unsigned int new_size) {
        if(new_size > physical_size)
            reserve((new_size + _BLOCK_SIZE - 1) / _BLOCK_SIZE * _BLOCK_SIZE);
        
        if(new_size > size)
            for(int i = size; i < new_size; i++)
                data[i] = Type();
        else if(new_size < size)
            for(int i = new_size; i < size; i++)
                data[i].~Type();
        
        size = new_size;
    }
    
public:
    unsigned int getSize() const {
        return size;
    }
    
    void erase(unsigned int start, unsigned int end) {
        for(int i = start; i < size - (end - start); i++) {
            data[i] = data[i + (end - start)];
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
    
    unsigned int find(const Type& item) const {
        for(int i = 0; i < size; i++)
            if(item == data[i])
                return i;
        return size;
    }
    
    bool contains(const Type& item) const {
        return find(item) != size;
    }
    
    void clear() {
        erase(0, size);
    }
    
    ~Array() {
        delete data;
    }
};
