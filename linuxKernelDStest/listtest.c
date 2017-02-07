/* linux kernel list test */

#include <stdio.h>
#include <stdlib.h>

#include "list.h"

struct test_data{
	struct list_head list;
	int index;
};


int main()
{
	LIST_HEAD(testDataList);

	int i;
	for(i=1;i<=10;++i){
		struct test_data *td = malloc(sizeof(struct test_data));
		td->list.next = NULL;
		td->list.prev = NULL;
		td->index = i;
		list_add(&td->list, &testDataList);
	}

	struct list_head *pos;
	struct test_data *tdp;
	list_for_each(pos, &testDataList){
		tdp = list_entry(pos, struct test_data, list);
		printf("%d\n",tdp->index);
	}

	while(!list_empty(&testDataList)){
		tdp = list_entry(testDataList.next, struct test_data, list);
		list_del(testDataList.next);
		free(tdp);
	}
	return 0;
}

