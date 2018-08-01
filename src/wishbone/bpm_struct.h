#ifndef __XWB_SCOPE_H__
#define __XWB_SCOPE_H__

#ifdef __cplusplus
extern "C" {
#endif


struct bpm_position {
   uint32_t intensity_flagsY;   
   uint32_t intensity_flagsX;  
   int16_t posY;
   int16_t posX;
   uint32_t flags_cycle_counter;
};

#define BPM_DEFAULT_RADIUS 50
#define BPM_TO_MM(X, RADUIS) ((((double)X) * ((double) BPM_DEFAULT_RADIUS)) / (1 << 15))
#define BPM_GET_COUNTER(X) (X & 0x3FF)
#define BPM_GET_INTENSITY(X) (X & 0x3FFFFFFF)

#define BPM_POSITION_GET_COUNTER(X) (BPM_GET_COUNTER(X.flags_cycle_counter))
#define BPM_POSITION_GET_INTENSITY_X(X) (BPM_GET_INTENSITY(X.intensity_flagsX))
#define BPM_POSITION_GET_INTENSITY_Y(X) (BPM_GET_INTENSITY(X.intensity_flagsY))

#ifdef __cplusplus
}
#endif

#endif
