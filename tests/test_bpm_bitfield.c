#include <stdio.h> 
#include <string.h>
#include "../doc/bpm_position.h"

int main(void) {
   unsigned char raw_data[] = { 0x9a, 0x03, 0x00, 0x00, 0x41, 0x03, 0x00, 0x00, 0x19, 0xfc, 0x70, 0x08, 0x77, 0x03, 0x00, 0x80 };
   struct bpm_position pos;
   memset(&pos, 0 , sizeof(pos));
   pos.data_type = 2;
   pos.counter = 0x0377;
   pos.positionX=0x0870;
   pos.positionY=0xfc19;
   pos.intenistyX = 0x341;
   pos.intenistyY = 0x39A;
   printf("Size: %u\n", sizeof(pos));
   int i;
   unsigned char *pos_ptr = (char *)(&pos);
   for (i = 0 ; i<sizeof(pos); i++) {
     printf("%02X ", raw_data[i]);
   }
   printf("\n");
   for (i = 0 ; i<sizeof(pos); i++) {
     printf("%02X ", pos_ptr[i]);
   }
   printf("\n");
  
   if (memcmp(pos_ptr, raw_data, sizeof(pos)) != 0) {
     printf("Bitfield defined wrong\n");
     return 1;
   }
  return 0;
}
