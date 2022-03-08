/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int main()
{
    // ========== Check: ReadChar, PrintChar ==========
    char c = 't';

    PrintChar('N'); PrintChar('h'); PrintChar('a'); PrintChar('p'); PrintChar(' ');
    PrintChar('m'); PrintChar('o'); PrintChar('t'); PrintChar(' ');
    PrintChar('k'); PrintChar('i'); PrintChar(' ');
    PrintChar('t'); PrintChar('u'); PrintChar(':'); PrintChar(' ');

    c = ReadChar();

    PrintChar('K'); PrintChar('i'); PrintChar(' ');
    PrintChar('t'); PrintChar('u'); PrintChar(' ');
    PrintChar('v'); PrintChar('u'); PrintChar('a'); PrintChar(' ');
    PrintChar('n'); PrintChar('h'); PrintChar('a'); PrintChar('p'); PrintChar(' ');
    PrintChar('l'); PrintChar('a'); PrintChar(':'); PrintChar(' '); PrintChar('"'); PrintChar(c); PrintChar('"');

    PrintChar('\n');
    PrintChar('B'); PrintChar('y'); PrintChar('e');
    
	Halt();
    // ==================================================


    // ========== Check: ReadString, PrintString ==========
    // char *buffer;
    // PrintString("Nhap mot chuoi: ");
    // ReadString(buffer, 10);
    
    // PrintString("Chuoi vua nhap la: \"");
    // PrintString(buffer);
    // PrintString("\"\nBye");

    // Halt();
    // ==================================================
    /* not reached */
}
