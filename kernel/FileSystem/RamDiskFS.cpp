#include "RamDiskFS.h"
#include "logging.h"

RamDiskFS::RamDiskFS(multiboot_info_t& mbt)
: FileSystem("/initrd") {
	ASSERT(mbt.mods_count == 1);
	multiboot_module_t* ramdisk_module = (multiboot_module_t*) mbt.mods_addr;
	kprintf("mod_start: 0x%x, mod_end: 0x%x\n", ramdisk_module->mod_start, ramdisk_module->mod_end);
	m_base = (void*) ramdisk_module->mod_start;
	m_size = ramdisk_module->mod_end - ramdisk_module->mod_start + 1;
}
