#include <stdint.h>
#define SFS_MAX_OPEN_FILES 100

struct sfs_state
{
  //unix file descriptor
  int unix_fd;

  //boot directory fields
  uint16_t bps;
  uint8_t spc;
  uint8_t fat_count;
  uint16_t root_count;
  uint16_t spf;

  //derived offsets
  int fat1_start;
  int fat2_start;
  int root_start;
  int data_start;

  //file descriptors
  struct file_descriptor
  {
    int available;
    int size;
    int bytes_read;
    int current_cluster;//physical
    int current_offset;
  }file_descs[100];
};

typedef struct sfs_state SFS;

/****************************************/
/* do not change the portion below this */
/****************************************/

/* 
 * sfs_initfs: this function is called before any other action on the
 * file system.
 *
 * Input parameters:
 *    disk_image: Name of a file on the Linux machine that contains a FAT12 disk image.
 *      
 * Return value: 
 *    if the disk image file can be successfully opened and is the
 *    correct format, then the return value must be a handle to the
 *    SFS state that will be passed back during each call to a
 *    subsequent SFS function
 *
 *    upon error, NULL should be returned
 */
SFS * sfs_initfs(const char * disk_image);

/*
 * This function is called once, after the file system has been used.
 * After this function is called, sfs_initfs has to be called again
 * before further use.
 */
void sfs_concludefs(SFS * sfs_handle);

/*
 * sfs_open: open a file for reading, and set the read pointer to the beginning 
 *           of the file.
 *
 * Input parameters:
 *    sfs_handle: Handle to SFS state. This is the pointer returned by the 
 *                function sfs_initfs.
 *
 *    pathname: absolute pathname for the file in question. The
 *              directories are delimited by "/". For instance, "/home/snt/filename.txt"
 *
 * Return value:
 *    if the open succeeded, then a SFS file descriptor (an integer) must be 
 *       returned. This should be a non-negative integer, and the system must return the
 *       smallest non-negative integer that is not in use by another open file within SFS.
 *
 *    on error (for instance, if the file does not exist), -1 must be returned.
 */
int sfs_open(SFS *sfs_handle, const char * pathname);


/*
 * sfs_read: read bytes from an SFS file that is already open
 * 
 * Input parameters:
 *    sfs_handle: Handle to SFS state. The same as in sfs_open
 *
 *    sfs_fd: SFS file descriptor that was earlier returned by an "sfs_open".
 *    
 *    buffer: memory area to which the data bytes will be read into
 *  
 *    num_bytes: the number of bytes to read from the file
 *
 * Return value:
 *    if non-zero bytes are read into the buffer, the number of bytes actually
 *       read must be returned.
 *
 *    upon error (for instance, invalid file descriptor, or invalid sfs_handle), 
 *       -1 must be returned.
 */
int sfs_read(SFS *sfs_handle, int sfs_fd, void * buffer, int num_bytes);


/*
 * sfs_close: close an SFS file that is already open
 *
 * Input parameters:
 *    sfs_handle: handle to SFS state. The description is the same as in sfs_open.
 *   
 *    sfs_fd: SFS file descriptor that was earlier returned by an "sfs_open"
 *
 * Return value:
 *    1  upon successful close
 *    -1 upon error (for instance, invalid file descriptor of invalid sfs handle)
 */
int sfs_close(SFS *sfs_handle, int sfs_fd);

/*********************************************************/
