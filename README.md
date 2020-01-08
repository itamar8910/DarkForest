# DarkForest Operating System

A work in progress x86 operating system

## Current features:

- Preemptive Multiprocessing
- Basic round-robin scheduler
- Virtual Memory management
- Heap memory management
- Interrupts
- Keyboard driver
- Tar RamDisk filesystem
- Partial elf parser
- Userspace
- ATA Disk driver (currently read only)
- Fat32 (currently read only)

## Coming up:
- userspace shell

## build
```bash
cd toolchain
./build.sh
cd ..
export PATH="$HOME/DarkForest/toolchain/cross/bin:$PATH"
./fs2_setup.sh # needs root priv to mount fs
make
```

## run
```bash
./run
```


I take a lot of inspiration from Andreas Kling's amazing [SerenityOS](https://github.com/SerenityOS/serenity) - check it out!

