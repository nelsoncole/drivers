/*
 * Nelson Sapalo da Silva Cole
 * (nelsoncole72@gmail.com)
 *
 * Lubango, 28 de Junho 2017
 *
 * Código em construção
 *
 */


// o RTC também pode gerar diferentes taxa de frequência

#include <io.h>
#include <rtc.h>


BYTE status_c;


void disable_NMI(){

    outb(0x70,inb(0x70)|0x80);

}

void enable_NMI(){

    outb(0x70,inb(0x70) & 0x7F);
}
 


void rtc_install(){

    BYTE status;

    cli();
    outb(0x70,0x8B);  // registro status B e desabilita NMI
    status = inb(0x71); // ler actual valor de status B
    outb(0x70,0x8B); // registro status B
    outb(0x71, status | 0x67); /* Hablita interrupções periódica IRQ8 bit 6
,
                               hablita interrupção de alarme bit 5,
                              caledário em formato binário bit 2,
                              formato 24h bit 1  */

    enable_NMI();
    sti();

}



// código c para o manipulador IRQ8 offset no IDT vetor 40

void rtc_irq(){

 outb(0x70,0x0C); 
 
 status_c = inb(0x71); // é importante ler do registro status C

}
