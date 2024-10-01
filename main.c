/*
 * CS 261: P4 Y86 Interpreter Main driver
 *
 * Name: Charlie Virag 
 *    or risk losing points, and delays when you seek my help during office hours 
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "p1-check.h"
#include "p2-load.h"
#include "p3-disas.h"
#include "p4-interp.h"

int main (int argc, char **argv)
{
    elf_hdr_t   elf_head;
    memory_t    vMem;
    char        *file = NULL;
    bool        header = false;
    bool        segments = false;
    bool        membrief = false;
    bool        memfull = false;
    bool        disas_code = false;
    bool        disas_data = false;
    bool        exec_normal = false;
    bool        exec_debug = false;

    // Write code to call the parse function then
    // open the elf file, read its header, and react depending on whether -H was present

    // commands are parsed
    if (!parse_command_line_p4(argc, argv, &header, 
        &segments, &membrief, &memfull, &disas_code,
        &disas_data, &exec_normal, &exec_debug, &file ))
    {
        exit(EXIT_FAILURE);
    }

    // if file param false exit
    if (!file)
    {
        exit(EXIT_SUCCESS);
    }

    // open file, if file could not open exit failure w/ appropriate message
    FILE *input = fopen(file, "rb");
    if (!input)
    {
        puts("Failed to open File");
        exit(EXIT_FAILURE);
    }

    // allocate y86 virtual memory, if memory could not be allocated
    // exit w/ appropriate message
    vMem = malloc(MEMSIZE * sizeof(memory_t));
    if (!vMem)
    {
        puts("Failed to Allocate Dynamic Memory");
        free(vMem);
        exit(EXIT_FAILURE);
    }

    // initialize vMem to 0
    memset(vMem, 0, MEMSIZE * sizeof(memory_t));

    // read the elf header, if elf header could not be read exit w/
    // appropriate message
    if (!read_header(input, &elf_head))
    {
        puts("Failed to Read ELF Header");
        free(vMem);
        exit(EXIT_FAILURE);
    }

    // if header is true, dump header
    if (header)
    {
        dump_header(elf_head);
    }

    // declare array of phdrs
    elf_phdr_t  elf_phead[elf_head.e_num_phdr];
    for (int i = 0; i < elf_head.e_num_phdr; i++)
    {
        // read the phdrs into array of phdrs, if phdrs could not be read exit
        // w/ appropriate message
        if (!read_phdr(input, (i * 20) + elf_head.e_phdr_start, &elf_phead[i]))
        {
            puts("Failed to Read Program Header");
            free(vMem);
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < elf_head.e_num_phdr; j++)
    {
        // load segments from elf file to virtual memory, if segments could not be
        // loaded into memory exit w/ appropriate message
        if (!load_segment(input, vMem, elf_phead[j]))
        {
            puts("Failed to Load Segment");
            free(vMem);
            exit(EXIT_FAILURE);
        }
    }

    // if segments flag true, dump phdrs
    if (segments)
    {
        dump_phdrs(elf_head.e_num_phdr, elf_phead);
    }

    // if memfull flag true, dump full memory contents
    if (memfull)
    {
        dump_memory(vMem, 0, MEMSIZE);
    }

    // if membrief flag true, dump only segments that contain values
    if (membrief)
    {
        for (int n = 0; n < elf_head.e_num_phdr; n++)
        {
            if (elf_phead[n].p_filesz > 0)
            {
                int start_addr = elf_phead[n].p_vaddr;
                int end_addr   = start_addr + elf_phead[n].p_filesz;

                start_addr &= ~(0xF);

                dump_memory(vMem, start_addr, end_addr);
            }
        }
    }

    // if disas_code flag true, print the disassembly of code contents
    if (disas_code)
    {
       puts("Disassembly of executable contents:");
       for (int p = 0; p < elf_head.e_num_phdr; p++)
        {
            if (elf_phead[p].p_type == CODE)
            {
                disassemble_code(vMem, &elf_phead[p], &elf_head);
                puts("");
            }
        }
    }

    // if disas_data flag true, printt he disassembly of data contents
    if (disas_data)
    {
        puts("Disassembly of data contents:");
        for (int q = 0; q < elf_head.e_num_phdr; q++)
        {
            if (elf_phead[q].p_type == DATA)
            {
                if (elf_phead[q].p_flag == RW)
                {
                    disassemble_data(vMem, &elf_phead[q]);
                    puts("");
                }
                else if (elf_phead[q].p_flag == RO)
                {
                    disassemble_rodata(vMem, &elf_phead[q]);
                    puts("");
                }
            }
        }
    }
    
    // // ==== Begin Project 4 Solution  =====
  
    if ( exec_normal || exec_debug ) 
    {
        y86_t          cpu;
        y86_inst_t     ins;
        y86_register_t valA;
        y86_register_t valE;
        bool           cond;
        int            insCount = 0;
        // other variables as needed

        // initialize CPU state
        cpu.pc = elf_head.e_entry;
        cpu.stat = AOK;
        cpu.sf = cpu.of = cpu.zf = false;
        cpu.rax = cpu.rcx = cpu.rdx = cpu.rbx = cpu.rsp
            = cpu.rbp = cpu.rsi = cpu.rdi = cpu.r8 = cpu.r9
            = cpu.r10 = cpu.r11 = cpu.r12 = cpu.r13 = cpu.r14 = 0;

        printf("Entry execution point at 0x%04lx\n", cpu.pc );
        printf( "Initial " ); 
        dump_cpu( &cpu );

        // Start the von Neumann cycle       
        while ( cpu.stat == AOK ) 
        {
            // Fetch next instruction
            ins = fetch( &cpu, vMem) ;
            // if instruction type flag is invalid, print correspinding error message and break the loop
            if( ins.type == INVALID ) 
            {
                printf("Corrupt Instruction (opcode 0x%02x) at address 0x%04lx\n", ins.opcode, cpu.pc  );
                printf("Post-Fetch ");
                dump_cpu( &cpu );
                insCount++;
                break ;
            }
            // if exec_debug flag true, print the dissasembly of the instruction
            if ( exec_debug )
            { 
                printf("Executing: ");   
                disassemble( ins );
                printf("\n");
            }

            // Decode-Execute
            valE = decode_execute( &cpu, &cond, &ins, &valA );

            // memory writeback & pc update
            memory_wb_pc( &cpu, vMem, cond, &ins, valE, valA );
            
            // if cpu status is AOK and exec_debug flag true, print the post execution cpu dump
            if ( cpu.stat != AOK || exec_debug )
            {
                printf( "Post-Exec " ); 
                dump_cpu( &cpu );
            }

            insCount++;
        }
      
        // print total number of instructions
        printf("Total execution count: %d instructions\n\n", insCount );

        // if exec_debug flag true, dump entire contents of memory
        if ( exec_debug )
        {
            dump_memory(vMem, 0, MEMSIZE);
        }
    }
    
    // Release the dynamic memory back to the Heap and Close any open files
    fclose(input);
    free(vMem);
    return EXIT_SUCCESS;    

}
