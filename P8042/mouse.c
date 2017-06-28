/* 
 * Nelson Cole
 * (nelsoncole72@gmail.com)
 * Lubango, 28 de Junho de 2017.
 *
 *
 * Código em construção
 * 
*/



#include <io.h>
#include <mouse.h>



BYTE mouse_status,delta_x,delta_y,buffer_mouse[3];

int count_mouse=0;

WORD posicao_mouse;



void cursor_mouse(){

	cli();
	outb(0x3D4,0x0A);
     	outb(0x3D5,0);
	outb(0x3D4,0xB);
     	outb(0x3D5,16);
	sti();

}


void mouse_install(){

	

	mouse_write(0xFF);   // reseta o mouse
	mouse_write(0xF6);  // dizemos ao rato que use as configurações padrão
	mouse_write(0xF4);  // agora habilitamos o rato.
	while(!0xFA)mouse_read();
	cursor_mouse();

	irq_enable(12);

}

void update_mouse(){

/* esta função é de improvisação estar em construção*/

posicao_mouse = posicao_mouse + (delta_x);

     cli();
     outb(0x3D4,0x0E);
     outb(0x3D5,(BYTE)(posicao_mouse >> 8) &0xFF);
     outb(0x3D4,0x0F);
     outb(0x3D5,(BYTE)(posicao_mouse )&0xFF);
     sti();

}



// Função para entrada no manipulador, IRQ12
void mouse_irq(){

	buffer_mouse[count_mouse++]=mouse_read();
	
if(count_mouse==3){

	mouse_status = buffer_mouse[0];

	delta_x = buffer_mouse[1];
	
	delta_y = buffer_mouse[2];

	update_mouse();

	count_mouse=0;
	
}
		

}
