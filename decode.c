#include<stdio.h>
#include"common.h"
#include"decode.h"
#include"types.h"
#include<string.h>

/*read and validate args
input: argument vector 
output: status success or failure
Description: Function reads the arguments passed through CLA and checks extn
 */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ptr=strstr(argv[2],".bmp");
    if(ptr!=NULL)
    {
        if (strcmp(strstr(argv[2],"."),".bmp")==0)
        {
            printf("argv[2] is a .bmp extension\n");
            decInfo -> stego_image_fname = argv[2];
        }
    }
    else{
        printf("argv[2] is Not a .bmp file please provide .bmp file\n");
        return e_failure;
    }
    if(argv[3]==NULL)
    {
        strcpy(decInfo -> dest_fname,"output");
        return e_success;
    }
    else
    {
        printf("argv[3] is a %s extension\n",strstr(argv[3],"."));
        strcpy(decInfo -> dest_fname,argv[3]);
        return e_success; 
    }
}

/*Do decoding
input: Structure members
output: print status message 
Description: Function calls for all the specific functions
 */
Status do_decoding(DecodeInfo *decInfo)
{
    if(decode_open_files(decInfo)==e_success)
    {
        printf("Success: Open files is successful\n"); 
        if(decode_skip_header(decInfo->fptr_stego_image)==e_success)
        {
            printf("Success: Skip header is successful\n");
            if(decode_magic_string(decInfo)==e_success)
            {
                printf("Success: Decode magic string successful\n");
                if(decode_file_extn_size(decInfo)==e_success)
                {
                    printf("Success: Decode file extension size successful\n");
                    if(decode_file_extn(decInfo)==e_success)
                    {
                        printf("Success: Decode file extension successful\n");
                        if(decode_secret_file_extn_size(decInfo)==e_success)
                        {
                            printf("Success: Decode secret file size successful\n");
                            if(decode_secret_file_data(decInfo)==e_success)
                            {
                                printf("Success: Decoded secret data\n");
                            }
                            else
                            {
                                printf("Failure: Decoded secret data\n");
                            }
                        }
                        else
                        {
                            printf("Failue: Decode secret file size unsuccessful\n");
                        }
                    }
                    else
                    {
                        printf("Failue: Decode secret file extension unsuccessful\n");
                    }
                }
                else
                {
                    printf("Failue: Decode file extension size unsuccessful\n");
                }
            }
            else
            {
                printf("Failue: Decode magic string unsuccessful\n");
            }
        }
        else
        {
            printf("Failue: Decode skip header unsuccessful\n");
        }
    }
    else
    {
        printf("Failue: Opening of files unsuccessful\n");
    }

}
/*Open files
input: stego file
output: status
Description: Function opens the stego file
 */
Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    // printf("\nSuccess: Open files is successful");
    return e_success;
}

/*skip header
input: stego file pointer
output: status
Description: Function skips the header part of the stego file
 */
Status decode_skip_header(FILE *fptr)
{
    char buf[54];
    fread(buf,54,1,fptr);
    // printf("%d",ftell(fptr));
    return e_success;
}

/*Magic string
input: structure
output: status
Description: verifing whether the magic string is present or not 
 */
Status decode_magic_string(DecodeInfo *decInfo)
{
    char buf[8];
    char magic_str[3];
    int i;
    for(i=0;i<strlen(MAGIC_STRING);i++)
    {
        fread(buf,8,1,decInfo->fptr_stego_image);
        magic_str[i]=decode_byte_to_lsb(buf);
    }
    magic_str[i]='\0';
    if(strcmp(magic_str,"#*")==0)
        return e_success;
    else
        return e_failure;
}

/*byte to lsb
input: image buffer
output: character ie. 0x23 and 0x2a
Description: Function decodes the magic string from lsb bits of the data
 */
char decode_byte_to_lsb(char *image_buffer)
{
    char ch=0;
    int i;
    for(i=0;i<8;i++)
    {
        ch=(((image_buffer[i] & 0x01) << i) | ch);
    }
    //  printf("%x\n",ch);
    return ch;
}

/*file extn size
input: stego file
output: status
Description: Function reads 32bytes from stego file 
 */

Status decode_file_extn_size(DecodeInfo *decInfo) 
{
    char buf[32];
    fread(buf,32,1,decInfo->fptr_stego_image);
    decInfo->extn_size=decode_size_to_lsb(buf);
    // printf("%d\n",decInfo->extn_size);
    return e_success;
}

/*size to lsb
input: image buffer
output: return the extn size
Description: Function gets and return the extn size 
 */
uint decode_size_to_lsb(char *image_buffer)
{
    char size=0;
    int i;
    for(i=0;i<32;i++)
    {
        size=(((image_buffer[i] & 0x01) << i) | size);
    }
    //  printf("%x\n",size);
    return size;
}

/*file extn
input: stego file
output: status
Description: Function decodes the file extn and concatenates the dest file
 */
Status decode_file_extn(DecodeInfo *decInfo)
{
    char buf[8];
    char file_extn[decInfo->extn_size+1];
    int i;
    for(i=0; i<decInfo->extn_size; i++)
    {
        fread(buf,8,1,decInfo->fptr_stego_image);
        file_extn[i]=decode_byte_to_lsb(buf);

    }
    file_extn[i]='\0';   
    // printf("%s%s\n",decInfo->dest_fname,file_extn);
    fflush(stdout);
    strcat(decInfo->dest_fname,file_extn);

    decInfo->fptr_dest_file=fopen(decInfo->dest_fname,"w");
    printf("Destination file opened successfull\n");
    return e_success;
}

/*file extn size
input: structure members
output: status
Description: Function decodes the secret file extension size
 */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buf[32];
    fread(buf,32,1,decInfo->fptr_stego_image);
    decInfo->data_size=decode_size_to_lsb(buf);
    return e_success;
}

/*Secret data
input: structure members
output: status
Description: Function decodes the secret message and copies to dest file
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buf[8];
    int i;
    char secret_data[decInfo->data_size+1];

    for(i=0;i<decInfo->data_size;i++)
    {
        fread(buf,8,1,decInfo->fptr_stego_image);
        secret_data[i]=decode_byte_to_lsb(buf);
    }
    secret_data[i]='\0';
    // printf("%s",secret_data);
    fwrite(secret_data,decInfo->data_size,1,decInfo->fptr_dest_file);
    fclose(decInfo->fptr_dest_file);
    return e_success;
}
