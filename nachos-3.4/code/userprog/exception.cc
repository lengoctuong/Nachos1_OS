// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}

char* User2System(int virtAddr,int limit)
{
    int i;// index
    int oneChar;
    char* kernelBuf = NULL;
    kernelBuf = new char[limit +1];//need for terminal string
    if (kernelBuf == NULL)
        return kernelBuf;
    
    memset(kernelBuf,0,limit+1);
    //printf("\n Filename u2s:");
    for (i = 0 ; i < limit ;i++)
    {
        machine->ReadMem(virtAddr+i,1,&oneChar);
        kernelBuf[i] = (char)oneChar;
        //printf("%c",kernelBuf[i]);
        if (oneChar == 0)
            break;
    }

    return kernelBuf;
}

int System2User(int virtAddr,int len,char* buffer)
{
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0 ;
    
    do {
        oneChar= (int) buffer[i];
        machine->WriteMem(virtAddr+i,1,oneChar);
        i ++;
    } while(i < len && oneChar != 0);
    
    return i;
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    switch (which)
    {
    case NoException:
        return;

    case PageFaultException:
        DEBUG('a', "\nNo valid translation found!\n");
        printf("\nNo valid translation found!\n");
   	    interrupt->Halt();
        break;

    case ReadOnlyException:
        DEBUG('a', "\nWrite attempted to page marked!\n");
        printf("\nWrite attempted to page marked!\n");
   	    interrupt->Halt();
        break;
    
    case BusErrorException:
        DEBUG('a', "\nTranslation resulted in an invalid physical address!\n");
        printf("\nTranslation resulted in an invalid physical address!\n");
   	    interrupt->Halt();
        break;
    
    case AddressErrorException:
        DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space!\n");
        printf("\nUnaligned reference or one that was beyond the end of the address space!\n");
   	    interrupt->Halt();
        break;

    case OverflowException:
        DEBUG('a', "\nInteger overflow in add or sub!\n");
        printf("\nInteger overflow in add or sub!\n");
   	    interrupt->Halt();
        break;

    case IllegalInstrException:
        DEBUG('a', "\nUnimplemented or reserved instr!\n");
        printf("\nUnimplemented or reserved instr!\n");
   	    interrupt->Halt();
        break;

    case NumExceptionTypes:
        DEBUG('a', "\nNumber exception types!\n");
        printf("\nNumber exception types!\n");
   	    interrupt->Halt();
        break;

    case SyscallException:
        switch (type)
        {
        case SC_Halt:
            DEBUG('a', "\nShutdown, initiated by user program!\n");
            printf("\nShutdown, initiated by user program!\n");
            interrupt->Halt();
            return;

        case SC_ReadChar:
            //Input: Khong co
            //Output: Duy nhất 1 ky tu (char)
            //Cong dung: Doc mot ky tu tu nguoi dung nhap

            int maxBytes;
            maxBytes = 255;
            char* buffer;
            buffer = new char[255];
            int numBytes;
            numBytes = gSynchConsole->Read(buffer, maxBytes);

            if(numBytes > 1) //Neu nhap nhiều hơn 1 ky tu thi khong hop le
            {
                DEBUG('a', "ERROR: Chi duoc nhap duy nhat 1 ky tu!\n");
                printf("Chi duoc nhap duy nhat 1 ky tu!\n");
                machine->WriteRegister(2, 0);
            }
            else if(numBytes == 0)  //Ky tu rong
            {
                DEBUG('a', "ERROR: Ky tu rong!\n");
                printf("Ky tu rong!\n");
                machine->WriteRegister(2, 0);
            }
            else    //Chuoi vua lay có 1 ky tu, lay ky tu o index = 0, return vào thành ghi R2
            {
                char c = buffer[0];
                machine->WriteRegister(2, c);
            }

            delete buffer;
            IncreasePC();
            break;

        case SC_PrintChar:
            // Input: Ki tu(char)
			// Output: Ki tu(char)
			// Cong dung: Xuat mot ki tu la tham so arg ra man hinh

            char c;
            c = (char)machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
			gSynchConsole->Write(&c, 1); // In ky tu tu bien c, 1 byte
			IncreasePC();
			break;

        case SC_ReadString:
	    {
            // Input: Buffer(char*), do dai toi da cua chuoi nhap vao(int)
            // Output: Khong co
            // Cong dung: Doc vao mot chuoi voi tham so la buffer va do dai toi da

            int virtAddr, length;
            virtAddr = machine->ReadRegister(4); // Lay dia chi tham so buffer truyen vao tu thanh ghi so 4
            char* buffer;
            length = machine->ReadRegister(5); // Lay do dai toi da cua chuoi nhap vao tu thanh ghi so 5
            buffer = User2System(virtAddr, length); // Copy chuoi tu vung nho User Space sang System Space

            gSynchConsole->Read(buffer, length); // Goi ham Read cua SynchConsole de doc chuoi
            System2User(virtAddr, length, buffer); // Copy chuoi tu vung nho System Space sang vung nho User Space
            
            delete buffer; 
            IncreasePC(); // Tang Program Counter
            break;
	    }

	    case SC_PrintString:
        {
            // Input: Buffer(char*)
            // Output: Chuoi doc duoc tu buffer(char*)
            // Cong dung: Xuat mot chuoi la tham so buffer truyen vao ra man hinh

            int virtAddr;
            virtAddr = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
            char* buffer;
            buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu

            int length = 0;
            while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
            gSynchConsole->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
            
            delete buffer; 
            IncreasePC(); // Tang Program Counter 
            break;
        }

        // default:
        //     DEBUG('a', "\nUnexpected user mode exception!\n");
        //     printf("\nUnexpected user mode exception %d %d!\n", which, type);
        //     interrupt->Halt();
        //     break;
        }
        break;

    default:
        DEBUG('a', "\nUnexpected user mode exception!\n");
        printf("\nUnexpected user mode exception %d %d!\n", which, type);
        interrupt->Halt();
        break;
    }

    // if ((which == SyscallException) && (type == SC_Halt)) {
	// DEBUG('a', "Shutdown, initiated by user program.\n");
   	// interrupt->Halt();
    // } else {
	// printf("Unexpected user mode exception %d %d\n", which, type);
	// ASSERT(FALSE);
    // }
}
