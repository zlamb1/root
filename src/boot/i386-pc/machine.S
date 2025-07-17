.code32
.section .stage2.util

.global root_reboot

root_reboot:
    call enter_real
.code16
    ljmp $0xFFFF, $0x0