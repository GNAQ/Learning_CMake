#include <cstdio>
#include <iostream>
#include "config.h"

#ifdef USE_STDMATH
	#include <cmath>
#else 
	#include "pw/pw.hpp"
#endif

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Use %s base power to calc!\n", argv[0]);
		return 1;
	}
	
	#ifdef USE_STDMATH
		printf("%s ^ %s = %lf\n", argv[1], argv[2], pow(atof(argv[1]), atof(argv[2])));
	#else 
		printf("%s ^ %s = %lf\n", argv[1], argv[2], Calc_Power(argv[1], argv[2]));
	#endif
	
	return 0;
}