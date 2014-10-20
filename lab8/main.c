#include "sfs.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
  int sfs_fd;
  int unix_fd;
  int sfs_bytes_read;
  char buffer[1000];

  //Initialize the SFS structure.
  SFS * handle = sfs_initfs("f12_lab8.img");
  printf("SFS initialized\n");

//--------------------------------------------------------------------//

  printf("ATTEMPT: BALROG.LOG\n");   
  
  //Open the file BALROG.LOG and dump contents to the terminal.
  sfs_fd = sfs_open(handle, "DIG/DEEP/DEEPER/DEEPEST/BALROG.LOG");
  sfs_bytes_read = sfs_read(handle, sfs_fd, buffer, 1000);
  buffer[sfs_bytes_read] = '\0';
  printf("%s",buffer);

  //Close BALROG.LOG
  sfs_close(handle,sfs_fd);

  printf("CLOSED BALROG.LOG\n");

//--------------------------------------------------------------------//

  printf("ATTEMPT: FILE.TXT\n");

  //Open the file FILE.TXT and dump contents to the terminal.
  sfs_fd = sfs_open(handle, "FILE.TXT");
  sfs_bytes_read = sfs_read(handle, sfs_fd, buffer, 1000);
  buffer[sfs_bytes_read] = '\0';
  printf("%s",buffer);

  //Close FILE.TXT
  sfs_close(handle,sfs_fd);

  printf("CLOSED FILE.TXT\n");

//--------------------------------------------------------------------//

  printf("ATTEMPT: BUNNIES/255/FILE.TXT\n");

  //Open the file BUNNIES/255/FILE.TXT and dump contents to the terminal.
  sfs_fd = sfs_open(handle, "BUNNIES/255/FILE.TXT");
  sfs_bytes_read = sfs_read(handle, sfs_fd, buffer, 1000);
  buffer[sfs_bytes_read] = '\0';
  printf("%s",buffer);

  //Close BUNNIES/FILE.TXT
  sfs_close(handle,sfs_fd);

  printf("CLOSED BUNNIES/FILE.TXT\n");

//--------------------------------------------------------------------//

  printf("ATTEMPT: DIG/DEEP/HERP/DERP.TXT\n");

  //Open the file DIG/DEEP/HERP/DERP.TXT and dump contents to the terminal.
  sfs_fd = sfs_open(handle, "DIG/DEEP/HERP/DERP.TXT");
  sfs_bytes_read = sfs_read(handle, sfs_fd, buffer, 1000);
  //buffer[sfs_bytes_read] = '\0';
  //printf("%s",buffer);

  //Close DIG/DEEP/HERP/DERP.TXT
  sfs_close(handle,sfs_fd);

  printf("CLOSED DIG/DEEP/HERP/DERP.TXT\n");

//--------------------------------------------------------------------//

  printf("ATTEMPT: CATS/HUNGRY.JPG\n");

  //Open the file HUNGRY.JPG and dump contents to file.
  sfs_fd = sfs_open(handle,"CATS/HUNGRY.JPG");
  //Note, this file should open as an image if you have implemented
  //Everything correctly.
  unix_fd = open("HUNGRY.JPG",O_WRONLY|O_CREAT,0644);//,0644);
  int read = 1;
  while(read)
  {
    read = sfs_read(handle,sfs_fd,buffer,1000);
    if(read) write(unix_fd,buffer,read);
  }
  //while((sfs_bytes_read = sfs_read(handle,sfs_fd,buffer,1000)))
  //  write(unix_fd,buffer,sfs_bytes_read);//Write the bytes we read to file.
  //Close HUNGRY.JPG
  close(unix_fd);
  sfs_close(handle,sfs_fd);

  printf("CLOSED CATS/HUNGRY.JPG\n");

//--------------------------------------------------------------------//

  sfs_concludefs(handle);
  return 0;
}
