// Modified ‘hello_world_small’ demonstrator using the JTAG UART and PIO interface
// written by C. Jakob, fbeit, h_da, December 2023, christian.jakob@h-da.de

#include "system.h"
#include "sys/alt_stdio.h"
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdint.h>

typedef unsigned int alt_u32;

#define __I volatile const // read-only permission
 #define __IO volatile // read/write permission ...
 #define __O volatile // write only permission ;-) doesn't work in C...

 typedef struct {
 __IO alt_u32 DATA_REG;
 __IO alt_u32 DIRECTION_REG;
 __IO alt_u32 INTERRUPTMASK_REG;
 __IO alt_u32 EDGECAPTURE_REG;
 __O alt_u32 OUTSET_REG;
 __O alt_u32 OUTCLEAR_REG;
 } PIO_TYPE;

 #define LEDS (*((PIO_TYPE *) 0x80011020 ))

 volatile unsigned long delay = 0;

 uint32_t circular_left_shift(uint32_t value, int shift) {
     return (value << shift) | (value >> (32 - shift));
 }

 void sha_1(uint32_t * hash_ptr, const uint32_t * message, const uint32_t * prev_hash)
 {
 	uint32_t word[80];


 	for (int i = 0; i < 16; ++i) {
 	        word[i] = message[i];
 	    }
 	    for (int i = 16; i < 32; ++i) {
 	    	word[i] = circular_left_shift((word[i-3] ^ word[i-8] ^ word[i-14] ^ word[i-16]), 1);
 	    	//printf("\n word %d is :%x",i,word[i]);
 	    }
 	    for(int i = 32; i<80;i++)
 	    {
 	    	word[i] = circular_left_shift((word[i-6] ^ word[i-16] ^ word[i-28] ^ word[i-32]), 2);
 	    }

 		//constants
 		uint32_t A = prev_hash[0];
 		uint32_t B = prev_hash[1];
 		uint32_t C = prev_hash[2];
 		uint32_t D = prev_hash[3];
 		uint32_t E = prev_hash[4];


 				for(int round = 0; round < 80 ; round++)
 				{
 					//Group 0
 					if(round>= 0 && round < 20)
 					{
 						uint32_t f = (B & C) | (~B & D);
 						uint32_t temp = f + E + (circular_left_shift(A, 5)) + word[round] + 0x5A827999;


 						E = D;
 						D = C;
 						C = circular_left_shift(B,30);
 						B = A;
 						A = temp;
 					}
 					//group 1
 					else if(round>= 20 && round < 40){

 						uint32_t f = B ^C ^D;
 						uint32_t temp = f + E + (circular_left_shift(A, 5)) + word[round] + 0x6ED9EBA1;


 						E = D;
 						D = C;
 						C = circular_left_shift(B,30);
 						B = A;
 						A = temp;
 					}
 					//group 2
 					else if(round>= 40 && round < 60){

 						uint32_t f = (B & C) | (B & D) | (C & D) ;
 						uint32_t temp = f + E + (circular_left_shift(A, 5)) + word[round] + 0x8F1BBCDC;

 						E = D;
 						D = C;
 						C = circular_left_shift(B,30);
 						B = A;
 						A = temp;
 					}
 					//group 3
 					else if(round >=60 && round < 80){

 						uint32_t f = B ^C ^D;
 						uint32_t temp = f + E + (circular_left_shift(A, 5)) + word[round] + 0xCA62C1D6;

 						E = D;
 						D = C;
 						C = circular_left_shift(B,30);
 						B = A;
 						A = temp;
 					}

 					}
 				//Output of one chunk is gonna be input of next chunk
 				hash_ptr[0] = prev_hash[0] + A;
 				hash_ptr[1] = prev_hash[1] + B;
 				hash_ptr[2] = prev_hash[2] + C;
 				hash_ptr[3] = prev_hash[3] + D;
 				hash_ptr[4] = prev_hash[4] + E;

 }
 void sha1_arbitary(const char * message, uint32_t * padded_message) {
     size_t total_len_bytes = strlen(message);
     uint64_t total_msg_bits = (uint64_t)total_len_bytes * 8;
     uint32_t prev_hash[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0}; //only for first time
     uint32_t hash_out[5] = {0};

     // Calculate the total length after padding: message + 1 bit + 0 bits + 64-bit length field
     size_t total_padded_len_bytes = ((total_len_bytes + 1 + 8 + 63) / 64) * 64;
     uint16_t num_of_chunks = total_padded_len_bytes / 64;

     //pading
     for (int i = 0; i < num_of_chunks; i++) {
         // make the padded message 0 every time we process new chunk
         memset(padded_message, 0, 64);

         // Calculate the starting index of the current chunk
         //0th chunk will start taking string from 0 and 1st chunk from 64th byte...
         size_t chunk_base_index = i * 64;

         // Copy the message into the current chunk one byte by byte
         //stop at end of chunk ka string or if message ka final bytes are reached
         for (int str_itr= 0; str_itr < 64 && chunk_base_index + str_itr < total_len_bytes; str_itr++) {
             padded_message[str_itr / 4] |= (uint32_t)(message[chunk_base_index + str_itr]) << (24 - 8 * (str_itr % 4));
         }

         // Apply '0x80' bit padding if current chunk has end of main message
         //%64 to find the last positon of message in that 64 byte chunk
         if (chunk_base_index <= total_len_bytes && chunk_base_index + 64 > total_len_bytes) {
             padded_message[(total_len_bytes % 64) / 4] |= 0x80 << (24 - 8 * (total_len_bytes % 4));
         }

         // Add message length in bits at the end of the last chunk
         if (i == num_of_chunks - 1) {
             padded_message[14] = (uint32_t)(total_msg_bits >> 32);
             padded_message[15] = (uint32_t)(total_msg_bits & 0xFFFFFFFF);
         }

         // Process the chunk
         sha_1(hash_out, padded_message, prev_hash);

         // Update prev_hash for the next chunk
          prev_hash[0] = hash_out[0];
          prev_hash[1] = hash_out[1];
          prev_hash[2] = hash_out[2];
          prev_hash[3] = hash_out[3];
          prev_hash[4] = hash_out[4];
     }

     for (int i = 0; i < 5; i++)
         printf("\n%x", hash_out[i]);
    	 //alt_putstr(hash_out[i]);

     //check and glow LEDs
     if(hash_out[0] == 0xa617f4b3 &&
    	hash_out[1]	== 0xa108b6dd &&
		hash_out[2]	==	 0x82bb8c4a &&
		hash_out[3]	==	 0x16ab0b35 &&
		hash_out[4]	==	 0x2a32a0b9)
     {
    	 while(1){
    	  LEDS.DATA_REG = 0x00; //ON only if hash is matching
    	  for(delay = 0; delay < 100000; delay++) {};
    	  }
     }
 }

 int main(void){
 LEDS.DATA_REG = 0xFF; //LEDs OFF initially
 char* message = "FSOC23/24 is fun!";
 alt_putstr("The input to be hashed is: ");
 alt_putstr(message);
 alt_putstr("\nThe output hash is: ");


 uint32_t padded_msg[16] = {0};
 sha1_arbitary(message,padded_msg);
 return 0;
 }
