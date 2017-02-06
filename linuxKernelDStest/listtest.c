/* linux kernel list test */

#include <stdio.h>
#include <stdlib.h>

#include "list.h"


struct list_head{
	struct list_head *next, *prev;
};

struct testData{
	struct list_head list;
	int index;
};


int main()
{

	return 0;
}

