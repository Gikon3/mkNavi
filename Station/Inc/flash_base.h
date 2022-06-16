#ifndef INC_FLASH_BASE_H_
#define INC_FLASH_BASE_H_

typedef struct
{
  union
  {
    uint8_t   quarter[4];
    uint32_t  whole;
  };
  uint16_t    port;
} FlashIP;

typedef struct
{
  uint8_t   release;
  uint8_t   function;
  uint16_t  bug;
} FlashStaRevision;

typedef struct
{
  FlashStaRevision  rev;
  struct
  {
    uint32_t        checksum;
    size_t          size;
  } firmware;
  uint8_t           boot;
  FlashIP           ip[10];
} FlashMemory;

#endif /* INC_FLASH_BASE_H_ */
