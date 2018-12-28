/* Written by: Yujie Tao
 * Onyen: yujiet
 *
 * This program read in character inputs and then generates 80 character line output
 * To compile code: gcc ex2.c -o ex2 -g
 *   
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Define max buffer length and input, output array
#define BUFLEN 80
char inbuf[BUFLEN + 1];
char outbuf[BUFLEN + 1];


int main () {
  int eof = 0;
  int rv;
  int outbuf_idx = 0;
  int inbuf_idx = 0;
  int valid_chars = 0;
  
  do {
    
    // Read in a buffer's worth of text, check for errors
    fgets(&inbuf[valid_chars],BUFLEN-valid_chars+2,stdin);
    valid_chars = strlen(inbuf);

     if (feof(stdin)){
      valid_chars++;
      inbuf[valid_chars]='\n';
    }



    for (int i = 0; i < valid_chars; i++) {

      // Special-case the newline
      if(inbuf[i]=='\n'){
        inbuf[i]=' ';
      }


      // Special-case for %%
      if(i< valid_chars-1 && inbuf[i]=='%' && inbuf[i+1]=='%'){
        inbuf[i]='*';
        for(int j = i+1; j<valid_chars; j++){
          inbuf[j]= inbuf[j+1];
        }
        valid_chars--;
      }

      // Edge case for two %% at the end of 80 char input
      if(inbuf[valid_chars-1]=='%'&&inbuf[valid_chars+1]=='%'){
        inbuf[valid_chars-1]='*';
        inbuf[valid_chars]='\n';
      }
     
    }

      // Handle the case where the inbuf is full and ready for printout
      if(valid_chars == BUFLEN+1){

        for (int k = 0; k < valid_chars; k++) {
          outbuf[k]= inbuf[k];
         }

         outbuf[BUFLEN]= '\n';
         inbuf[0] = inbuf[valid_chars-1];

        for (int k = 0; k < valid_chars; k++) {
            printf("%c",outbuf[k]);
         }

        //Reset inbuf array
        for(int k=1; k<valid_chars;k++){
          inbuf[k]='\n';
         }

         valid_chars = 1;
    }

    // Ctrl-D generates EOF in interactive shell
    if (feof(stdin)){
        exit(0);
    }
 
  } while (!eof);

  return rv;
}