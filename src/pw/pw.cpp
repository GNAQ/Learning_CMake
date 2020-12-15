#include <cmath>
#include <cstring>
#include <cstdio>

double Calc_Power(char *base_ch, char *pw_ch)
{
	puts("Using my own power calc function now");
	
	double base, pw;
	base = atof(base_ch);
	pw = atof(pw_ch);
	return pow(base, pw);
}