#!/bin/bash
echo > out.bin
gcc descriptor.c -I ./include/ -D__code= \
	-Tdescr.ld -nostdlib -Wl,--build-id=none -fdata-sections \
	-odescr.elf

append_section(){
    objcopy -j .data.$1 -Obinary descr.elf tmp.bin
    LEN=`stat --format "%s" tmp.bin`
    echo "#define IPAGE_OFF_${1} ${OFFSET}"
    echo "#define IPAGE_LEN_${1} ${LEN}"
    OFFSET=$((OFFSET+LEN))
    cat tmp.bin >> out.bin
}

OFFSET=256
append_section string_zero
append_section g_usb_string_desc_1
append_section g_usb_string_desc_2
append_section g_usb_string_desc_3
append_section g_usb_conf_desc
append_section g_usb_hid_report_desc
append_section g_usb_dev_desc
append_section ldr_dev_info