#include <stdint.h> //defines uint8_t and uint16_t, unsigned 8-bit and 16-bit integers
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

typedef struct FAT_boot_sector_info {
     char name[8]; //FAT name
     uint16_t bps; //bytes per sector
     uint8_t spc; //sectors per cluster
     uint16_t r_sectors; //reserved sectors
     uint8_t fat_count; // number of FATs
     uint16_t entry_count; // number of root directory entries
     uint16_t logical_count; //number of logical sectors
     uint8_t descriptor; //medium descriptor
     uint16_t spf; //sectors per FAT
     uint16_t head_count; //number of heads
     uint16_t hidden_count; // number of hidden sectors
} bs_info;

unsigned int little_endian_to_uint(uint8_t *bytes, size_t nmemb);
void print_directory(uint8_t *bytes);

int main(int argc, char **argv) {
     if(argc < 2) {
	  printf("usage: %s floppy_image\n", argv[0]);
	  return 1;
     }
     char *floppy = argv[1]; //floppy image path

     //insert code here
    int fd = open(floppy, O_RDONLY);    
    bs_info boot;
    unsigned char buffer[10];
    
    //name @ 0x03 (8 bytes)    
    lseek(fd, 0x03, SEEK_SET);
    read(fd, buffer, 8);
    memcpy(boot.name, buffer, 8);
    int i;
    printf("VOLUME NAME: ");
    for(i=0; i<8; i++)
    {
        printf("%c",boot.name[i]);
    }
    printf("\n");

    //bytes/sector      @ 0x0b (2 bytes)
    read(fd, buffer, 2);
    boot.bps = little_endian_to_uint(buffer, 2);
    //printf("bytes/sector %d\n", boot.bps);

    //sectors/cluster @ 0x0d (1 byte)
    read(fd, buffer, 1);
    boot.spc = buffer[0];
    //printf("sectors/cluster %d\n", boot.spc);

    //reserved sectors @ 0x0e (2 bytes)
    read(fd, buffer, 2);
    boot.r_sectors = little_endian_to_uint(buffer, 2);
    //printf("reserved sectors %d\n", boot.r_sectors);

    //number of FATS @ 0x10 (1 byte)
    read(fd, buffer, 1);
    boot.fat_count = buffer[0];
    //printf("FATs %d\n", boot.fat_count);

    //number of root entries @ 0x11 (2 bytes)
    read(fd, buffer, 2);
    boot.entry_count = little_endian_to_uint(buffer, 2);
    //printf("root entries %d\n", boot.entry_count);

    //number of logical sectors @ 0x13 (2 bytes)
    read(fd, buffer, 2);
    boot.logical_count = little_endian_to_uint(buffer, 2);
    //printf("logical sectors %d\n",boot.logical_count);

    //medium descriptor @ 0x15 (1 byte)
    read(fd, buffer, 1);
    boot.descriptor = buffer[0];
    //printf("medium descriptor %d\n", boot.descriptor);

    //sectors per FAT @ 0x16 (2 bytes)
    read(fd, buffer, 2);
    boot.spf = little_endian_to_uint(buffer, 2);
    //printf("sectors per FAT %d\n", boot.spf);

    //number of heads @ 0x1a (2bytes)
    lseek(fd, 2, SEEK_CUR);
    read(fd, buffer, 2);
    boot.head_count = little_endian_to_uint(buffer, 2);
    //printf("number of heads %d\n", boot.head_count);

    //number of hidden sectors @ 0x1c
    read(fd, buffer, 2);
    boot.hidden_count = little_endian_to_uint(buffer, 2);
    //printf("number of hidden sectors %d\n", boot.hidden_count);
    
    int fat1_base = boot.bps;
    int fat2_base = fat1_base + boot.spf*boot.bps;
    int root_base = fat2_base + boot.spf*boot.bps;

    //printf("root base %d\n", root_base);

    lseek(fd, 9728, SEEK_SET); //jump to start of root directory

    printf("FILENAME.EXT ATTR      SIZE DATE/TIME\n");
    for(i=0; i<boot.entry_count; i++)
    {
        read(fd, buffer, 8);
        if(buffer[0] == 0x00)
        {
            lseek(fd, 24, SEEK_CUR);
        }
        else if(buffer[0] == 0xE5)
        {
            lseek(fd, 24, SEEK_CUR);
        }
        else
        {
            int j;
            char s[10];

            //file name (8 bytes)
            sprintf(s,"");
            for(j=0; j<8; j++)
            {
                sprintf(s,"%s%c", s, buffer[j]);
            }
            printf("%8s.", s);

            //file extension (3 bytes)
            read(fd, buffer, 3);
            sprintf(s,"");
            for(j=0; j<3; j++)
            {
                sprintf(s,"%s%c", s, buffer[j]);
            }
            printf("%3s", s);

            //attributes (1 byte)
            read(fd, buffer, 1);
            unsigned char attr = buffer[0];
            //XXAXXSHR
            //00100000 = A = 0x20
            //00000100 = S = 0x04
            //00000010 = H = 0x02
            //00000001 = R = 0x01
            char read_only = (attr & 0x01) ? 'R' : ' ';
            char hidden = (attr & 0x02) ? 'H' : ' ';
            char system = (attr & 0x04) ? 'S' : ' ';
            char archive = (attr & 0x20) ? 'A' : ' ';
            printf(" %c%c%c%c", read_only, hidden, system, archive);

            //skip over reserved section
            lseek(fd, 10, SEEK_CUR);

            //time last modified (2 bytes) [0]SSSSSMMM [1]MMMHHHHH
            read(fd, buffer, 2);
            unsigned int sec = (buffer[0] >> 3) << 1; //1st 5 bits * 2
            unsigned int min = ((buffer[0] & 0x07) << 3) | buffer[1] >> 5; //middle 6
            unsigned int hour = buffer[1] & 0xE0; // last 5

            //date last modified (2 bytes) [0]YYYYYYYM [1]MMMDDDDD
            read(fd, buffer, 2);
            unsigned int year = (buffer[0] >> 1) + 1980; //1st 7 bits + 1980
            unsigned int month = ((buffer[0] & 0x01) << 3) | (buffer[1] >> 5); //next 4
            unsigned int day = buffer[1] & 0xE0; // last 5 bits

            //first cluster (skip - 2 bytes)
            lseek(fd, 2, SEEK_CUR);            

            //size (4 bytes)
            read(fd, buffer, 4);
            unsigned int size = little_endian_to_uint(buffer, 4);
            printf("%10d", size);
            printf(" %2d:%2d:%2d", hour, min, sec);
            printf(" %2d/%2d/%4d\n", month, day, year);
        }
    }
    

     return 0;
}

/*
 * Reverse an array of bytes. Useful for switching endianness.
 * @param bytes an array of bytes.
 * @param nmemb the size of byte array. This should not be more than four
 *              (the size of an unsigned int).
 */
unsigned int little_endian_to_uint(uint8_t *bytes, size_t nmemb) {
     //insert code here
    unsigned int i, ret=0;
    for(i=0; i<nmemb; i++)
    {
        ret |= *(bytes+i) << i * 8;
    }
    return ret;
}

/*
 * Print a 32-bit FAT directory entry.
 * @param bytes an array of bytes that make up the directory entry. This
 *              function assumes the array is exactly 32 bytes long.
 */
void print_directory(uint8_t *bytes) {
     //insert code here
}
