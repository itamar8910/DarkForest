KERNEL_DIR = kernel

.PHONY: all kernel

all: kernel

kernel:
	make -C $(KERNEL_DIR)/
