/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.2
 *
 *
 */

#include <io.h>
#include <pci.h>
#include <block_device.h>
#include "ata.h"

#define DISK1 1
#define DISK2 2
#define DISK3 3
#define DISK4 4

static const _i8 *ata_sub_class_code_register_strings[] ={
    "Unknown",
    "IDE Controller",
    "Unknown",
    "Unknown",
    "RAID Controller",
    "Unknown",
    "AHCI Controller"
};


extern st_dev *current_dev;



// base address 
static _u32 ATA_BAR0;    // Primary Command Block Base Address
static _u32 ATA_BAR1;    // Primary Control Block Base Address
static _u32 ATA_BAR2;    // Secondary Command Block Base Address
static _u32 ATA_BAR3;    // Secondary Control Block Base Address
static _u32 ATA_BAR4;    // Legacy Bus Master Base Address
static _u32 ATA_BAR5;    // AHCI Base Address / SATA Index Data Pair Base Address




void ata_initialize()
{
    _u32 data;


    data = pci_scan_device(PCI_CALSSE_MASS);

    if(data  == -1)
    return (PCI_MSG_ERROR); // Abortar
    _u8 bus =(data  >>  8 &0xff);
    _u8 dev =(data  >> 3  &31);
    _u8 fun =(data  &7);



 
    data = ata_pci_configuration_space(bus,dev,fun);

    if(data == PCI_MSG_ERROR){
        kprintf("Error Driver [%X]\n",data);
        return;    }
    else if(data == PCI_MSG_AVALIABLE){
        
         kprintf("Not suport, RAID Controller\n");
         return;       
    }


    // Initialize base address
    // AHCI/IDE Compativel com portas IO IDE legado
    ATA_BAR0 = (ata_pci.bar0 & ~7) + ATA_IDE_BAR0 * ( !ata_pci.bar0);
    ATA_BAR1 = (ata_pci.bar1 & ~3) + ATA_IDE_BAR1 * ( !ata_pci.bar1);       
    ATA_BAR2 = (ata_pci.bar2 & ~7) + ATA_IDE_BAR2 * ( !ata_pci.bar2);
    ATA_BAR3 = (ata_pci.bar3 & ~3) + ATA_IDE_BAR3 * ( !ata_pci.bar3);
    ATA_BAR4 = (ata_pci.bar4 & ~0x7) + ATA_IDE_BAR4 * ( !ata_pci.bar4);
    ATA_BAR5 = (ata_pci.bar5 & ~0xf) + ATA_IDE_BAR5 * ( !ata_pci.bar5);

    //Soft Reset, defina IRQ
    outb(ATA_BAR1 +2,0xff);
    outb(ATA_BAR3 +2,0xff);
    outb(ATA_BAR1 +2,0x00);
    outb(ATA_BAR3 +2,0x00);


    ata_record_dev = -1;
    ata_record_channel = -1;

    cli();
    irq_enable(14);
    irq_enable(15);
    sti();


    kprintf("Mass Storage initialize ...\n");
    mass_storage_initialize();

    // Testing...
    ata.cmd_read_modo = ATA_DMA_MODO; //ATA_PIO_MODO; //FIXME


    _u8 *buffer = (_u8*)kmalloc(sizeof(_u8)*4096);

   
    /*
    if(ata_read_sector(DISK1,1,0,buffer))return;
    kprintf("dma teste de leitura em unidade ata: ( %s )\n",buffer+3);
    if(atapi_read_sector(DISK3,1,16,buffer))return;
    kprintf("dma teste de leitura em unidade atapi:( %s )\n",buffer); */
                                                                                                    
   
  
}



_void set_ata_addr(_i32 channel)
{

    switch(channel){
    case ATA_PRIMARY:
        ata.cmd_block_base_address  = ATA_BAR0;
        ata.ctrl_block_base_address = ATA_BAR1;
        ata.bus_master_base_address = ATA_BAR4;
        break;
    case ATA_SECONDARY:
        ata.cmd_block_base_address  = ATA_BAR2;
        ata.ctrl_block_base_address = ATA_BAR3;
        ata.bus_master_base_address = ATA_BAR4 + 8;
        break;
    default:
       //PANIC
        break;

    }

}


