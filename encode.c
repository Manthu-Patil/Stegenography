#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "string.h"
#include "common.h"
/* Function Definitions */

/*get image size
input: image file pointers
output:width * height * bytes per pixel
Description: in bmp, width is stored in offset 18 and height after that
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //   printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //  printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*Check the encoding or decoding
input: arugement vector passed through CLA
output: return encode or decode
Description: To start the process user enters his choice ie. encoding or decoding
 */
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

/*Check the extensions and the arguments passed through CLA
input: argument vector and a structure variable
output: success or failure
Description: check each and every argument extension
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ptr=strstr(argv[2],".bmp");
    {
        if(ptr!=NULL)
        {
            if (strcmp(strstr(argv[2],"."),".bmp")==0)
            {
                printf("argv[2] is a .bmp extension\n");
                encInfo -> src_image_fname = argv[2];
            }
        }
        else
        {
            printf("Not a .bmp file please provide .bmp extension\n");
            return e_failure;
        }

        ptr=(strstr(argv[3],".txt") || strstr(argv[3],".sh") || strstr(argv[3],".c"));
        if(ptr!=NULL)
        {
            if ((strcmp(strstr(argv[3],"."),".txt")==0)  || (strcmp(strstr(argv[3],"."),".c")==0) || (strcmp(strstr(argv[3],"."),".sh")==0))  
            {
                printf("argv[3] is a %s extension\n",strstr(argv[3],"."));
                encInfo -> secret_fname = argv[3];
                //  strcpy(encInfo->extn_secret_file,strstr(argv[3],".")); 
            }
        }
        else
        {
            printf("%s file please provide .txt/.sh/.c files\n",strstr(argv[3],"."));
            return e_failure;
        }
        if(argv[4]==NULL)
        {
            encInfo -> stego_image_fname = "stego.bmp";
            return e_success;
        }

        else
        {
            ptr=strstr(argv[4],".bmp");
            if(ptr!=NULL)
            {

                if (strcmp(strstr(argv[4],"."),".bmp")==0)
                {
                    printf("argv[4] It is a .bmp extension\n");
                    encInfo -> stego_image_fname = argv[4];
                    return e_success;
                }
            }
            else
            {
                printf("Error not .bmp extension\n");
                return e_failure;
            }
        }
    }
}

/*Open all 3 files beautiful.bmp secret.txt stego.bmp
input: Structure members
output: open the files and return status
Description: open the files
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    printf("\nSuccess: Open files is successful");
    return e_success;
}

/*Do encoding
input: structure members
output: Print the status message ie. success or failure of particular function
Description: Function calls for all specific functions
 */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        if(check_capacity(encInfo)==e_success)
        {
            printf("\nSuccess: Check capacity is successful");
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image)==e_success)
            {
                printf("\nSuccess: Bmp header copied successfully");
                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
                {
                    printf("\nSuccess: Copied magic string successfully");
                    strcpy(encInfo ->  extn_secret_file,(strstr(encInfo->secret_fname,".")));
                    if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                    {
                        printf("\nSuccess: Encoded secret file extension size");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)
                        {
                            printf("\nSuccess: Encoded secret file extension");
                            if(encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
                            {
                                printf("\nSuccess: Encoded secret file size");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("\nSuccess: Encode secret data");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("\nSuccess: Encoded Remaining data");

                                    }
                                    else
                                    {
                                        printf("\nFailure: Encoding of remaining data");
                                    }
                                }
                                else
                                {
                                    printf("\nFailure: Encoding of secret data");
                                }
                            }
                            else
                            {
                                printf("\nFailure: Encoding of seret file size");
                            }
                        }
                        else
                        {
                            printf("\nFailure: Encoding secret file extension");
                        }
                    }
                    else
                    {
                        printf("\nFailure: encoding of secret file extn failed");
                    }
                }
                else
                {
                    printf("\nFailure: Magic string not copied");
                }              
            }
            else
            {
                printf("\nFailure: Bmp header not copied");
            }

        }
        else
            printf("\nFailure: Check_capacity has failed");
    }
    else
        printf("\nFailure: Open files has failed");
}

