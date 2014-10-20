#include "sfs.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

unsigned int little_endian_to_uint(uint8_t *bytes, size_t nmemb);

int move_to_next_cluster(SFS * sfs_handle, int sfs_fd);

SFS * sfs_initfs(const char * disk_image)
{
  SFS * ret = (SFS*) malloc(sizeof(SFS));
  int fd = open(disk_image, O_RDONLY);
  if(fd == -1)
  {
    perror("Error opening disk image");
    return NULL;
  }
  ret -> unix_fd = fd;
  unsigned char buffer[10];

  //bytes per sector -- 2 bytes @0x0b
  lseek(fd, 0x0b, SEEK_SET);
  read(fd, buffer, 2);
  ret -> bps = little_endian_to_uint(buffer,2);

  //sectors per cluster -- 1 byte @0x0d
  read(fd, buffer, 1);
  ret -> spc = buffer[0];

  //skip reserved (2 bytes)
  lseek(fd, 2, SEEK_CUR);

  //number fats -- 1 byte @0x10
  read(fd, buffer, 1);
  ret -> fat_count = buffer[0];

  //root directory size -- 2 bytes @0x11
  read(fd, buffer, 2);
  ret -> root_count = little_endian_to_uint(buffer,2);

  //skip logical sectors, medium descriptor (3 bytes)
  lseek(fd, 3, SEEK_CUR);

  //sectors per fat -- 2 bytes @0x16
  read(fd, buffer, 2);
  ret -> spf = little_endian_to_uint(buffer,2);

  //fill in derived values
  ret -> fat1_start = ret -> bps;
  ret -> fat2_start = ret -> fat1_start + ret -> spf * ret -> bps;
  ret -> root_start = ret -> fat2_start + ret -> spf * ret -> bps;
  ret -> data_start = ret -> root_start + 32 * ret -> root_count;
  ret -> data_start += ret -> data_start % ret -> bps; //need to align to sector start

  //initialize file descriptors
  int i;
  for(i=0; i<100; i++)
  {
    ret -> file_descs[i].available = 1;
    ret -> file_descs[i].size = 0;
    ret -> file_descs[i].bytes_read = 0;
    ret -> file_descs[i].current_cluster = 0;
    ret -> file_descs[i].current_offset = 0;
  }

  return ret;        
}

void sfs_concludefs(SFS * sfs_handle)
{
  close(sfs_handle -> unix_fd);
  free(sfs_handle);     
}

int sfs_open(SFS *sfs_handle, const char * pathname)
{
  //find available file descriptor
  int i, fd = -1;
  for(i=0; i<100; i++)
  {
    if(sfs_handle -> file_descs[i].available)
    {
      fd = i;
      break;
    }
  }
  if(fd == -1) return fd; //no file descriptors available

  //assuming we have a descriptor, need to navigate the path
  //start at root
  sfs_handle -> file_descs[fd].current_cluster = -1;
  sfs_handle -> file_descs[fd].size = sfs_handle -> root_count * 32;
  sfs_handle -> file_descs[fd].bytes_read = 0;
  sfs_handle -> file_descs[fd].current_offset = 0;
  char * pch;
  char tmp[100];
  sprintf(tmp,"%s",pathname);
  pch = strtok(tmp, "/");
  while(pch != NULL)
  {
    //find the current token in the current cluster
    
    //calculate address of start of current cluster
    int cluster_addr;
    if(sfs_handle -> file_descs[fd].current_cluster == -1)
    {
      cluster_addr = sfs_handle -> root_start;
    }
    else
    {
      cluster_addr = sfs_handle -> data_start +
        sfs_handle -> file_descs[fd].current_cluster *
        sfs_handle -> spc * sfs_handle -> bps;
    }
   
    lseek(sfs_handle -> unix_fd, cluster_addr, SEEK_SET);
    char buffer[10];
    //search the current directory
    int found = 0;
    int bpc = sfs_handle -> spc * sfs_handle -> bps;
    int has_next = 1;
    while(has_next)
    {
      read(sfs_handle -> unix_fd, buffer, 8);
      buffer[8] = '\0';
      char name[10];
      strcpy(name,buffer);
      for(i=0; i<8; i++) //strings won't compare correctly unless whitespace is gone
      {
        if(name[i] == ' ')
        {
          name[i] = '\0';
        }
      }      

      char name_ext[15];
      strcpy(name_ext, name);
      read(sfs_handle -> unix_fd, buffer, 3);
      sprintf(name_ext, "%s.%c%c%c", name_ext, buffer[0], buffer[1], buffer[2]);

      lseek(sfs_handle -> unix_fd, 15, SEEK_CUR);
      read(sfs_handle -> unix_fd, buffer, 2);
      int first_cluster = little_endian_to_uint(buffer,2) -2;
      read(sfs_handle -> unix_fd, buffer, 4);
      int size = little_endian_to_uint(buffer,4);

      sfs_handle -> file_descs[fd].bytes_read += 32;
      sfs_handle -> file_descs[fd].current_offset += 32;

      if(strcmp(pch,name) == 0 || strcmp(pch,name_ext) == 0)
      {
        found = 1;
        sfs_handle -> file_descs[fd].current_cluster = first_cluster;
        sfs_handle -> file_descs[fd].size = size;
        sfs_handle -> file_descs[fd].bytes_read = 0;
        sfs_handle -> file_descs[fd].current_offset = 0;
        sfs_handle -> file_descs[fd].available = 0;
        break;
      }
      //if a subdirectory, and hit end of cluster, see if theres a next cluster
      if(sfs_handle -> file_descs[fd].current_offset >= bpc
        && sfs_handle -> file_descs[fd].current_cluster != -1)
      {
        has_next = move_to_next_cluster(sfs_handle, fd);
      }
      //else if root, and hit end of the directory.. you're done
      else if(sfs_handle -> file_descs[fd].current_offset >= 32 * sfs_handle -> root_count
        && sfs_handle -> file_descs[fd].current_cluster == -1)
      {
        has_next = 0;
      }
    }
    if(!found)
    {
      printf("FILE NOT FOUND: %s\n", pch);
      return -1;
    }
    pch = strtok(NULL, "/");
  }
  //now the fd state should set up at byte 0 of the file's first cluster, ready to read
  return fd;
}

