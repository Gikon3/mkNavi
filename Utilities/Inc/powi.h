#ifndef INC_POWI_H_
#define INC_POWI_H_

static int32_t powi(int32_t base, uint32_t degree)
{
  if (!degree) {
    return 1;
  }
  int32_t tmp = base;
  for (size_t i = 0; i < (degree - 1); ++i) {
    tmp *= base;
  }
  return tmp;
}

#endif /* INC_POWI_H_ */