/*Get file size
input: Secret file pointer
output: return the size of secret file
Description: Function gets the size of the secret file
 */
uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret,0,SEEK_END);  
    return ftell(fptr_secret);
}

/*Check capacity
input: structure variable
output: return status success or failure
Description: Function to check whether bmp img size and header size is greater than the overall sizes of extension
 */
Status check_capacity(EncodeInfo *encInfo)
{
    int magic_len=strlen(MAGIC_STRING);
    magic_len=magic_len*8;
    int encode_ext_len=((strlen(strstr(encInfo -> secret_fname,".")))*8);
    int size_of_secret_file=get_file_size(encInfo -> fptr_secret); 
    encInfo -> size_secret_file=size_of_secret_file;
    int sum=magic_len + 32 + encode_ext_len + 32 + (size_of_secret_file * 8);
    int img_size=get_image_size_for_bmp(encInfo -> fptr_src_image);
    if((img_size-54) > sum)
        return e_success;
    else
        return e_failure; 

}

/*Copy the bmp header
input: beautiful file and stego file
output: return status success or failure
Description: Function copies 54 bytes from beautiful.bmp to stego.bmp
 */
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buf[54];
    rewind(fptr_src_image);
    rewind(fptr_stego_image);
    fread(buf,54,1,fptr_src_image);
    fwrite(buf,54,1,fptr_stego_image);
    if(ftell(fptr_src_image)==54)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/*Encode magic string
input: magic string
output: return status success or failure
Description: Function encodes the magic string byte by byte to stego.bmp
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo -> fptr_src_image, encInfo -> fptr_stego_image);
    return e_success;
}

/*Data to image
input: magic string,string len,source file and stegofile
output: status 
Description: Function gets the lsb bit of the individual data from source file and encodes in stego file by calling byte to lsb
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buf[8];
    for(int i=0;i<size;i++)
    {
        fread(buf,sizeof(char),8,fptr_src_image);
        encode_byte_to_lsb(data[i],buf);
        fwrite(buf,sizeof(char),8,fptr_stego_image);
    }
    return e_success;
}

/*Byte to lsb
input: data
output: status
Description: Function encodes the lsb bit by toggling and doing or operation with data
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=((image_buffer[i]& 0xFE) | (data & 1<<i) >>i);
    }
    return e_success;

}

/*Secret file extension size
input: source file and destination file
output: status
Description: Function reads 32 bytes from src file and copies to dst file
 */
Status encode_secret_file_extn_size(int size,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buf[32];
    fread(buf,32,1,fptr_src_image);
    encode_size_to_lsb(size,buf);
    fwrite(buf,32,1,fptr_stego_image);
    return e_success;
}

/*Size to lsb
input: size and array
output: status
Description: encode the extn size in stego.bmp 
 */
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=(image_buffer[i]& 0xFE) | ((size & 1<<i) >>i);
    }
    return e_success;
}

/*Secret file extension
input: secret file extension
output: status
Description: Function encodes the extn ie. .txt in stego.bmp by calling data to image function call
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo -> fptr_src_image,encInfo -> fptr_stego_image);
    return e_success;
}

/*Secret file size
input: file size
output: status
Description: Function copies the secret file size to stego.bmp
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buf[32];
    fread(buf,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buf);
    fwrite(buf,32,1,encInfo->fptr_stego_image);
    return e_success;
}

/*secret file data
input: Structure members
output: status
Description: Function encodes the secret message in the stego file
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    char buf[8];
    rewind(encInfo->fptr_secret);
    // printf("\n%d",encInfo->size_secret_file);
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(buf,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buf);
        fwrite(buf,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/*copy remaining data
input: File pointers
output: status
Description: Function copies the remaining data from src file to stego file
*/
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while((fread(&ch,1,1,fptr_src))>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;

}
