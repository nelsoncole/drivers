/*

  Função, compara duas strings e retorna 1 se é verdadeiro, retorna 0 se for falso.

*/

unsigned char strcmpb (void* s1, void* s2,unsigned count)
{
        unsigned char ret;
        __asm__ __volatile__ ("cld;\
                               rep;\
                               cmpsb;\
                               je 1f;\
                               movb $0,%%al; \
                               jmp 2f;\
                               1: movb $1,%%al;\
                               2:"\
                               :"=aN"(ret):"c"(count),"D"(s1),"S"(s2));
        return ret;
}
