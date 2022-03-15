unsigned int systemCall(unsigned int func, unsigned int arg1=0, unsigned int arg2=0, unsigned int arg3=0) {
    unsigned int return_value = 0;
    asm volatile("pusha");
    asm volatile("" : : "a"(func), "b"(arg1), "c"(arg2), "d"(arg3));
    asm volatile("int $0x40");
    asm volatile("" : "=a"(return_value) : :);
    asm volatile("popa");
    return return_value;
}

void print(const char* text) {
    systemCall(0, (unsigned int)text);
}

int main() {
    print("Hello, World!\n");
    
    return 0;
}
