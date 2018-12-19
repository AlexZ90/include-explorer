#ifndef ERROR_PRAGMA_TEST
#error "Test error"
#endif
#include <stdio.h>
#include "foo1.h"


void main ()
{
	foo1();
	foo3();
	printf (STR);
}