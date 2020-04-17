@echo off

diskpart /s fs_scripts\nt\detach_vdisk.diskpart

del %USERPROFILE%\darkforest_disk.vhdx

