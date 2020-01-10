# DarkForest Operating System

A work in progress x86 operating system

## Current features

- Preemptive Multiprocessing
- Basic round-robin scheduler
- Virtual Memory management
- Heap memory management
- Interrupts
- Keyboard driver
- Tar RamDisk filesystem
- Partial elf parser
- Userspace

## Coming up
- userspace shell

## Build prerequisites

### On debian
```bash
apt-get install gcc g++ make curl libmpc-dev nasm qemu-system-i386
```

## Build toolchain
```bash
cd toolchain
./build.sh full
cd ..
export LOCATION_OF_DARKFOREST_REPO=$HOME/DarkForest # change this to where you have placed the repo
export PATH="$LOCATION_OF_DARKFOREST_REPO/toolchain/cross/bin:$PATH"
```

## Build DarkForest
```bash
make
```

## Run
```bash
./run.sh
```


I take a lot of inspiration from Andreas Kling's amazing [SerenityOS](https://github.com/SerenityOS/serenity) - check it out!


