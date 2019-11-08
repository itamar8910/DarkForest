

int main() {
    asm volatile(
        "mov $3, %eax\n"
        "int $0x80\n"
    );
    return 0;
}