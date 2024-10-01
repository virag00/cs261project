/*
 * CS 261 PA2: Mini-ELF loader
 *
 * Name: Charlie Virag
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "p2-load.h"

void usage_p2 ()
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
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

bool parse_command_line_p2 (int argc, char **argv,
        bool *header, bool *segments, bool *membrief, bool *memfull,
        char **file)
{
    // Initialize variables
    int opt;
    opterr = 0;
    bool H_flag = false;
    bool s_flag = false;
    bool m_flag = false;
    bool M_flag = false;
    char *option_string = "+hHafsmM";

    // Parse command line arguments
    while((opt = getopt(argc, argv, option_string)) != -1)
    {
        switch(opt)
        {
            case 'h':   // Display usage information and reset all variables and flags
                        usage_p2();
                        *header = false;
                        *segments = false;
                        *membrief = false;
                        *memfull = false;
                        *file = NULL;
                        return true;

            case 'H':   // Display the ELF header, only once
                        if (H_flag)
                        {
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *file = NULL;
                            return false;
                        }
                        *header = true;
                        H_flag = true;
                        break;

            case 'a':   // Display all information: ELF header, program segments, memory map (brief)
                        if (H_flag || s_flag || m_flag || M_flag)
                        {
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
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
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
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
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *file = NULL;
                            return false;
                        }
                        *segments = true;
                        s_flag = true;
                        break;

            case 'm':   // Display the virtual memory (brief), only once
                        if (m_flag || M_flag)
                        {
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *file = NULL;
                            return false;
                        }
                        *membrief = true;
                        m_flag = true;
                        break;

            case 'M':   // Display the virtual memory (full), only once
                        if (M_flag || m_flag)
                        {
                            usage_p2();
                            *header = false;
                            *segments = false;
                            *membrief = false;
                            *memfull = false;
                            *file = NULL;
                            return false;
                        }
                        *memfull = true;
                        M_flag = true;
                        break;

            default:    // Incorrect arguments, display usage and reset all flags and variables
                        usage_p2();
                        *header = false;
                        *segments = false;
                        *membrief = false;
                        *memfull = false;
                        *file = NULL;
                        return false;

        }
    }

    // Ensure exactly one file name at the end of the command line
    if (optind != argc - 1)
    {
        usage_p2();
        return false;
    }

    *file = argv[optind];
    return true;
}

bool read_phdr (FILE *file, uint16_t offset, elf_phdr_t *phdr)
{
    if (phdr == NULL || !file)
    {
        return false;
    }
    // Seek to the specified offset in the file
    if (fseek(file, offset, SEEK_SET) != 0) {
        return false; // Failed to seek to offset
    }

    // Read the program header into the buffer pointed to by phdr
    if (fread(phdr, sizeof(elf_phdr_t), 1, file) != 1) {
        return false; // Failed to read program header
    }

    // Check for valid magic number
    if (phdr->magic != 0xDEADBEEF)
    {
        return false;
    }

    return true;
}

void dump_phdrs (uint16_t numphdrs, elf_phdr_t phdr[])
{
    char *type = NULL;
    char *flag = NULL;
    
    printf("Segment   Offset    VirtAddr  FileSize  Type      Flag\n");
    for (int i = 0; i < numphdrs; i++)
    {
        switch (phdr[i].p_type)
        {
            case 0:
                type = "DATA      ";
                break;

            case 1:
                type = "CODE      ";
                break;

            case 2:
                type = "STACK     ";
                break;

            case 3:
                type = "HEAP      ";
                break;

            case 4:
                type = "UNKOWN    ";
                break;
        }

        switch (phdr[i].p_flag)
        {
            case 4:
                flag = "R";
                break;

            case 5:
                flag = "R X";
                break;
            
            case 6:
                flag = "RW";
                break;
        }

        printf("  %02d      0x%04x    0x%04x    0x%04x    %s%s\n",
            i, phdr[i].p_offset, phdr[i].p_vaddr,
            phdr[i].p_filesz, type, flag);
    }
    printf("\n");
}

bool load_segment (FILE *file, memory_t memory, elf_phdr_t phdr)
{
    if (!memory || !file)
    {
        return false;
    }
    // Check for valid magic number
    if (phdr.magic != 0xDEADBEEF)
    {
        return false;
    }

    // Check for valid vaddr
    if (phdr.p_filesz + phdr.p_vaddr > MEMSIZE)
    {
        return false;
    }

    // Do not read anything from zero-size segments
    if (phdr.p_filesz == 0)
    {
        return true;
    }

    // Seek to the offset of the segment in the file
    if (fseek(file, phdr.p_offset, SEEK_SET) != 0)
    {
        return false;
    }

    // Read the segment from the file into memory
    if (fread(memory + phdr.p_vaddr, phdr.p_filesz, 1, file) != 1)
    {
        return false;
    }

    return true;
}

void dump_memory (memory_t memory, uint16_t start, uint16_t end)
{
    int i = start;
    int line_count = 0;
    int hex_max = 16;
    printf("Contents of memory from %04x to %04x:\n", start, end);

    while (i < end)
    {
        printf("  %04x  ", (i));
        if (end < hex_max)
        {
            hex_max = end;
        }

        for (int j = 0; j < hex_max; j++)
        {
            printf("%02x", memory[i]);
            if ((j != 7) && (j != 15)) printf(" ");
            if (j == 7) printf("  ");
            i++;
            if (i == end) break;
        }
        printf("\n");
        line_count++;
        if (start == 0) {
            start += hex_max;
        }
    }

    printf("\n");
}
