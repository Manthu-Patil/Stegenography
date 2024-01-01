/*
Name: Mahantesh Patil
Description: Stegenography project
Date: 15-11-2023
Sample input:
Sample output:
 */

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"


/*Check minimum number of arguments passed for encoding or decoding process
  Check user choice encoding or decoding
  Read and validate the arguments print the success message
  Finally print the success message of overall encoding or decoding
 */

int main(int argc,char *argv[])
{
    if(argc<3 && (strcmp(argv[1],"-d")==0))
    {
        printf("Error: Please pass valid arguments\n ./a.out -d <stego.bmp>");
        return 0;
    }
    else if(argc<4 && (strcmp(argv[1],"-e")==0))
    {
        printf("Error : Please pass valid arguments\n ./a.out -e <source.bmp> <secret.txt>\n");
        return 0;
    }
    //encode 
    if(check_operation_type(argv)==e_encode)
    {
        printf("User choice is encoding\n");
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo)==e_success)
        {
            printf("read and validate successfull\n");
            if(do_encoding(&encInfo)==e_success)
            {
                printf("encoding is successful\n");
            }
        }
        else
        {
            printf("read and validate unsuccessfull\n");
        }

    }
    //decode
    else if(check_operation_type(argv)==e_decode)
    {
        printf("User choice is decoding\n");
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv,&decInfo)==e_success)
        {
            printf("read and validate successfull\n");
            if(do_decoding(&decInfo)==e_success)
            {
                printf("Do decoding is successful\n");
            }
        }
        else
        {
            printf("read and validate unsuccessful\n");
        }

    }
    else if(check_operation_type(argv)==e_unsupported)
    {
        printf("Please provide -e or -d choice\n");
    }    
}
