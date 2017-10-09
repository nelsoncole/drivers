#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define FAT_ATTR_READ_ONLY	0x01
#define FAT_ATTR_HIDDEN		0x02
#define FAT_ATTR_SYSTEM		0x04
#define FAT_ATTR_VOLUME_ID	0x08
#define FAT_ATTR_DIRECTORY	0x10
#define FAT_ATTR_ARCHIVE		0x20
#define FAT_ATTR_LONG_NAME	(FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN\
 				| FAT_ATTR_SYSTEM| FAT_ATTR_VOLUME_ID)

#define FAT_DIR_ENTRY_SIZE	32


typedef struct fat_bpb {

	char BS_jmpBoot[3];
	char BS_OEMName[8];
	unsigned short BPB_BytsPerSec;
	unsigned char BPB_SecPerClus;
	unsigned short BPB_RsvdSecCnt;
	unsigned char BPB_NumFATs;
	unsigned short BPB_RootEntCnt;
	unsigned short BPB_TotSec16;
	unsigned char BPB_Media;
	unsigned short BPB_FATSz16;
	unsigned short BPB_SecPertrk;
	unsigned short BPB_NumHeads;
	unsigned int BPB_HiddSec;
	unsigned int BPB_TotSec32;	
	unsigned int BPB_FATSz32;
	unsigned short BPB_ExtFlags;
	unsigned short BPB_FSVer;
	unsigned int BPB_RootClus;
	unsigned short BPB_FSInfo;
	unsigned short BPB_BkBootSec;
	char BPB_Reserved[12];
	unsigned char BS_DrvNum;
	unsigned char BS_Reserved1;
	unsigned char BS_BootSig;
	unsigned int BS_VolID;
	char BS_VolLab[11];
	char BS_FilSysType[8];
	char code[512-90];

}__attribute__ ((packed)) fat32_t;




typedef struct fat_directory{
	
	char DIR_Name[11];
	unsigned char DIR_Attr;
	unsigned char DIR_NTRes;
	unsigned char DIR_CrtTimeTenth;
	unsigned short DIR_CrtTime;
	unsigned short DIR_CrtDate;
	unsigned short DIR_LstAccDate;
	unsigned short DIR_FstClusHI;
	unsigned short DIR_WrtTime;
	unsigned short DIR_WrtDate;
	unsigned short DIR_FstClusLO;
	unsigned int DIR_FileSize;
	
}__attribute__ ((packed)) fat_directory_t;

static size_t file_size_bytes(FILE *fp);
static void fta_region(FILE *fp,unsigned int end_offset, unsigned int value);

fat32_t buffer_boot_record;
fat_directory_t directory;


