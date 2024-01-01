#include "types.h" // Contains user defined types

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* output txt info */
    char dest_fname[20];
    FILE *fptr_dest_file;

    
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* extn size info */
    int extn_size;
    int data_size;

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);

/*skip the header*/
Status decode_skip_header(FILE *fptr);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode stego file extenstion */
Status decode_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
char decode_byte_to_lsb(char *image_buffer);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*Decode size_to_lsb of file*/
uint decode_size_to_lsb(char *image_buffer);

