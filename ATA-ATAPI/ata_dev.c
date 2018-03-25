/* *
 * Copyright (C) 2017-2018 (Nelson Sapalo da Silva Cole)
 * Khole OS v0.2
 *
 *
 */

#include <io.h>
#include "ata.h"
#include <block_device.h>

const _i8 *dev_type[]={
"ATA",
"ATAPI"
};

_i8 devn[]={0,'A','B','C','D','E','F'};


st_dev *current_dev;     // A unidade actualmente seleccionada
st_dev *ready_queue_dev; // O início da lista.
_u32  dev_next_pid = 0;     // O próximo ID de unidade disponível.  

_void mass_storage_initialize()
{

    current_dev = ready_queue_dev   = (st_dev*)kmalloc(sizeof(st_dev));
    current_dev->dev_id             = dev_next_pid++;
    current_dev->dev_type           = NULL;
    current_dev->dev_num            = NULL;
    current_dev->dev_channel        = -1;
    current_dev->next               = NULL;
    current_dev->dev_nport          = NULL;




    // Testing
    mass_storage_conect(1);
    mass_storage_conect(2);
    mass_storage_conect(3);
    mass_storage_conect(4);
 
    
}

_u8 mass_storage_conect(_i8 port)
{

    _u8 data = ata_identify_device(port);
    if(data == -1)
    return 1;

    st_dev *new_dev = (st_dev*)kmalloc(sizeof(st_dev));
    if(data == 0){

        // unidades ATA

        new_dev->dev_type   =(ata_identify_dev.ata_dev &0x8000)? 0xffff:ATA_DEVICE_TYPE;
        new_dev->dev_access =48; //FIXME devemos verificar
        new_dev->dev_byte_per_sector = 512;  
        new_dev->dev_total_num_sector_lba48 = ata_identify_dev.total_number_of_user_addr_sector_lba48;
        new_dev->dev_size = (ata_identify_dev.total_number_of_user_addr_sector_lba48 * 512);

        
    }else if(data == 1){

        // Unidades ATAPI


        new_dev->dev_type   = ATAPI_DEVICE_TYPE;
        new_dev->dev_access =28; //FIXME devemos verificar
        new_dev->dev_byte_per_sector = 2048;  
        new_dev->dev_total_num_sector_lba48 = 0;
        new_dev->dev_size = (( 0 * 2048)/1024);

    
    }else{
        //Identificar o erro
        return (1);}

//Dados em comum.

    new_dev->dev_id     = dev_next_pid++;
    new_dev->dev_num    = ata.dev_num;
    new_dev->dev_channel= ata.channel;
    kprintf("Disk Local %c: Dev%d, Channel %d, Type %s, Size %dKB\n",devn[new_dev->dev_id],\
    new_dev->dev_num,new_dev->dev_channel,dev_type[new_dev->dev_type],new_dev->dev_size/1024);


        new_dev->dev_nport = port;
        switch(port){
            case 1:
            ata_busy_nport.ata1 = 1;
            break;
            case 2:
            ata_busy_nport.ata2 = 1;
            break;
            case 3:
            ata_busy_nport.ata3 = 1;
            break;
            case 4:
            ata_busy_nport.ata4 = 1;
            break;        
        }



// Aqui inicializa o dispositivo 
// IDENTIFY DEVICE/IDENTIFY PACKET DEVICE
// Salva dados.    

    new_dev->next = NULL;


    // TODO
    st_dev* tmp_dev = (st_dev*)ready_queue_dev;

    while (tmp_dev->next){
        tmp_dev = tmp_dev->next;
    }
    
    tmp_dev->next = new_dev;
    return 0;
}




static inline _void dev_switch(_void)
{

    // Pula, se ainda não tiver 
    // nenhuma unidade
    if (!current_dev)
    return;
    // Obter a próxima tarefa a ser executada.
    current_dev = current_dev->next;
    
    // Se caímos no final da lista vinculada, 
    // comece novamente do início.
    if (!current_dev)
    current_dev = ready_queue_dev;


}

static inline _i32 getpid_dev()
{
    return current_dev->dev_id;
}


static inline _i32 getnport_dev()
{
    return current_dev->dev_nport;
}






_u8 nport_ajuste(_i8 nport){
    _i8 i = 0;
    while(nport != getnport_dev()){
    if (i == 4)return 1;
    dev_switch();
    i++;
    }
    if(getnport_dev() == 0)return 1;
     return 0;
}