int sfs_read(SFS *sfs_handle, int sfs_fd, void * buffer, int num_bytes)
{
  //error check
  if(sfs_handle == NULL) return -1;
  if(sfs_fd < 0 || sfs_fd >= 100) return -1;
  if(sfs_handle -> file_descs[sfs_fd].available == 1 ||
    sfs_handle -> file_descs[sfs_fd].current_cluster < 0)
  {
    return -1;
  }
  int bpc = sfs_handle -> spc * sfs_handle -> bps;
  int image_offset = sfs_handle -> file_descs[sfs_fd].current_cluster * bpc +
      sfs_handle -> file_descs[sfs_fd].current_offset + sfs_handle -> data_start;
  lseek(sfs_handle -> unix_fd, image_offset, SEEK_SET);
  int i;
  for(i=0; i<num_bytes; i++)
  {
    //read bytes > size
    if(sfs_handle -> file_descs[sfs_fd].bytes_read > sfs_handle -> file_descs[sfs_fd].size)
    {
      break;
    }
    //offset in cluster > bytes/cluster
    else if(sfs_handle -> file_descs[sfs_fd].current_offset >= bpc)
    {
      if(!move_to_next_cluster(sfs_handle, sfs_fd))
      {
        return i;
      }
    }
    char * cbuffer = (char*) buffer;
    read(sfs_handle -> unix_fd, cbuffer + i, 1);
    sfs_handle -> file_descs[sfs_fd].bytes_read++;
    sfs_handle -> file_descs[sfs_fd].current_offset++;
  }
  return i;
}

int sfs_close(SFS *sfs_handle, int sfs_fd)
{
  //error check
  if(sfs_handle == NULL) return -1;
  if(sfs_fd < 0 || sfs_fd >= 100) return -1;

  sfs_handle -> file_descs[sfs_fd].available = 1;
  sfs_handle -> file_descs[sfs_fd].size = 0;
  sfs_handle -> file_descs[sfs_fd].bytes_read = 0;
  sfs_handle -> file_descs[sfs_fd].current_cluster = -1;
  sfs_handle -> file_descs[sfs_fd].current_offset = 0;
  return 1;
}

unsigned int little_endian_to_uint(uint8_t *bytes, size_t nmemb) {
    unsigned int i, ret=0;
    for(i=0; i<nmemb; i++)
    {
        ret |= *(bytes+i) << i * 8;
    }
    return ret;
}

/*
attempt to move the file to its next cluster
by looking it up in the FAT
if successful, the file pointer is moved to the
start of the next cluster, and file descriptor fields
are updated accordingly

return 1 if successful
return 0 if current cluster is the last
*/
int move_to_next_cluster(SFS * sfs_handle, int sfs_fd)
{
  int entry = sfs_handle -> file_descs[sfs_fd].current_cluster + 2;
  int offset = (entry/2)*3 + entry%2;
  int address = sfs_handle -> fat1_start + offset;
  unsigned char buffer[3];
  lseek(sfs_handle -> unix_fd, address, SEEK_SET);
  read(sfs_handle -> unix_fd, buffer, 2);
  unsigned int new_cluster = (((unsigned int)buffer[1]) << 8) | buffer[0];
  if(entry%2 == 0)
  {
    new_cluster &= 0x0FFF;
  }
  else
  {
    new_cluster = new_cluster >> 4;
  }
  
  if(new_cluster >= 0x0FF8)
  {
    return 0;
  }
  else
  {
    //calculate new start address, set state, pointer to that spot, return true
    sfs_handle -> file_descs[sfs_fd].current_cluster = new_cluster - 2;
    sfs_handle -> file_descs[sfs_fd].current_offset = 0;
    int bpc = sfs_handle -> spc * sfs_handle -> bps;
    offset = sfs_handle -> data_start + bpc * (new_cluster - 2);
    lseek(sfs_handle -> unix_fd, offset, SEEK_SET);
    return 1;
  }
}
