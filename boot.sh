make

if [[ "$1" == "debug" ]]; then
    qemu-system-i386 \
        -drive id=disk,file=build/boot.img,format=raw,index=0,media=disk,if=none \
        -device ide-hd,drive=disk,bus=ide.0 \
        -nic none -m 4G -s -S &
    gdb -x gdb/gdb-cmd
else
    qemu-system-x86_64 \
        -drive id=disk,file=build/boot.img,format=raw,index=0,media=disk,if=none \
        -device ide-hd,drive=disk,bus=ide.0 \
        -nic none -m 4G
fi