#include "main.h"
#include "stm32f3xx_hal.h"
#include "gpio.h"
#include "stdbool.h"

void Led_on(uint8_t Lednr);
void Show_points(uint8_t Left, uint8_t Right);
bool L_hit(void);
bool R_hit(void);