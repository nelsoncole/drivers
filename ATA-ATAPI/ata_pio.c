/**
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.22
 *  
 *
 *
 */
#include <io.h>
#include "ata.h"

_void ata_pio_read(_void *buffer,_i32 bytes){
__asm__ __volatile__(\
                "cld;\
                 rep; insw"::"D"(buffer),\
                "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
                "c"(bytes/2));


}


_void ata_pio_write(_void *buffer,_i32 bytes){
__asm__ __volatile__(\
                "cld;\
                 rep; outsw"::"S"(buffer),\
                "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
                "c"(bytes/2));
}
