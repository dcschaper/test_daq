#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "CAENDigitizer.h"

#include <unistd.h>
#include <sys/timeb.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/time.h>


uint32_t RegisterSetBits(uint32_t reg, int start_bit, int end_bit, int val);
long get_time();
