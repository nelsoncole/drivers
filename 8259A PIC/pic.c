/*
 * Nelson Sapalo da Silva Cole
 * (nelsoncole72@gmail.com)
 *
 * Lubango, 28 de Junho de 2017
 *
 *
*/


#include <io.h>
#include <pic.h>

void pic_install(){

	// Envia ICW1 reset
	outb(0x20,0x11);	// reset PIC 1
	outb(0xA0,0x11);	// reset PIC 2

	// Envia ICW2 start novo PIC 1 e 2
	outb(0x21,0x20);	// PIC 1 localizado no IDT 38-32 
	outb(0xA1,0x28);	// PIC 2 localizado no IDT 47-40

	// Envia ICW3
	outb(0x21,0x04);	// IRQ2"mestre" conexão em cascata com o PIC 2
	outb(0xA1,0x02);  // IRQ9 "escravo"
	
	// Envia ICW4
	outb(0x21,0x01);
	outb(0xA1,0x01);

	// Desabilita todas as interrupcões
	// OCW1
	outb(0x21,0xFF);
	outb(0xA1,0xFF);

}
