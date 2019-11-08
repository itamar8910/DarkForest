

int func() {
    asm volatile(
        // "1:\n"
        "mov $3, %eax\n"
        "int $0x80\n"
        // "jmp 1b\n"
    );
    return 0;
}

int main() {
    return func();
}