set pagination off
set confirm off
file G:/PEEPSHOW firmwares/PEEPSHOW_HW5_FW1/firmware/peepshow_hw5_fw0/build/Debug/peepshow_hw5_fw0.elf
target remote localhost:61234
info registers pc lr sp xpsr
info breakpoints
detach
quit
