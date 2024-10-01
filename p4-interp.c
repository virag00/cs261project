/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Charlie Virag
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p4-interp.h"
//=======================================================================
// functions I created - charlie virag

// gets the condition codes based on passed instruction function
bool set_cond(y86_t CC, int ifun)
{
    switch (ifun)
    {
    case 0:
        return true;
    
    case 1:
        return ((CC.sf ^ CC.of) || CC.zf);

    case 2:
        return (CC.sf ^ CC.of);

    case 3:
        return CC.zf;

    case 4:
        return !CC.zf;

    case 5:
        return !(CC.sf ^ CC.of);

    case 6:
        return (!(CC.sf ^ CC.of) && !CC.zf);
    
    default:
        return false;
    }
}

// set condition codes based on 0's, negatives, or overflow
void check_condition_code(int64_t t, int64_t ra, int64_t rb, y86_op_t op, y86_t *cpu)
{
    cpu->zf = false;
    cpu->sf = false;
    cpu->of = false;
    
    switch (op)
    {
    case ADD:
        if (t == 0)
        {
            cpu->zf = true;
        }
        if (t < 0)
        {
            cpu->sf = true;
        }
        if ((ra > 0 && rb > 0 && t < 0) || (ra < 0 && rb < 0 && t > 0))
        {
            cpu->of = true;
        }
        break;

    case SUB:
        if (t == 0)
        {
            cpu->zf = true;
        }
        if (t < 0)
        {
            cpu->sf = true;
        }
        if ((ra > 0 && t > rb) || (ra < 0 && t < rb))
        {
            cpu->of = true;
        }
        break;

    case AND:
        if (t == 0)
        {
            cpu->zf = true;
        }
        if (t < 0)
        {
            cpu->sf = true;
        }
        break;

    case XOR:
        if (t == 0)
        {
            cpu->zf = true;
        }
        if (t < 0)
        {
            cpu->sf = true;
        }
        break;
    
    default:
        break;
    }
}

void reset_flags(bool *header, bool *segments, bool *membrief, bool *memfull,
                 bool *disas_code, bool *disas_data, bool *exec_normal,
                 bool *exec_debug, char **file) {
    usage_p4();
    *header = false;
    *segments = false;
    *membrief = false;
    *memfull = false;
    *disas_code = false;
    *disas_data = false;
    *exec_normal = false;
    *exec_debug = false;
    *file = NULL;
}

// returns the register in the cpu according to the instrection register
y86_register_t get_register_val(y86_rnum_t reg, y86_t *cpu)
{
    switch (reg)
    {
    case RAX:
        return cpu->rax;
    case RCX:
        return cpu->rcx;
    case RDX:
        return cpu->rdx;
    case RBX:
        return cpu->rbx;
    case RSP:
        return cpu->rsp;
    case RBP:
        return cpu->rbp;
    case RSI:
        return cpu->rsi;
    case RDI:
        return cpu->rdi;
    case R8:
        return cpu->r8;
    case R9:
        return cpu->r9;
    case R10:
        return cpu->r10;
    case R11:
        return cpu->r11;
    case R12:
        return cpu->r12;
    case R13:
        return cpu->r13;
    case R14:
        return cpu->r14;
    default:
        break;
    }
    return 0;
}

void set_register_value(y86_rnum_t reg, y86_t *cpu, y86_register_t value)
{
    switch (reg) {
        case RAX:
            cpu->rax = value;
            break;
        case RCX:
            cpu->rcx = value;
            break;
        case RDX:
            cpu->rdx = value;
            break;
        case RBX:
            cpu->rbx = value;
            break;
        case RSP:
            cpu->rsp = value;
            break;
        case RBP:
            cpu->rbp = value;
            break;
        case RSI:
            cpu->rsi = value;
            break;
        case RDI:
            cpu->rdi = value;
            break;
        case R8:
            cpu->r8 = value;
            break;
        case R9:
            cpu->r9 = value;
            break;
        case R10:
            cpu->r10 = value;
            break;
        case R11:
            cpu->r11 = value;
            break;
        case R12:
            cpu->r12 = value;
            break;
        case R13:
            cpu->r13 = value;
            break;
        case R14:
            cpu->r14 = value;
            break;
        default:
            break;
    }
}

//=======================================================================

void usage_p4 ()
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
    printf("  -e      Execute program\n");
    printf("  -E      Execute program (debug trace mode)\n");
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

//=======================================================================

