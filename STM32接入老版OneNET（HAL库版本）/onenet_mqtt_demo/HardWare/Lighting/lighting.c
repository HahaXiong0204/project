#include "lighting.h"


uint16_t  lighting_value(void)
{
	uint16_t val=0,val1=0;
	HAL_ADC_Start(&hadc1);
	val=HAL_ADC_GetValue(&hadc1);
	val1=((4096-val)*100/4096);
	
	return val1;
}
