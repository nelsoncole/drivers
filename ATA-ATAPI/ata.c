/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.2
 *
 *
 */
#include <io.h>
#include "ata.h"

#include <irq.h>

extern st_dev *current_dev;
static _u32 ata_irq_invoked = 1; 

_void ata_wait(_i32 val)
{ 
   val/=100;
   while(val--)io_delay();
}



// TODO: Nelson, ao configurar os bits BUSY e DRQ 
// devemos verificar retornos de erros.
_u8 ata_wait_not_busy()
{
    while(ata_status_read() &ATA_SR_BSY)
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;

}

_u8 ata_wait_busy()
{
    while(!(ata_status_read() &ATA_SR_BSY))
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;

}


_u8 ata_wait_no_drq()
{
    while(ata_status_read() &ATA_SR_DRQ)
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;
}

_u8 ata_wait_drq()
{
    while(!(ata_status_read() &ATA_SR_DRQ))
    if(ata_status_read() &ATA_SR_ERR)
    return 1;

    return 0;
}




_void ata_irq_handler1(_void)
{

    ata_irq_invoked = 1;
   
}

_void ata_irq_handler2(_void)
{

    ata_irq_invoked = 1;
    
}


_u8 ata_wait_irq()
{
   _u32 tmp = 0x10000;
   _u8 data;
   while (!ata_irq_invoked){
    data = ata_status_read();
    if((data &ATA_SR_ERR)){
        ata_irq_invoked = 0;
        return -1;
        }
                         //ns
        if(--tmp)ata_wait(100);
        else{
        ata_irq_invoked = 0;
        return 0x80;
        }
    }
 
    ata_irq_invoked = 0;
    return 0;
}

_void ata_soft_reset()
{
    _u8 data =  inb(ata.ctrl_block_base_address + 2);
    outb(ata.ctrl_block_base_address + 2, data | 0x4);
    outb(ata.ctrl_block_base_address + 2, data &0xfb);    
}


_u8 ata_status_read()
{
   	return inb(ata.cmd_block_base_address + ATA_REG_STATUS);

}

_void ata_cmd_write(_i32 cmd_val)
{
           
    // no_busy      	
    ata_wait_not_busy();
	outb(ata.cmd_block_base_address + ATA_REG_CMD,cmd_val);
	ata_wait(400);  // Esperamos 400ns

}



_u8 ata_assert_dever(_i8 nport)
{

    switch(nport){
    case 1:
        ata.channel = 0;
        ata.dev_num = 0;
    break;
    case 2:   
        ata.channel = 0;
        ata.dev_num = 1;
    break;
    case 3:
        ata.channel = 1;
        ata.dev_num = 0;
    break;
        case 4:
        ata.channel = 1;
        ata.dev_num = 1;
    break;
    default:
        kprintf("Port %d, volue not used\n",nport);
        return -1;
     break;
    }

    set_ata_addr(ata.channel);


    return 0;

}

static inline _void ata_set_device_and_sector(_u32 count,_u64 addr,\
                                            _i32 access_type,_i8 nport)
{
   
    ata_assert_dever(nport);

    switch(access_type){
    case 28:
        //Mode LBA28
	    outb(ata.cmd_block_base_address + ATA_REG_SECCOUNT,count);	// Sector Count 7:0
	    outb(ata.cmd_block_base_address + ATA_REG_LBA0,addr);		        // LBA 7-0   
	    outb(ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 8);          // LBA 15-8
	    outb(ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 16);	        // LBA 23-16
         // Modo LBA active, Select device, and LBA 27-24
        outb(ata.cmd_block_base_address + ATA_REG_DEVSEL,0x40 |(ata.dev_num << 4) | (addr >> 24 &0x0f));
        // verifique se e a mesma unidade para nao esperar pelos 400ns
        if(ata_record_dev != ata.dev_num && ata_record_channel != ata.channel){
            ata_wait(400);
            //verifique erro
            ata_record_dev      = ata.dev_num;
            ata_record_channel  = ata.channel;}
        break;
    case 48:
        //Mode LBA48
        outb(ata.cmd_block_base_address + ATA_REG_SECCOUNT,0);	            // Sector Count 15:8
	    outb(ata.cmd_block_base_address + ATA_REG_LBA0,addr >> 24);	        // LBA 31-24   
	    outb(ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 32);	        // LBA 39-32
	    outb(ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 40);	        // LBA 47-40
	    outb(ata.cmd_block_base_address + ATA_REG_SECCOUNT,count);	        // Sector Count 7:0
	    outb(ata.cmd_block_base_address + ATA_REG_LBA0,addr);		        // LBA 7-0   
	    outb(ata.cmd_block_base_address + ATA_REG_LBA1,addr >> 8);           // LBA 15-8
	    outb(ata.cmd_block_base_address + ATA_REG_LBA2,addr >> 16);	        // LBA 23-16
        outb(ata.cmd_block_base_address + ATA_REG_DEVSEL,0x40 | ata.dev_num << 4);   // Modo LBA active, Select device,        
        // verifique se e a mesma unidade para nao esperar pelos 400ns
        if(ata_record_dev != ata.dev_num && ata_record_channel != ata.channel){
            ata_wait(400);
            ata_record_dev      = ata.dev_num;
            ata_record_channel  = ata.channel;}
        break;
    case 2:
        //Modo CHS
        // not suport
        break;



       }
       

}

