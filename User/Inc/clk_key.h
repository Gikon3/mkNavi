#ifndef INC_CLK_KEY_H_
#define INC_CLK_KEY_H_

typedef enum {
  clkInternal,
  clkExternal,
  clkDisable
} ClkKey;

void clk_key_sel_src(ClkKey selClk);

#endif /* INC_CLK_KEY_H_ */
