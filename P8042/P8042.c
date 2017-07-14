
/*
 * Nelson Sapalo da Silva Cole
 * (nelsoncole72@gmail.com)
 *
 * Lubango, 28 de Junho de 2017
 *
 *
 * Código em construção
 *
 */




#define outanyb(p)\ 
__asm__ __volatile__(\
"outb %%al,%0"::"dN"((p)) :"eax"\
)  /* Valeu Fred */


void kbdc_wait(BYTE type){
	if(type==0){

                  while(!inb(0x64)&1)outanyb(0x80);
            }

	else {

                 while(inb(0x64)&2)outanyb(0x80);

      }

}



void mouse_write(BYTE write){

	kbdc_wait(1);
	outb(0x64,0xD4);
	kbdc_wait(1);
	outb(0x60,write);

}


BYTE mouse_read(){


	kbdc_wait(0);
	return inb(0x60);

}


/* configurando o controlador PS/2, 

   e activar a segunda porta PS/2 (mouse)

*/
void P8042_install(){
	
	BYTE status;


/* Desativar dispositivos PS/2 , isto envita que os dispositivos PS/2 envie dados no momento da configuração.
 */

  	kbdc_wait(1);
	outb(0x64,0xAD);  // Desativar a primeira porta PS/2
	kbdc_wait(1);
	outb(0x64,0xA7);  /* Desativar a segunda porta PS/2, hahaha por default ela já vem desativada, só para constar */

	kbdc_wait(1);    
	outb(0x64,0x20);     // defina a leitura do byte actual de configuração do controlador PS/2

	kbdc_wait(0);
	status=inb(0x60)|2;  /* Activar o segundo despositivo PS/2, modificando o status de configuração do controlador PS/2. 

				Lembrando que o bit 1 é o responsável por habilitar, desabilitar o segundo despositivo PS/2  ( o rato). Só 					para constar se vedes aqui fizemos duas coisas lemos ao mesmo tempo modificamos o byte 
				de configuração do controlador PS/2 
			     */

	
	kbdc_wait(1);
	outb(0x64,0x60);  // defina, a escrita  de byte de configuração do controlador PS/2

	
	kbdc_wait(1);
	outb(0x60,status);  // devolvemos o byte de configuração modificado

	// Agora temos dois dispositivos sereais teclado e mouse (PS/2).
	


	kbdc_wait(1);
	outb(0x64,0xAE);   // Activar a primeira porta PS/2

	kbdc_wait(1);
	outb(0x64,0xA8);  // activar a segunda porta PS/2

 kbdc_wait(1);
  //espera 

/* NOTA. Esta configuração discata do teste do controlador PS/2 e de seus dispositivos. Depois façamos a configuração decente e minuciosa do P8042.
 
*/

}