bool parse_command_line_p4 
    (   int argc         , char **argv      ,
        bool *header     , bool *segments   , bool *membrief , 
        bool *memfull    , bool *disas_code , bool *disas_data ,
        bool *exec_normal, bool *exec_debug , char **file 
    )
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
    bool e_flag = false;
    bool E_flag = false;
    char *option_string = "+hHafsmMdDeE";

    // Parse command line arguments
    while((opt = getopt(argc, argv, option_string)) != -1)
    {
        switch(opt)
        {
            case 'h':   // Display usage information and reset all variables and flags
                        reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                        return true;

            case 'H':   // Display the ELF header, only once
                        if (H_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *header = true;
                        H_flag = true;
                        break;

            case 'a':   // Display all information: ELF header, program segments, memory map (brief)
                        if (H_flag || s_flag || m_flag || M_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
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
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
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
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *segments = true;
                        s_flag = true;
                        break;

            case 'm':   // Display the virtual memory (brief), only once
                        if (m_flag || M_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *membrief = true;
                        m_flag = true;
                        break;

            case 'M':   // Display the virtual memory (full), only once
                        if (M_flag || m_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *memfull = true;
                        M_flag = true;
                        break;

            case 'd':   // Disassemble code contents
                        if (d_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *disas_code = true;
                        d_flag = true;
                        break;

            case 'D':   //  Disassemble data contents
                        if (D_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        *disas_data = true;
                        D_flag = true;
                        break;

            case 'e':   // Execute program
                        if (e_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        if (E_flag) {
                            *exec_normal = false;
                            e_flag = true;
                            break;
                        }
                        *exec_normal = true;
                        e_flag = true;
                        break;

            case 'E':   // Execute program (debug trace mode)
                        if (E_flag)
                        {
                            reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                            return false;
                        }
                        if (e_flag)
                        {
                            *exec_normal = false;
                            *exec_debug = true;
                            E_flag = true;
                            break;
                        }
                        *exec_debug = true;
                        E_flag = true;
                        break;

            default:    // Incorrect arguments, display usage and reset all flags and variables
                        reset_flags(header, segments, membrief, memfull, disas_code, disas_data, exec_normal, exec_debug, file);
                        return false;;

        }
    }

    // Ensure exactly one file name at the end of the command line
    if (optind != argc - 1)
    {
        usage_p4();
        return false;
    }

    *file = argv[optind];
    return true;
}

//=======================================================================

void dump_cpu( const y86_t *cpu ) 
{
    printf("dump of Y86 CPU:\n");
    printf("  %%rip: %016lx   flags: SF%d ZF%d OF%d  ", cpu->pc, cpu->sf, cpu->zf, cpu->of);
    switch (cpu->stat)  // switch case to handle printing cpu status     
    {
    case AOK:
        printf("AOK\n"); // print AOK
        break;

    case HLT:
        printf("HLT\n"); // print HLT
        break;

    case ADR:
        printf("ADR\n"); // print ADR
        break;

    case INS:
        printf("INS\n"); // print INS
        break;
    
    default:
        break;
    }
    printf("  %%rax: %016lx    %%rcx: %016lx\n", cpu->rax, cpu->rcx);
    printf("  %%rdx: %016lx    %%rbx: %016lx\n", cpu->rdx, cpu->rbx);
    printf("  %%rsp: %016lx    %%rbp: %016lx\n", cpu->rsp, cpu->rbp);
    printf("  %%rsi: %016lx    %%rdi: %016lx\n", cpu->rsi, cpu->rdi);
    printf("   %%r8: %016lx     %%r9: %016lx\n", cpu->r8, cpu->r9);
    printf("  %%r10: %016lx    %%r11: %016lx\n", cpu->r10, cpu->r11);
    printf("  %%r12: %016lx    %%r13: %016lx\n", cpu->r12, cpu->r13);
    printf("  %%r14: %016lx\n\n", cpu->r14);
}

//=======================================================================

y86_register_t decode_execute(  y86_t *cpu , bool *cond , const y86_inst_t *inst ,
                                y86_register_t *valA 
                             )
{
    if (cpu == NULL || cond == NULL || inst == NULL || valA == NULL || cpu->stat == INS)
    {
        cpu->stat = INS;
        return 0;
    }

    // initialize variables
    y86_register_t valE = 0;
    y86_register_t valB = 0;
    y86_register_t ra = 0;
    y86_register_t rb = 0;

    switch (inst->type)
    {
    case HALT:
        // Exe
        cpu->stat = HLT;
        break;
    
    case NOP:
        break;

    case CMOV:
        // Dec
        *valA = get_register_val(inst->ra, cpu);
        // Exe
        valE = *valA;
        switch (inst->cmov)
        {
        case RRMOVQ:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVLE:
           *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVL:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVE:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVNE:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVGE:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;

        case CMOVG:
            *cond = (bool *)set_cond(*cpu, inst->cmov);
            break;
        
        default:
            cpu->stat = INS;
            break;
        }
        break;
    
    case IRMOVQ:
        // Exe
        valE = inst->value;
        break;

    case RMMOVQ:
        // Dec
        *valA = get_register_val(inst->ra, cpu);
        valB = get_register_val(inst->rb, cpu);
        // Exe
        valE = valB + inst->d;
        break;

    case MRMOVQ:
        // Dec
        valB = get_register_val(inst->rb, cpu);
        // Exe
        valE = valB + inst->d;
        break;

    case OPQ:
        // Dec
        *valA = get_register_val(inst->ra, cpu);
        rb = get_register_val(inst->rb, cpu);
        ra = get_register_val(inst->ra, cpu);
        // Exe
        switch (inst->op)
        {
        case ADD:
            valE = (int64_t) rb + (int64_t) ra;
            // cc check
            check_condition_code(valE, ra, rb, inst->op, cpu);
            break;
        
        case SUB:
            valE = (int64_t) rb - (int64_t) ra;
            // cc check
            check_condition_code(valE, ra, rb, inst->op, cpu);
            break;

        case AND:
            valE = (int64_t) rb & (int64_t) ra;
            // cc check
            check_condition_code(valE, ra, rb, inst->op, cpu);
            break;

        case XOR:
            valE = (int64_t) rb ^ (int64_t) ra;
            // cc check
            check_condition_code(valE, ra, rb, inst->op, cpu);
            break;

        default:
            cpu->stat = INS;
            break;
        }
        break;

    case JUMP:
        // Exe
        switch (inst->jump)
        {
        case JMP:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JLE:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JL:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JE:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JNE:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JGE:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;

        case JG:
            *cond = (bool *)set_cond(*cpu, inst->jump);
            break;
        
        default:
            *cond = false;
            cpu->stat = INS;
            break;
        }
        break;

    case CALL:
        // Dec
        valB = cpu->rsp;
        // Exe
        valE = valB - 8;
        break;

    case RET:
        // Dec
        *valA = cpu->rsp;
        valB = cpu->rsp;
        // Exe
        valE = valB + 8;
        break;

    case PUSHQ:
        // Dec
        *valA = get_register_val(inst->ra, cpu);
        // Exe
        valE = cpu->rsp - 8;
        break;

    case POPQ:
        // Dec
        *valA = cpu->rsp;
        // Exe
        valE = cpu->rsp + 8;
        break;

    default:
        cpu->stat = INS;
        break;
    }
    return valE;
}

//=======================================================================

void memory_wb_pc(  y86_t *cpu , memory_t memory , bool cond , 
                    const y86_inst_t *inst , y86_register_t  valE , 
                    y86_register_t  valA 
                 )
{
    y86_register_t valM = 0;
    y86_register_t valP = 0;

    switch (inst->type)
    {
    case HALT:
        cpu->sf = false;
        cpu->zf = false;
        cpu->of = false;
        cpu->pc += inst->size;
        break;

    case NOP:
        // PC
        cpu->pc += inst->size;
        break;

    case CMOV:
        // WB
        if (cond) set_register_value(inst->rb, cpu, valE);
        // PC
        cpu->pc += inst->size;
        break;

    case IRMOVQ:
        // WB
        set_register_value(inst->rb, cpu, valE);
        // PC
        cpu->pc += inst->size;
        break;

    case RMMOVQ:
        // Mem
        if (valE >= MEMSIZE || valE + 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc += inst->size;
            break;
        }
        memcpy(&memory[valE], &valA, 8);
        // PC
        cpu->pc += inst->size;
        printf("Memory write to 0x%04lx: 0x%lx\n", valE, valA);
        break;

    case MRMOVQ:
        // Mem
        if (valE >= MEMSIZE || valE + 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc += inst->size;
            break;
        }
        memcpy(&valM, &memory[valE], 8);
        // WB
        set_register_value(inst->ra, cpu, valM);
        // PC
        cpu->pc += inst->size;
        break;

    case OPQ:
        // WB
        set_register_value(inst->rb, cpu, valE);
        // PC
        cpu->pc += inst->size;
        break;

    case JUMP:
        // PC
        cpu->pc = cond ? inst->dest : cpu->pc + inst->size;
        break;

    case CALL:
        valP = cpu->pc + inst->size;
        // Mem
        if (valE >= MEMSIZE || valE + 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc = inst->dest;
            break;
        }
        memcpy(&memory[valE], &valP, 8);
        // *(&memory[valE]) = cpu->pc + inst->size;
        // WB
        cpu->rsp = valE;
        // PC
        cpu->pc = inst->dest;
        printf("Memory write to 0x%04lx: 0x%lx\n", cpu->rsp, valP);
        break;

    case RET:
        // Mem
        if (valA >= MEMSIZE || valA + 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc += inst->size;
            break;
        }
        memcpy(&valM, &memory[valA], 8);
        // *(&memory[valA]) = valM;
        // WB
        cpu->rsp = valE;
        // PC
        cpu->pc = valM;
        break;

    case PUSHQ:
        // MEM
        if (valE >= MEMSIZE || valE + 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc += inst->size;
            break;
        }
        memcpy(&memory[valE], &valA, 8);
        // *(&memory[valE]) = valA;

        // WB
        cpu->rsp = valE;
        // PC
        cpu->pc += inst->size;
        printf("Memory write to 0x%04lx: 0x%lx\n", valE, valA);
        break;

    case POPQ:
        // Mem
        // valM = memory[valA];
        if (valA >= MEMSIZE || valA+ 8 >= MEMSIZE)
        {
            cpu->stat = ADR;
            cpu->pc += inst->size;
            break;
        }
        memcpy(&valM, &memory[valA], 8);
        // WB
        cpu->rsp = valE;
        set_register_value(inst->ra, cpu, valM);
        // PC
        cpu->pc += inst->size;
        break;

    default:
        cpu->stat = INS;
        break;
    }
}

