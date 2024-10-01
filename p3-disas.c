/*
 * CS 261 PA3: Mini-ELF disassembler
 *
 * Name: Charlie Virag
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p3-disas.h"

//============================================================================
// Functions I created - charlie virag
void get_reg(y86_rnum_t reg_num)
{
    switch (reg_num)
    {
    case RAX: // 0
        printf("%%rax");
        break;

    case RCX: // 1
        printf("%%rcx");
        break;

    case RDX: // 2
        printf("%%rdx");
        break;

    case RBX: // 3
        printf("%%rbx");
        break;

    case RSP: // 4
        printf("%%rsp");
        break;

    case RBP: // 5
        printf("%%rbp");
        break;

    case RSI: // 6
        printf("%%rsi");
        break;

    case RDI: // 7
        printf("%%rdi");
        break;

    case R8: // 8
        printf("%%r8");
        break;

    case R9: // 9
        printf("%%r9");
        break;
    
    case R10: // 10
        printf("%%r10");
        break;

    case R11: // 11
        printf("%%r11");
        break;

    case R12: // 12
        printf("%%r12");
        break;

    case R13: // 13
        printf("%%r13");
        break;

    case R14: // 14
        printf("%%r14");
        break;
    }
}

void spacing(int size)
{
    for (int i = 0; i < size; i++)
    {
        printf(" ");
    }
    // add the bar and spaces
    printf(" |   ");
}

//============================================================================
void usage_p3 ()
{
    printf("Usage: y86 <option(s)> mini-elf-file\n");
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("  -a      Show all with brief memory\n");
    printf("  -f      Show all with full memory\n");
    printf("  -s      Show the program headers\n");
    printf("  -m      Show the memory contents (brief)\n");
    printf("  -M      Show the memory contents (full)\n");
    printf("  -d      Disassemble code contents\n");
    printf("  -D      Disassemble data contents\n");
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

//============================================================================
bool parse_command_line_p3 (int argc, char **argv,
        bool *header, bool *segments, bool *membrief, bool *memfull,
        bool *disas_code, bool *disas_data, char **file)
{
    
    // Initialize variables
    int opt;
    opterr = 0;
    bool H_flag = false;
    bool s_flag = false;
    bool m_flag = false;
    bool M_flag = false;
    bool d_flag = false;
    bool D_flag = false;
    char *option_string = "+hHafsmMdD";

    // Parse command line arguments
    while((opt = getopt(argc, argv, option_string)) != -1)
    {
        switch(opt)
        {
            case 'h':   // Display usage information and reset all variables and flags
                        usage_p3();
                        *header = false;
                        *segments = false;
                        *membrief = false;
                        *memfull = false;
                        *disas_code = false;
                        *disas_data = false;
                        *file = NULL;
                        return true;

            case 'H':   // Display the ELF header, only once
                        if (H_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *header = true;
                        H_flag = true;
                        break;

            case 'a':   // Display all information: ELF header, program segments, memory map (brief)
                        if (H_flag || s_flag || m_flag || M_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *header = true;
                        *segments = true;
                        *membrief =true;
                        H_flag = true;
                        s_flag = true;
                        m_flag = true;
                        break;

            case 'f':   // Display all information: ELF header, program segments, memory map (full)
                        if (H_flag || s_flag || M_flag || m_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *header = true;
                        *segments = true;
                        *memfull = true;
                        H_flag = true;
                        s_flag = true;
                        M_flag = true;
                        break;

            case 's':   // Display the program segments, only once
                        if (s_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *segments = true;
                        s_flag = true;
                        break;

            case 'm':   // Display the virtual memory (brief), only once
                        if (m_flag || M_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *membrief = true;
                        m_flag = true;
                        break;

            case 'M':   // Display the virtual memory (full), only once
                        if (M_flag || m_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *memfull = true;
                        M_flag = true;
                        break;

            case 'd':   // Disassemble code contents
                        if (d_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *disas_code = true;
                        d_flag = true;
                        break;

            case 'D':   //  Disassemble data contents
                        if (D_flag)
                        {
                            usage_p3();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *disas_code = false;
                            *disas_data = false;
                            *file = NULL;
                            return false;
                        }
                        *disas_data = true;
                        D_flag = true;
                        break;

            default:    // Incorrect arguments, display usage and reset all flags and variables
                        usage_p3();
                        *header = false;
                        *segments = false;
                        *membrief = false;
                        *memfull = false;
                        *disas_code = false;
                        *disas_data = false;
                        *file = NULL;
                        return false;;

        }
    }

    // Ensure exactly one file name at the end of the command line
    if (optind != argc - 1)
    {
        usage_p3();
        return false;
    }

    *file = argv[optind];
    return true;
}

//============================================================================

y86_inst_t fetch (y86_t *cpu, memory_t memory)
{
    y86_inst_t ins;
    if (cpu == NULL || memory == NULL)
    {
        ins.type = INVALID;
        cpu->stat = INS;
        return ins;
    }
    // Initialize the instruction
    memset( &ins , 0 , sizeof(y86_inst_t) ); // Clear all fields i=on instr.
    ins.type = INVALID; // Invalid instruction until proven otherwise
    address_t instAddr , *pa;
    uint64_t *pv ;
    int64_t *pd ;
    uint8_t byte0 , byte1 ;
    y86_rnum_t ra , rb ;
    size_t size ; // size of the instruction being fetched
    instAddr = cpu->pc ; // Where to fetch instruction from
    
    if( cpu->pc >= MEMSIZE ) // Is it outside the Address Space?
    {
        cpu->stat = ADR ; // an invalid fetch address
        return ins ; // an INVALID instruction
    }

    byte0 = memory[ instAddr ] ; // fetch 1st byte of instr.
    ins.opcode = byte0 ;
    switch( byte0 ) // Inspect the opcode
    {
        case 0x00:
            if ((memory[cpu->pc] & 0x0F) != 0x00)
            {
                ins.type = INVALID;
                cpu->stat = INS;
                return ins;
            }
            ins.type = HALT;
            ins.size = 1;
            ins.opcode = memory[cpu->pc];
            cpu->stat = HLT;
            break;

        case 0x10:
            if ((memory[cpu->pc] & 0x0F) != 0x00)
            {
                ins.type = INVALID;
                cpu->stat = INS;
                return ins;
            }
            ins.type = NOP;
            ins.size = 1;
            ins.opcode = memory[cpu->pc];
            break;

        case 0x20: // cmovXX TA helped
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
            size = 2 ;
            // is instr too big to fit in remaining memory
            if((cpu->pc + 2) >= MEMSIZE)
            {
                cpu->stat = ADR ; // an invalid fetch address
                return ins ; // invalid instruction
            }
            byte1 = memory[ instAddr + 1 ] ; // fetch 2nd byte
            ra = (byte1 >> 4) & 0x0F;
            rb = byte1 & 0xF;
            if( ra >= BADREG || rb >= BADREG ) // invalid registers
            {
                cpu->stat = INS ; // invalid instruction encountered
                return ins ; // invalid instruction
            }
            ins.size = size ;
            ins.type = CMOV ;
            ins.cmov = byte0 & 0x0F;
            ins.ra = ra ; ins.rb = rb ;
            break;

        case 0x30: // irmovq
            size = 10 ;
            ins.size = size;
            if ((instAddr + size >= MEMSIZE))
            {
                cpu->stat = ADR;
                return ins;
            }
            ins.type = IRMOVQ;
            pv = (uint64_t *) (memory + instAddr + 2); // &of 64-bit offset
            ins.value = *pv;
            byte1 = memory[instAddr + 1];
            ra = (byte1 >> 4) & 0xF;
            rb = byte1 & 0x0F;
            ins.ra = ra;
            ins.rb = rb;
            if (ra != 0x0F)
            {
                cpu->stat = INS;
                ins.type = INVALID;
                return ins;
            }
            break;
            
        case 0x40: // rmmovq TA helped
            size = 10 ;
            ins.size = size;
            if ((instAddr + size >= MEMSIZE))
            {
                cpu->stat = ADR;
                return ins;
            }
            ins.type = RMMOVQ;
            pd = (int64_t *) (memory + instAddr + 2); // &of 64-bit offset
            pv = (uint64_t *) (memory + instAddr + 2); // &of 64-bit offset
            ins.d = *pd;
            ins.value = *pd;
            byte1 = memory[instAddr + 1];
            ra = (byte1 >> 4) & 0xF;
            rb = byte1 & 0x0F;
            ins.ra = ra;
            ins.rb = rb;
            break;

        case 0x50: // mrmovq TA helped
            size = 10;
            ins.size = size;
            ins.type = MRMOVQ;
            pd = (int64_t *) (memory + instAddr + 2); // &of 64-bit offset
            pv = (uint64_t *) (memory + instAddr + 2); // &of 64-bit offset
            ins.d = *pd;
            ins.value = *pd;
            byte1 = memory[instAddr + 1];
            ra = (byte1 >> 4) & 0xF;
            rb = byte1 & 0x0F;
            ins.ra = ra;
            ins.rb = rb;
            break;

        case 0x60: // addq , subq , andq , xorq
        case 0x61:
        case 0x62:
        case 0x63:
            size = 2 ;
            ins.size = size;
            byte1 = memory[instAddr + 1];
            ins.type = OPQ;
            ra = byte1 >> 4;
            ins.ra = ra;
            rb = byte1 & 0x0F;
            ins.rb = rb;
            if (ins.rb >= 15 || ins.ra >= 15)
            {
                ins.type = INVALID;
                cpu->stat = INS;
                break;
            }
            ins.op = byte0 & 0x0F;
            break;

        case 0x70: // jXX
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
        case 0x75:
        case 0x76:
            size = 9;
            ins.size = size;
            byte1 = memory[instAddr + 1];
            ins.type = JUMP;
            ins.jump = byte0 & 0x0F;
            if (cpu->pc + 9 > MEMSIZE)
            {
                cpu->stat = ADR;
                return ins;
                break;
            }
            pa = (address_t *) (memory + cpu->pc + 1);
            ins.dest = *pa; 
            break;

        case 0x80: // call
            size = 9;
            ins.size = size;
            if (cpu->pc + 9 > MEMSIZE)
            {
                cpu->stat = ADR;
                return ins;
                break;
            }
            ins.type = CALL;
            pa = (address_t *) (memory + cpu->pc + 1);
            ins.dest = *pa; 
            break;

        case 0x90: // ret
            size = 1;
            ins.size = size;
            ins.type = RET;
            break;

        case 0xA0: // pushq
            size = 2 ;
            ins.size = size;
            if (cpu->pc + 9 > MEMSIZE)
            {
                cpu->stat = ADR;
                return ins;
                break;
            }
            ins.type = PUSHQ;
            byte1 = memory[instAddr + 1];
            ra = byte1 >> 4;
            ins.ra = ra;
            rb = byte1 & 0x0F;
            ins.rb = rb;
            if (rb != 0xF)
            {
                cpu->stat = INS;
                ins.type = INVALID;
                return ins;
            }
            break;

        case 0xB0: // popq
            size = 2 ;
            ins.size = size;
            if (cpu->pc + 9 > MEMSIZE)
            {
                cpu->stat = ADR;
                return ins;
                break;
            }
            ins.type = POPQ;
            byte1 = memory[instAddr + 1];
            ra = byte1 >> 4;
            ins.ra = ra;
            rb = byte1 & 0x0F;
            ins.rb = rb;
            if (rb != 0xF)
            {
                cpu->stat = INS;
                ins.type = INVALID;
                return ins;
            }
            break;

        default:
            ins.type = INVALID ; // INVALID instruction
            cpu->stat = INS ;
            break ;
    }
    return ins;
}

//============================================================================
void disassemble (y86_inst_t inst)
{
    switch (inst.type)
    {
    case HALT:
        printf("halt");
        break;

    case NOP:
        printf("nop");
        break;

    case CMOV:
        switch (inst.cmov)
        {
        case RRMOVQ:
            printf("rrmovq ");
            break;

        case CMOVLE:
            printf("cmovle ");
            break;

        case CMOVL:
            printf("cmovl ");
            break;

        case CMOVE:
            printf("cmove ");
            break;

        case CMOVNE:
            printf("cmovne ");
            break;

        case CMOVGE:
            printf("cmovge ");
            break;

        case CMOVG:
            printf("cmovg ");
            break;

        case BADCMOV:
            printf("badmov ");
            break;
        }
        get_reg(inst.ra);
        printf(", ");
        get_reg(inst.rb);
        break;

    case IRMOVQ:
        printf("irmovq ");
        printf("0x%lx, ", inst.value);
        get_reg(inst.ra);
        get_reg(inst.rb);
        break;

    case RMMOVQ:
        if (inst.rb == 15)
        {
            printf("rmmovq ");
            get_reg(inst.ra);
            printf(", 0x%lx", inst.value);
            break;
        }
        else
        {
            printf("rmmovq ");
            get_reg(inst.ra);
            printf(", 0x%lx(", inst.value);
            get_reg(inst.rb);
            printf(")");
            break;
        }
        break;

    case MRMOVQ:
        if (inst.rb == 15)
        {
            printf("mrmovq ");
            printf("0x%lx, ", inst.value);
            get_reg(inst.ra);
            break;
        }
        else
        {
            printf("mrmovq ");
            printf("0x%lx(", inst.value);
            get_reg(inst.rb);
            printf("), ");
            get_reg(inst.ra);
            break;
       }
        break;

    case OPQ:
        switch (inst.op)
        {
        case ADD:
            printf("addq ");
            break;

        case SUB:
            printf("subq ");
            break;

        case AND:
            printf("andq ");
            break;

        case XOR:
            printf("xorq ");
            break;

        case BADOP:
            printf("badop ");
            break;
        }
        get_reg(inst.ra);
        printf(", ");
        get_reg(inst.rb);
        break;

    case JUMP:
        switch (inst.jump)
        {
        case JMP:
            printf("jmp ");
            break;

        case JLE:
            printf("jle ");
            break;

        case JL:
            printf("jl ");
            break;

        case JE:
            printf("je ");
            break;

        case JNE:
            printf("jne ");
            break;

        case JGE:
            printf("jge ");
            break;

        case JG:
            printf("jg ");
            break;

        case BADJUMP:
            printf("badjump ");
            break;;
        }
        printf("0x%lx", inst.dest);
        break;

    case CALL:
        printf("call ");
        printf("0x%lx", inst.dest);
        break;

    case RET:
        printf("ret");
        break;

    case PUSHQ:
        printf("pushq ");
        get_reg(inst.ra);
        break;

    case POPQ:
        printf("popq ");
        get_reg(inst.ra);
        break;

    case INVALID:
    default:
        printf("invalid");
        break;
    }
}

//============================================================================
void disassemble_code (memory_t memory, elf_phdr_t *phdr, elf_hdr_t *hdr)
{
    int size = 0;

    if (memory == NULL || phdr == NULL || hdr == NULL)
    {
        return;
    }

    y86_t cpu; // CPU struct to store "fake" PC
    y86_inst_t ins; // struct to hold fetched instruction
    printf("  0x%03x:                      | .pos 0x%03x code\n", phdr->p_vaddr, phdr->p_vaddr);
    cpu.pc = phdr->p_vaddr; // start at beginning of the segment
    // iterate through the segment one instruction at a time
    while ( cpu.pc < (phdr->p_vaddr + phdr->p_filesz))
    {
        if (cpu.pc == hdr->e_entry)
        {
            printf("  0x%03lx:                      | _start:\n", cpu.pc);
        }
        ins = fetch ( &cpu , memory );      // fetch instruction
        
        if (ins.type >= INVALID)
        {
            printf("Invalid opcode: 0x%x\n", memory[cpu.pc]);
            return;
        }
        else
        {
            // print current address and raw hex bytes of instruction
            printf("  0x%03lx: ", cpu.pc);
        }
        // print current address and raw hex bytes of instruction
        for (int i = 0; i < ins.size; i++)
        {
            printf("%02x", memory[cpu.pc + i]);
        }
        
        size = 20 - ((cpu.pc + ins.size) - cpu.pc) * 2; // difference of total spaces and taken spaces
        spacing(size);
        disassemble ( ins );              // print assembly code
        cpu.pc += ins.size;                // update PC (next instruction)
        printf("\n");
    }
}

//============================================================================
void disassemble_data (memory_t memory, elf_phdr_t *phdr)
// Help recieved in TA hours to complete this function
{

    if (memory == NULL)
    {
        return;
    }
    y86_t cpu; // CPU struct to store "fake" PC
    printf("  0x%03x:                      | .pos 0x%03x data\n", phdr->p_vaddr, phdr->p_vaddr);
    cpu.pc = phdr->p_vaddr;

    // iterate through the segment one instruction at a time
    while (cpu.pc < (phdr->p_vaddr + phdr->p_filesz))
    {
        // boolean to check if the result is zero
        bool zero = true;
        printf("  0x%03lx: ", cpu.pc);
        for (int i = cpu.pc; i < cpu.pc + 8; i++)
        {
            if (zero && memory[i] != 0)
            {
                // set the zero to false because there are no zeroes
                zero = false;
            }
            uint8_t dest = memory[i];
            printf("%02x", dest);
        }
        printf("     |   .quad 0x");

        // go backwards to print the numbers reverse of their respective endian
        for (int i = cpu.pc + 7; i >= cpu.pc; i--)
        {
            if (memory[i] > 0)
            {
                printf("%x", memory[i]);
            }
        }
        if (zero)
        {
            // if zero was never set to false then print the zero out
            printf("%x", 0);
        }
        printf("\n");
        cpu.pc += 8;
    }
}

//============================================================================
// Help recieved in TA hours to complete this function
void disassemble_rodata (memory_t memory, elf_phdr_t *phdr)
{
    int spaces = 0;

    if (memory == NULL || phdr == NULL)
    {
        return;
    }
    printf("  0x%03x:                      | .pos 0x%03x rodata\n", phdr->p_vaddr, phdr->p_vaddr);
    y86_t cpu; // CPU struct to store "fake" PC
    uint64_t dest;
    bool do_print = true;
    cpu.pc = phdr->p_vaddr;
    while ( cpu.pc < (phdr->p_vaddr + phdr->p_filesz))
    {
        printf("  0x%03lx: ", cpu.pc);
        int size = 0;
        for (int i = cpu.pc; i < cpu.pc + 10; i++)
        {
            printf("%02x", memory[cpu.pc + size]);
            size++;
            if (memory[cpu.pc + size - 1] == 0x00)
            {
                break;
            }
        }

        spaces = 20 - ((cpu.pc + size) - cpu.pc) * 2; // difference of total spaces and taken spaces
        spacing(spaces);

        dest = cpu.pc;
        // if the print boolean is true, print out the string and set print to false.
        if (do_print)
        {
            printf(".string \"");
            while (memory[dest] != 0x00)
            {
                printf("%c", memory[dest++]);
            }
            printf("\"");
            do_print = false;
        }

        // check if less than 10 were reached
        if (size < 10)
        {
            do_print = true; // set print back to true
        }

        cpu.pc += size;
        printf("\n");
    }
}
//============================================================================
