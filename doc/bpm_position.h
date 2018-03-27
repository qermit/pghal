#ifndef __PGHAL_H__
#define __PGHAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error("BIG ENDIAN not supported")
#endif

#if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

struct __attribute__((__packed__))  bpm_position {
  unsigned int intenistyY : 30;
  unsigned int flagsY : 2;
  unsigned int intenistyX : 30;
  unsigned int flagsX : 2;
  int positionY  : 16;
  int positionX  : 16;
  unsigned int counter   : 16;
  unsigned int cycle_id  : 14;
  unsigned int data_type : 2;
} ;

#endif

#if !defined(__BYTE_ORDER__)
#error("ENDIANNESS unknown")
#endif

#ifdef __cplusplus
extern }
#endif

#endif
