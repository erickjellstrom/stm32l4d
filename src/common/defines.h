

#define DEC_TO_BCD(val)   (( (val / 10) << 4) | (val % 10) )
#define BCD_TO_DEC(val)   (( ((val) >> 4) * 10 ) + ((val) & 0x0F))