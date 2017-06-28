/*
 * Nelson Sapalo da Silva Cole
 * (nelsoncole72@gmail.com)
 *
 Lubango, 28 de Junho 2017
 */




void disable_NMI(){

    outb(0x70,inb(0x70)|0x80);

}

void enable_NMI(){

    outb(0x70,inb(0x70) & 0x7F);
}
 


void rtc_install(){



}
