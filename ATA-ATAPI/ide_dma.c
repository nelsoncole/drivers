/**
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.2
 * Legacy Bus Master Base Address
 *
 *  TODO Nelson, ja mais se esqueça de habiliatar o // Bus Master Enable
 *  no espaço de configuraçao PCI (offset 0x4 Command Register)
 */

#include "ata.h"
#include <io.h>


// Commands dma 
#define _start   1
#define _stop    0
#define _read    0
#define _write   1


// Status dma
#define ide_dma_sr_err     0x02



// Registros bus master base address
#define ide_dma_reg_cmd     0x00
#define ide_dma_reg_status  0x02
#define ide_dma_reg_addr    0x04

// channel
#define ide_dma_primary     0x00
#define ide_dma_secundary   0x01

struct ide_dma_prdt
{
    _u32    addr;
    _u16    byte_count;
    _u8     reserved;
    _u8     eot; 
}*ide_dma_prdt[4]__attribute__((aligned(4096)));


_u8 ide_dma_data(_void *addr,_u16 byte_count,_u8 eot,_u8 nport,_u8 flg)
{
    _u8 data;

    ide_dma_prdt[nport -1]->addr          = (_u32)addr &~1;
    ide_dma_prdt[nport -1]->byte_count    = byte_count &~1; //TODO (&~1)sera que e necessario?
    ide_dma_prdt[nport -1]->reserved      = 0;
    ide_dma_prdt[nport -1]->eot           = eot &~0x7f;
    
    outb(ata.bus_master_base_address + ide_dma_reg_addr +0,((_u32)ide_dma_prdt[nport -1]));
    outb(ata.bus_master_base_address + ide_dma_reg_addr +1,((_u32)ide_dma_prdt[nport -1] >>8));
    outb(ata.bus_master_base_address + ide_dma_reg_addr +6,((_u32)ide_dma_prdt[nport -1] >>16));
    outb(ata.bus_master_base_address + ide_dma_reg_addr +7,((_u32)ide_dma_prdt[nport -1] >>24));

    data = inb(ata.bus_master_base_address + ide_dma_reg_status);
	outb(ata.bus_master_base_address + ide_dma_reg_status, data &0xf0);

    
    //  bit 3 read/write
    //      0 = Memory reads
    //      1 = Memory writes
    data = inb(ata.bus_master_base_address + ide_dma_reg_cmd);

    if(flg ==_read)
	outb(ata.bus_master_base_address + ide_dma_reg_cmd, data & ~8);
    
    else if(flg == _write)
    outb(ata.bus_master_base_address + ide_dma_reg_cmd, data |  8);

    else return -1;

    return 0;       
    
}


_void ide_dma_start(){
    _u8 data = inb(ata.bus_master_base_address + ide_dma_reg_cmd);
	outb(ata.bus_master_base_address + ide_dma_reg_cmd, data | 1);

}

_void ide_dma_stop(){
    _u8 data = inb(ata.bus_master_base_address + ide_dma_reg_cmd);
	outb(ata.bus_master_base_address + ide_dma_reg_cmd, data &~1);
    data = inb(ata.bus_master_base_address + ide_dma_reg_status);
	outb(ata.bus_master_base_address + ide_dma_reg_status, data &~6);

}

_u8 ide_dma_read_status(){
    return inb(ata.bus_master_base_address + ide_dma_reg_status);
}