int main(int argc, char **argv){
	size_t arq_size;

	unsigned char* dados =(unsigned char*)malloc(4096);		


	FILE *fp1;
	if((fp1=fopen("stage1.bin","rb"))==NULL){
	printf("Erro ao abrir o arquivo: stage1.bin");
	exit(1); 
	}

	arq_size = file_size_bytes(fp1);
	
	if((fread(&buffer_boot_record,sizeof(char),arq_size,fp1))!=(sizeof(buffer_boot_record))){
	printf("Erro ao copiar o sector de boot");
	exit(1);	
	}



	FILE *fp2;
	if((fp2=fopen("disk.img","r+b"))==NULL){
	printf("Erro ao abrir o arquivo: disk.img");
	exit(1); 
	}

	/*Calculando o total de sectores*/

	buffer_boot_record.BPB_TotSec32 = file_size_bytes(fp2)/buffer_boot_record.BPB_BytsPerSec;

	/*Calculando a FATSz32*/
	buffer_boot_record.BPB_FATSz32 = (((((buffer_boot_record.BPB_TotSec32
				       	- buffer_boot_record.BPB_RsvdSecCnt)
					/ buffer_boot_record.BPB_SecPerClus)*4)
				     	/buffer_boot_record.BPB_BytsPerSec)
					/buffer_boot_record.BPB_NumFATs)+1;


	
	/*Escrevendo o sector de boot*/
	if((fwrite(&buffer_boot_record,sizeof(char),arq_size,fp2))!=(sizeof(buffer_boot_record))){
	printf("Erro ao escrever o sector de boot");
	exit(1);	
	}else{ fclose(fp1);}


	

	/*Copiando os arquivos no disco*/



	if((fp1=fopen("stage2.bin","rb"))==NULL){
	printf("Erro ao abrir o arquivo: stage2.bin");
	exit(1); 
	}
	 
	

	int total_cluster;
	int resto_cluster;
	int cluster_start;
	unsigned int byte_start;
	size_t size_copy;
	size_t size_colar;
	arq_size = file_size_bytes(fp1);

	if(arq_size > (buffer_boot_record.BPB_SecPerClus*buffer_boot_record.BPB_BytsPerSec)){
	total_cluster = arq_size / (buffer_boot_record.BPB_SecPerClus
				 * buffer_boot_record.BPB_BytsPerSec);

	resto_cluster = arq_size % (buffer_boot_record.BPB_SecPerClus
				 * buffer_boot_record.BPB_BytsPerSec);

	 }else{

		resto_cluster = arq_size;
		
	}



	/*Escrevendo no Directory raíz FAT32   */



	char *nome = "stage2     ";
	strncpy(directory.DIR_Name,nome,11);
	directory.DIR_Attr = FAT_ATTR_ARCHIVE;
	directory.DIR_NTRes = 0;
	directory.DIR_CrtTimeTenth = 0;
	directory.DIR_CrtTime = 0;
	directory.DIR_CrtDate = 0;
	directory.DIR_LstAccDate = 0;
	directory.DIR_FstClusHI = 0;
	directory.DIR_WrtTime = 0;
	directory.DIR_WrtDate = 0;
	directory.DIR_FstClusLO = 3;
	directory.DIR_FileSize = arq_size;


goto_0:
	cluster_start = buffer_boot_record.BPB_RootClus;

	fta_region(fp2,cluster_start,0x0FFFFFF8);	// Valor na Tabala de alocaçoa de arquivos


	byte_start =buffer_boot_record.BPB_BytsPerSec
				*((buffer_boot_record.BPB_RsvdSecCnt 
				+(buffer_boot_record.BPB_FATSz32
				*buffer_boot_record.BPB_NumFATs)
				+((cluster_start - 2) * buffer_boot_record.BPB_SecPerClus)));

	

	//FAT_DIR_ENTRY_SIZE * N



	fseek(fp2,byte_start,SEEK_SET);
	if((fwrite(&directory,sizeof(char),FAT_DIR_ENTRY_SIZE,fp2))!=FAT_DIR_ENTRY_SIZE){
	printf("Erro ao escrever directory/file");
	exit(1);
	}
	rewind(fp2);
	memset(dados,0,sizeof(dados));

	resto_cluster = 3;

	
goto_1:
	if(total_cluster == 0 && resto_cluster ==0)goto goto_4;
	else if(total_cluster !=0)goto goto_3;
	else if(resto_cluster !=0)goto goto_2;
	

goto_2:

	size_copy = resto_cluster;
	resto_cluster = 0;
	goto start;
goto_3:

	size_copy = buffer_boot_record.BPB_BytsPerSec * buffer_boot_record.BPB_SecPerClus;
	cluster_start ++;
	total_cluster--;
start:
	

	if((fread(dados,sizeof(char),size_copy,fp1))!=size_copy){
	printf("Erro ao copiar o arquivo");
	exit(1);
	}
	
	
	 byte_start =buffer_boot_record.BPB_BytsPerSec
				*((buffer_boot_record.BPB_RsvdSecCnt 
				+(buffer_boot_record.BPB_FATSz32
				*buffer_boot_record.BPB_NumFATs)
				+((cluster_start - 2) * buffer_boot_record.BPB_SecPerClus)));

	size_colar = buffer_boot_record.BPB_BytsPerSec * buffer_boot_record.BPB_SecPerClus;

	fseek(fp2,byte_start,SEEK_SET);
	if((fwrite(dados,sizeof(char),size_colar,fp2))!=size_colar){
	printf("Erro ao escrever no arquivo");
	exit(1);
	}
	rewind(fp2);
	memset(dados,0,size_colar);
	if(total_cluster !=0)goto goto_1;
goto_4:

	printf("Feito!\n"); 
	
	free(dados);
	fclose(fp1);
	fclose(fp2);
	exit(1);
	return 0;


}


static size_t file_size_bytes(FILE *fp){

	size_t count = -1; 
	do{ 
		fgetc(fp); 
		count++;

	}while(!feof(fp));
	rewind(fp);

	return count;
}


static void fta_region(FILE *fp,unsigned int end_offset, unsigned int value){

	unsigned int start_byte = (buffer_boot_record.BPB_RsvdSecCnt 
			     	*buffer_boot_record.BPB_BytsPerSec)
			     	+(end_offset*4); 
	
	
	fseek(fp,start_byte,SEEK_SET);
	fprintf(fp,"%d",value &0x0FFFFFFF);
	rewind(fp);

}