_u8 ata_read_sector(_i8 port,_u16 count,_u64 addr, _void *buffer)
{ 
       
        if(nport_ajuste(port) != 0)
        return 1;

        //select device, lba, count
        ata_set_device_and_sector(count,addr,current_dev->dev_access,port);
        
        if(current_dev->dev_access == 28 && ata.cmd_read_modo == ATA_PIO_MODO){

            ata_cmd_write(ATA_CMD_READ_SECTORS);
            //ata_wait_irq(); //FIXME IRQs
            ata_wait_not_busy();
            if(ata_wait_drq() != 0)return -1;
            ata_pio_read(buffer,current_dev->dev_byte_per_sector);
            ata_wait_not_busy();
            if(ata_wait_no_drq() != 0)return -1;

        }

        else if(current_dev->dev_access == 48 && ata.cmd_read_modo == ATA_PIO_MODO){

            ata_cmd_write(ATA_CMD_READ_SECTORS_EXT);
            //ata_wait_irq(); //FIXME IRQs
            ata_wait_not_busy();
            if(ata_wait_drq() != 0)return -1;
            ata_pio_read(buffer,current_dev->dev_byte_per_sector);
            ata_wait_not_busy();
            if(ata_wait_no_drq() != 0)return -1;

            
        }

        else if(current_dev->dev_access == 28 && ata.cmd_read_modo == ATA_DMA_MODO){
            
            ide_dma_data(buffer,512,0x80,port,0);    
            ata_cmd_write(ATA_CMD_READ_DMA_EXT);
            
            ide_dma_start();

            ata_wait_irq();
            
            while(ide_dma_read_status() &1)
            if(ide_dma_read_status() &2){
            ide_dma_stop(); 
            return -1;
            }   
        }


        else if(current_dev->dev_access == 48 && ata.cmd_read_modo == ATA_DMA_MODO){
            

            ide_dma_data(buffer,512,0x80,port,0);    
            ata_cmd_write(ATA_CMD_READ_DMA_EXT);

            ide_dma_start();

            ata_wait_irq();
            while(ide_dma_read_status() &1)
            if(ide_dma_read_status() &2){
            ide_dma_stop(); 
            return -1;
            } 
        }


        return 0;

}

