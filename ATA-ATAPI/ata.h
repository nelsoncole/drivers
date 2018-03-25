/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.2
 *
 *
 */


#ifndef __ATA_H__
#define __ATA_H__
#include <typedef.h>


#define PCI_CALSSE_MASS  1

// Controladores de unidades ATA
#define ATA_IDE_CONTROLLER      0x1
#define ATA_RAID_CONTROLLER     0x4
#define ATA_AHCI_CONTROLLER     0x6

// Retorno da inicializacao PCI 
#define PCI_MSG_ERROR       -1
#define PCI_MSG_AVALIABLE   0x80
#define PCI_MSG_SUCCESSFUL  0


// IO Space Legacy BARs IDE 
#define ATA_IDE_BAR0 0x1F0  // Primary Command Block Base Address
#define ATA_IDE_BAR1 0x3F4  // Primary Control Block Base Address
#define ATA_IDE_BAR2 0x170  // Secondary Command Block Base Address
#define ATA_IDE_BAR3 0x374  // Secondary Control Block Base Address
#define ATA_IDE_BAR4 0      // Bus Master Base Address
#define ATA_IDE_BAR5 0      //



// ATA/ATAPI Command Set

#define ATA_CMD_CFA_ERASE_SECTORS               0xC0
#define ATA_CMD_CFA REQUEST_EXTENDED_ERROR_CODE 0x03
#define ATA_CMD_CHECK_MEDIA_CARD_TYPE           0xD1
#define ATA_CMD_CHECK_POWER_MODE                0xE5
#define ATA_CMD_DEVICE_RESET                    0x08
#define ATA_CMD_EXECUTE_DEVICE_DIAGNOSTIC       0x90
#define ATA_CMD_FLUSH_CACHE                     0xE7
#define ATA_CMD_FLUSH_CACHE_EXT                 0xEA
#define ATA_CMD_IDENTIFY_DEVICE                 0xEC
#define ATA_CMD_IDENTIFY_PACKET_DEVICE          0xA1
#define ATA_CMD_PACKET                          0xA0
#define ATA_CMD_READ_BUFFER                     0xE4
#define ATA_CMD_READ_DMA                        0xC8
#define ATA_CMD_READ_DMA_EXT                    0x25
#define ATA_CMD_READ_SECTORS                    0x20
#define ATA_CMD_READ_SECTORS_EXT                0x24
#define ATA_CMD_WRITE_BUFFER                    0xE8
#define ATA_CMD_WRITE_DMA                       0xCA
#define ATA_CMD_WRITE_DMA_EXT                   0x35
#define ATA_CMD_WRITE_SECTORS                   0x30
#define ATA_CMD_WRITE_SECTORS_EXT               0x34

// ATAPI descrito no SCSI
#define ATAPI_CMD_READ  0xA8
#define ATAPI_CMD_EJECT 0x1B

//ATA bits de status control (alternativo)
#define ATA_SC_HOB      0x80    // High Order Byte
#define ATA_SC_SRST     0x04    // Soft Reset
#define ATA_SC_nINE     0x02    // INTRQ




//ATA bits de status 
#define ATA_SR_BSY      0x80    // Busy
#define ATA_SR_DRDY     0x40    // Device Ready
#define ATA_SR_DF       0x20    // Device Fault
#define ATA_SR_DSC      0x10    // Device Seek Complete
#define ATA_SR_DRQ      0x08    // Data Request
#define ATA_SR_SRST     0x04    // 
#define ATA_SR_IDX      0x02    // Index
#define ATA_SR_ERR      0x01    // Error

//ATA bits de errro apos a leitura.
#define ATA_ER_BBK      0x80    // 
#define ATA_ER_UNC      0x40    //
#define ATA_ER_MC       0x20    //
#define ATA_ER_IDNF     0x10    //
#define ATA_ER_MCR      0x08    //
#define ATA_ER_ABRT     0x04    //
#define ATA_ER_TK0NF    0x02    //
#define ATA_ER_AMNF     0x01    //





// Devices
#define ATA_MASTER_DEV  0x00
#define ATA_SLAVE_DEV   0x01

// Bus
#define ATA_PRIMARY     0x00
#define ATA_SECONDARY   0x01

// ATA type
#define ATA_DEVICE_TYPE     0x00
#define ATAPI_DEVICE_TYPE   0x01



// Registradores 
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_DEVSEL 0x06
#define ATA_REG_CMD 0x07
#define ATA_REG_STATUS 0x07


// Modo de transferencia
#define ATA_PIO_MODO 0 
#define ATA_DMA_MODO 1


// Historia
// Programaçao do ATA a partir do ICH5/9 e suporte a IDE legado.
// ICH5 integraçao do SATA e suporte total ACPI 2.0.
// ICH6 implementaram os controladores AHCI SATA pela primeira vez.
struct {

