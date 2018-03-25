/**
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.22
 *
 */
#include <io.h>
#include "ata.h"


static inline _void atapi_pio_read(_void *buffer,_u32 bytes)
{
__asm__ __volatile__(\
                "cld;\
                 rep; insw"::"D"(buffer),\
                "d"(ata.cmd_block_base_address + ATA_REG_DATA),\
                "c"(bytes/2));

}





_u8 atapi_read_sector(_u8 nport,_u32 count,_u64 addr,_void *buffer)
{

    //FIXME este bloco nao fica aqui.
    if(nport_ajuste(nport) != 0)
    return 1;

    _u32 sector_size = 2048; // 2048 bytes
    _u8  modo_atapi  = 0;
    _i8 atapi_packet [] ={ATAPI_CMD_READ,0,0,0,0,0,0,0,0,0,0,0};

    
    ata_assert_dever(nport); //FIXME
    
    

    // configurar o pacote SCSI, 
    // 12 bytes de tamanho
    // Use LBA28
    atapi_packet [ 0x02 ] = ( addr >> 24 ) & 0xff ;
    atapi_packet [ 0x03 ] = ( addr >> 16 ) & 0xff ;
    atapi_packet [ 0x04 ] = ( addr >> 8 ) & 0xff ;
    atapi_packet [ 0x05 ] = ( addr) & 0xff ;
    atapi_packet [ 0x09 ] = (count);
    
    // Seleccionar a unidade
    outb(ata.cmd_block_base_address + ATA_REG_DEVSEL,0x40 | ata.dev_num << 4);
    // verifique se e a mesma unidade para nao esperar
    if(ata_record_dev != ata.dev_num && ata_record_channel != ata.channel){
            ata_wait(400); // 400 ns
            ata_record_dev      = ata.dev_num;
            ata_record_channel  = ata.channel;
    }

    // Selecionar modo ATAPI
    // 0 = Modo PIO.
    // 1 = DMA or UDMA
    if( ata.cmd_read_modo == ATA_DMA_MODO){
        modo_atapi = 1;
        sector_size =0;
    } 
    outb(ata.cmd_block_base_address + ATA_REG_FEATURES,modo_atapi); // Modo PIO or DMA

    // Defina o tamanho do buffer em LBA0 e LBA1
    // O tamanho do bytes por sector
     outb(ata.cmd_block_base_address + ATA_REG_LBA0,sector_size &0xff);   // LBA 7-0   
	 outb(ata.cmd_block_base_address + ATA_REG_LBA1,sector_size >> 8);   // LBA 15-8

    // Enviar comando PACKET
    ata_cmd_write(ATA_CMD_PACKET);
    if(modo_atapi == 0)ata_wait_irq(); //FIXME IRQs
    else{ 
    if(ata_wait_not_busy() != 0)return -1;
    
                
    
    }

    if(ata_wait_drq() != 0)return -1;
    // Enviar os dados do pacote
    __asm__ __volatile__("cld; rep; outsw" ::"c" (6) , "d" (\
    (ata.cmd_block_base_address +ATA_REG_DATA)),"S"(atapi_packet));
 
    if(ata_wait_drq() != 0)return -1; //TODO Nelson e mesmo correcto DRQ aqui?

    if( ata.cmd_read_modo == ATA_DMA_MODO){


           
            ide_dma_data(buffer,2048,0x80,nport,0);    

            ide_dma_start();

            ata_wait_irq();
            while(ide_dma_read_status() &1)
            if(ide_dma_read_status() &2){
            ide_dma_stop(); 
            return -1;
            } 



     return 0;


    }
    
    atapi_pio_read(buffer,sector_size);
    ata_wait_irq();
    // Verifica erros
    
    return 0;
}