_u8 ata_write_sector(_i8 port,_u16 count,_u64 addr, _void *buffer)
{ 
       
        if(nport_ajuste(port) != 0)
        return 1;

        //select device, lba, count
        ata_set_device_and_sector(count,addr,current_dev->dev_access,port);
        
        if(current_dev->dev_access == 28 && ata.cmd_read_modo == ATA_PIO_MODO){

            ata_cmd_write(ATA_CMD_WRITE_SECTORS);
            //ata_wait_irq(); //FIXME IRQs
            ata_wait_not_busy();
            if(ata_wait_drq() != 0)return -1;
            ata_pio_write(buffer,current_dev->dev_byte_per_sector);

            //Flush Cache
            ata_cmd_write(ATA_CMD_FLUSH_CACHE);
            ata_wait_not_busy();
            if(ata_wait_no_drq() != 0)return -1;

        }

        else if(current_dev->dev_access == 48 && ata.cmd_read_modo == ATA_PIO_MODO){

            ata_cmd_write(ATA_CMD_WRITE_SECTORS_EXT);
            //ata_wait_irq(); //FIXME IRQs
            ata_wait_not_busy();
            if(ata_wait_drq() != 0)return -1;
            ata_pio_write(buffer,current_dev->dev_byte_per_sector);

            //Flush Cache
            ata_cmd_write(ATA_CMD_FLUSH_CACHE_EXT);
            ata_wait_not_busy();
            if(ata_wait_no_drq() != 0)return -1;

        }    
    
        else if(current_dev->dev_access == 28 && ata.cmd_read_modo == ATA_DMA_MODO){
            
            ide_dma_data(buffer,512,0x80,port,1);    
            ata_cmd_write(ATA_CMD_WRITE_DMA_EXT);

            ide_dma_start();

            ata_wait_irq();
            while(ide_dma_read_status() &1)
            if(ide_dma_read_status() &2){
            ide_dma_stop(); 
            return -1;
            } 

        } 


        else if(current_dev->dev_access == 48 && ata.cmd_read_modo == ATA_DMA_MODO){
            

            ide_dma_data(buffer,512,0x80,port,1);    
            ata_cmd_write(ATA_CMD_WRITE_DMA_EXT);

            ide_dma_start();

            ata_wait_irq();
            while(ide_dma_read_status() &1)
            if(ide_dma_read_status() &2){
            ide_dma_stop(); 
            return -1;
            } 
        }
        return 0;

}


_u8 ata_identify_device(_i8 nport)
{
    _u8 status;
    _u8 lba1,lba2;

    ata_assert_dever(nport);

    // Ponto flutuante
    if(ata_status_read() == 0xff)//Sem unidade conectada ao barramento
        return -1;

    outb(ata.cmd_block_base_address + ATA_REG_SECCOUNT,0);	// Sector Count 7:0
	outb(ata.cmd_block_base_address + ATA_REG_LBA0,0);        // LBA 7-0   
	outb(ata.cmd_block_base_address + ATA_REG_LBA1,0);        // LBA 15-8
	outb(ata.cmd_block_base_address + ATA_REG_LBA2,0);        // LBA 23-16

    
    // Select device,
    outb(ata.cmd_block_base_address + ATA_REG_DEVSEL,0xE0| ata.dev_num << 4);
    ata_wait(400);

    // cmd
    ata_cmd_write(ATA_CMD_IDENTIFY_DEVICE); 
    // ata_wait_irq();
    // Nunca espere por um IRQ aqui
    // Devido unidades PATAPI, ao menos que pesquisamos pelo Bit ERROR
    // Melhor seria fazer polling
     
    ata_wait(400);


    if(ata_status_read() == 0) //Sem unidade no canal 
        return -1;

   lba1 = inb(ata.cmd_block_base_address + ATA_REG_LBA1);
   lba2 = inb(ata.cmd_block_base_address + ATA_REG_LBA2);

   if(lba1 == 0x14 && lba2 == 0xEB){
        kputs("Unidade PATAPI\n");   
        //call atapi_identify_device
        return 1;
   }
   else if(lba1 == 0x69  && lba2 == 0x96){

        kputs("Unidade SATAPI\n");   
        //call atapi_identify_device
        return 1;

   }
   else if(lba1 == 0x3C && lba2 == 0xC3){
        kputs("Unidade SATA\n");   
        // O dispositivo responde imediatamente um erro ao cmd Identify device
        // entao devemos esperar pelo DRQ ao invez de um BUSY
        // em seguida enviar 256 word de dados PIO.
        ata_wait_drq(); 
        ata_pio_read(&ata_identify_dev,512);
        ata_wait_not_busy();
        ata_wait_no_drq();
        return 0;
   }
   else if(lba1 == 0 && lba2 == 0){
        kputs("Unidade PATA\n");
        // aqui esperamos pelo DRQ
        // e eviamoos 256 word de dados PIO
        ata_wait_drq();
        ata_pio_read(&ata_identify_dev,512);

        ata_wait_not_busy();
        ata_wait_no_drq();
        return 0;
    }   

}