    // Suporta a IDE Controller
    _u16    vendor_id;
    _u16    device_id;
    _u16    command;
    _u16    status;
    _u8     prog_if;
    _u8     revision_id;
    _u8     classe;
    _u8     subclasse;
    _u8     primary_aster_tatency_timer;
    _u8     header_type;
    _u8     BIST;
    _u32    bar0;
    _u32    bar1; 
    _u32    bar2;
    _u32    bar3;
    _u32    bar4;
    _u32    bar5;
    _u16    subsystem_vendor_id;
    _u16    subsystem_id;  
    _u32    capabilities_pointer;
    _u8     interrupt_line;
    _u8     interrupt_pin;

    // AHCI

    // "Emos" de acrescer com o tempo de acordo nossas necessidades.


}ata_pci;


// Estrutura para o controle de execuncao do programa
struct{
    _u8     chip_control_type;
    _u8     channel;
    _u8     dev_type;  
    _u8     dev_num;
    _u8     access_type;
    _u8     cmd_read_modo; //
    _u32    cmd_block_base_address;
    _u32    ctrl_block_base_address;
    _u32    bus_master_base_address;
}ata;



// Estrutura de dados ATA_CMD_IDENTIFY_DEVICE 
struct st_ata_identify_dev{

	uint16_t ata_dev;			//0
	uint16_t obsolete1; 		//1
	uint16_t specific_configuration; 	//2
	short reserved1[20]; 		//3-22
	char firmware_revision[8];	//23-26 Firmware revision (8 ASCII characters)
	char model_number[40];		//27-46 Model number (40 ASCII characters)
	short reserved2[2];		//47-48 
	uint16_t capabilities;		/*49 Capabilities
					11 --> 1 = IORDY supported, 0 = IORDY may be supported
					10 --> 1 = IORDY may be disabled
					9 --> 1 = LBA supported
					8 --> 1 = DMA supported.
					7-0 --> Retired */
	short reserved3[10];		//50-59
	uint16_t total_number_of_user_addr_sector_lba28;	//60-61 Total number of user addressable sectors
	short reserved4[2];		//62-63
	uint16_t pio_modes_supported;	//64  15-8 --> Reserved, 7-0 --> PIO modes supported
	short reserved5[15];		//65-79
	uint16_t major_version_number;	//80
	short reserved6[12];		//81-92
	uint16_t devices_info;		//93
	short reserved7[6];		//94-99
	uint64_t total_number_of_user_addr_sector_lba48; //100-103
	short reserved[152];		//104-255

}ata_identify_dev;


// Estrutura de bits para controlar o IDENTIFY_DEVICE
struct ata_busy_nport{
    _u8     ata1 :1;
    _u8     ata2 :1;
    _u8     ata3 :1;
    _u8     ata4 :1;
    _u8     ata5 :4;

}ata_busy_nport;

typedef struct st_dev {

    _u32        dev_id;
    _u8         dev_nport;
    _u8         dev_type; //ATA or ATAPI
    _u8         dev_num;
    _u8         dev_channel;
    _u8         dev_access; // LBA28 or LBA48
    _u32        dev_byte_per_sector;
    _u64        dev_total_num_sector_lba48;
    _u32        dev_size;
       
    struct st_dev *next;


}st_dev;


/*************************** variaves *************************************/
_u8 ata_record_dev;
_u8 ata_record_channel;



/**************************** Libs ***************************************/

// ata_dev.c
_u8 nport_ajuste(_i8 nport);

// ata_main.c
_void set_ata_addr(_i32 channel);
_i8 ata_pci_configuration_space(_u8 bus,_u8 dev,_u8 fun);

// ata.c
_u8 ata_read_sector(_i8 port,_u16 count,_u64 addr, _void *buffer);
_u8 ata_write_sector(_i8 port,_u16 count,_u64 addr, _void *buffer);
_u8 ata_identify_device(_i8 nport);
_void ata_wait(_i32 val);
_u8 ata_wait_not_busy();
_u8 ata_wait_busy();
_u8 ata_wait_no_drq();
_u8 ata_wait_drq();
_u8 ata_wait_irq();
_u8 ata_status_read();
_void ata_cmd_write(_i32 cmd_val);

_u8 ata_assert_dever(_i8 nport);


// ata_pio.c
_void ata_pio_read(_void *buffer,_i32 bytes);
_void ata_pio_write(_void *buffer,_i32 bytes);

//ide_dma.c
_u8 ide_dma_data(_void *addr,_u16 byte_count,_u8 eot,_u8 nport,_u8 flg);
_void ide_dma_start();
_void ide_dma_stop();
_u8 ide_dma_read_status();


//atapi_pio.c
_u8 atapi_read_sector(_u8 nport,_u32 count,_u64 addr,_void *buffer);


#endif
