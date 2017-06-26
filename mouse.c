/* 
 * Nelson Cole
 * (nelsoncole72@gmail.com)
 * Lubango, 26 de Junho de 2017.
 *
 *
 * Não leve muito a cério, este código está em construção 
 * 
*/



#include <io.h>
#include <mouse.h>


#define outanyb(p)\ 
__asm__ __volatile__(\
"outb %%al,%0"::"dN"((p)) :"eax"\
)  /* Valeu Fred */

BYTE delta_x,delta_y;
WORD posicao_mouse;





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


void P8042_install(){
	
	BYTE status;


/* Desativar dispositivos PS/2 , isto envita que os dispositivos PS/2 envie dados no momento da configuração.
 */

  	kbdc_wait(1);
	outb(0x64,0xAD);  // Desativar a primeira porta PS/2
	kbdc_wait(1);
	outb(0x64,0xA7);  /* Desativar a segunda porta PS/2, hahaha por default ela já vem desativada, só para constar */

	kbdc_wait(1);   // Limpar o buffer, só por diversão

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

	mouse_write(0xF6);  // dizemos ao rato que use as configurações padrão
	mouse_write(0xF4);  // agora habilitamos o rato.

/* NOTA. Esta configuração discarta do teste do controlador PS/2 e de seus dispositivos. Depois façamos a configuração decente e minuciosa do P8042.
 
*/

}



void cursor_mouse(){

	cli();
	outb(0x3D4,0x0A);
     	outb(0x3D5,0);
	outb(0x3D4,0xB);
     	outb(0x3D5,24);
	sti();

}


void mouse_install(){

     	cursor_mouse();
	irq_enable(12);

}

void update_mouse(){

/* esta função é de improvisação estar em construção*/

posicao_mouse = delta_x + (80 * delta_y);

     cli();
     outb(0x3D4,0x0E);
     outb(0x3D5,(BYTE)(posicao_mouse >> 8) &0xFF);
     outb(0x3D4,0x0F);
     outb(0x3D5,(BYTE)(posicao_mouse )&0xFF);
     sti();

}



void mouse_irq(){
	
if(inb(0x64)&1) // emprovisaremos por enquanto
{

	inb(0x60); // aprimeira leitura é do status do rato.

	delta_x = inb(0x60);
	
	delta_y = inb(0x60);

	update_mouse();
	
}
	
		

}
