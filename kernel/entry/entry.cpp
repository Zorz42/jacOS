void kernelMain();

extern "C" void kernelEntry() {
    kernelMain();
}
