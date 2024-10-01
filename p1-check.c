/*
 * CS 261 PA1:  Mini-ELF header verifier
 *
 * Name:        Charlie Virag
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include "p1-check.h"

#define MAGIC 0x454c4600

void usage_p1 ()
{
    printf("Usage: y86 <option(s)> mini-elf-file\n");
    printf(" Options are:\n");
    printf("  -h      Display usage\n");
    printf("  -H      Show the Mini-ELF header\n");
    printf("Options must not be repeated neither explicitly nor implicitly.\n");
}

bool parse_command_line_p1 (int argc, char **argv, bool *header, char **file)
{
    // Implement this function
    int opt;
    opterr = 0;
    bool H_flag = false;
    char *option_string = "+hH";

    
    while((opt = getopt(argc, argv, option_string)) != -1)
    {
        switch(opt)
        {
            case 'h':   usage_p1();
                        *header = false;
                        *file = NULL;
                        return true;

            case 'H':   if (H_flag)
                        {
                            usage_p1();
                            *header = false;
                            *file = NULL;
                            return false;
                        }
                        *header = true;
                        H_flag = true;
                        break;

            default:    usage_p1();
                        *header = false;
                        *file = NULL;
                        return false;

        }
    }

    if (optind != argc - 1)
    {
        usage_p1();
        return false;
    }

    *file = argv[optind];
    return true;
}

bool read_header (FILE *file, elf_hdr_t *hdr)
{
    // Implement this function
    if (file == NULL || hdr == NULL)
    {
        //puts("failed file==NULL || hdr == NULL");
        return false;
    }

    if (fread(hdr, sizeof(elf_hdr_t), 1, file) != 1)
    {
        //puts("failed fread");
        return false;
    }

    if (hdr->magic == ntohl(MAGIC))
    {
        return true;
    }

    //puts("failed at end");
    return false;
}

void dump_header (elf_hdr_t hdr)
{
    unsigned char *bytes = (unsigned char*) &hdr;
    unsigned int i;  

    // Implement this function
    printf("%08x  ", 0);

    for (i = 0; i < sizeof(elf_hdr_t) && i < 16; i++)
    {
        printf("%02x ", bytes[i]);
        if (i == 7) printf(" ");
    }
    printf("\nMini-ELF version %d\n", hdr.e_version);
    printf("Entry point 0x%0x\n", hdr.e_entry);
    printf("There are %d program headers, starting at offset %d (0x%0x)\n", hdr.e_num_phdr, hdr.e_phdr_start, hdr.e_phdr_start);
    if (hdr.e_symtab == 0)
    {
        printf("There is no symbol table present\n");
    } else 
    {
        printf("There is a symbol table starting at offset %d (0x%0x)\n", hdr.e_symtab, hdr.e_symtab);
    }
    if (hdr.e_strtab == 0)
    {
        printf("There is no string table present\n");
    } else
    {
        printf("There is a string table starting at offset %d (0x%0x)\n", hdr.e_strtab, hdr.e_strtab);
    }
    printf("\n");
}