// Espaço de configuraçao PCI Mass Storage
_i8 ata_pci_configuration_space(_u8 bus,_u8 dev,_u8 fun)
{
    _u32 data;

    kprintf("PCI Mass Storage Initialize...\n");
  

    // Indentification Device
    data  = read_pci_config_addr(bus,dev,fun,0);
    
    ata_pci.vendor_id = data &0xffff;
    ata_pci.device_id = data >> 16 &0xffff;
    kprintf("Vendor ID: %X,Device ID: %X\n",ata_pci.vendor_id,ata_pci.device_id);

    // PCI command and status

    // Habilitar interrupcao (INTx#)
    // data  = read_pci_config_addr(bus,dev,fun,4);
    // write_pci_config_addr(bus,dev,fun,4,data & ~0x400);


    data  = read_pci_config_addr(bus,dev,fun,4);

    ata_pci.command = data &0xffff; 
    ata_pci.status  = data >>16 &0xffff;


    kprintf("Command %x Status %x \n",ata_pci.command,ata_pci.status);
    

    // Classe code, programming interface, revision id
    data  = read_pci_config_addr(bus,dev,fun,8);
    
    ata_pci.classe      = data  >> 24 &0xff;
    ata_pci.subclasse   = data  >> 16 &0xff;
    ata_pci.prog_if     = data >> 8 &0xff;
    ata_pci.revision_id = data &0xff;

    if(ata_pci.classe == 1 && ata_pci.subclasse == 1){
        ata.chip_control_type = ATA_IDE_CONTROLLER;        
        kprintf("Sub Class Code Register %s Programming Interface %d Revision ID %d\n",\
        ata_sub_class_code_register_strings[ata.chip_control_type],ata_pci.prog_if,ata_pci.revision_id);
    
        // IDE

        // Bus Master Enable
        data  = read_pci_config_addr(bus,dev,fun,4);
        write_pci_config_addr(bus,dev,fun,4,data | 0x4);        

            
    }
    else if(ata_pci.classe == 1 && ata_pci.subclasse == 4){
        ata.chip_control_type = ATA_RAID_CONTROLLER;
        kprintf("Sub Class Code Register %s Programming Interface %d Revision ID %d\n",\
        ata_sub_class_code_register_strings[ata.chip_control_type],ata_pci.prog_if,ata_pci.revision_id);
    
        // Em avaliacao

         return PCI_MSG_AVALIABLE;
    }
    else if(ata_pci.classe == 1 && ata_pci.subclasse == 6){
       ata.chip_control_type = ATA_AHCI_CONTROLLER;
       kprintf("Sub Class Code Register %s Programming Interface %d Revision ID %d\n",\
       ata_sub_class_code_register_strings[ata.chip_control_type],ata_pci.prog_if,ata_pci.revision_id);
        //ACHI
        // Bus Master Enable
        data  = read_pci_config_addr(bus,dev,fun,4);
        write_pci_config_addr(bus,dev,fun,4,data | 0x4);        

    }
    else {
       //PANIC 
        kprintf("PANICO DRIVER BLOCK");
        while(TRUE);
    }



    // PCI cacheline, Latancy, Headr type, end BIST
    data  = read_pci_config_addr(bus,dev,fun,0xC);

    ata_pci.primary_aster_tatency_timer = data >>8 &0xff;
    ata_pci.header_type     = data >>16 &0xff;
    ata_pci.BIST            = data >>24 &0xff;
    
    ata_pci.bar0    = read_pci_config_addr(bus,dev,fun,0x10);
    ata_pci.bar1    = read_pci_config_addr(bus,dev,fun,0x14);
    ata_pci.bar2    = read_pci_config_addr(bus,dev,fun,0x18);
    ata_pci.bar3    = read_pci_config_addr(bus,dev,fun,0x1C);
    ata_pci.bar4    = read_pci_config_addr(bus,dev,fun,0x20);
    ata_pci.bar5    = read_pci_config_addr(bus,dev,fun,0x24);

    
    // Interrupt Line and Interrupt Pin
    // Analise se o dispositivo precisa de um atributo IRQ
    data  = read_pci_config_addr(bus,dev,fun,0x3C);
    write_pci_config_addr(bus,dev,fun,0x3C,data | 0xFE);
    data  = read_pci_config_addr(bus,dev,fun,0x3C);
    ata_pci.interrupt_line  = data &0xff;
    ata_pci.interrupt_pin   = data >> 8 &0xff;

    kprintf("Interrupt Line %x Interrupt Pin %x\n",ata_pci.interrupt_pin,ata_pci.interrupt_line );
    // Nao assinado em PCI IDE


    return (PCI_MSG_SUCCESSFUL);

}
