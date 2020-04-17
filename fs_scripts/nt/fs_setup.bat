@echo off

if exist "%USERPROFILE%\darkforest_disk.vhdx" (
    diskpart /s fs_scripts\nt\attach_vdisk.diskpart
    exit /b
)

diskpart /s fs_scripts\nt\create_vdisk.diskpart
mklink /d mnt R:\
