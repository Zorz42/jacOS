void kernelMain();

extern "C" void start() {
    kernelMain();
}
