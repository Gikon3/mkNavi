#ifndef INC_UTIL_TYPES_H_
#define INC_UTIL_TYPES_H_

typedef enum {
  bFalse = 0,
  bTrue = 1
} Bool;

typedef enum {
  pOff = 0,
  pOn = 1
} Power;

typedef enum {
  tTurnOff = 0,
  tTurnOn = 1
} Tumbler;

typedef void (*util_remove_t)(void *);

typedef enum {
  utilOK,
  utilInvalidIndex,
  utilNullPtrInArg,
  utilEmpty,
  utilMallocFail
} UtilStat;

#endif /* INC_UTIL_TYPES_H_ */
