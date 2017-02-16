/* rbtest.c */

#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

#ifdef typeof
#define lengthof(arr) (sizeof(arr))/(sizeof(typeof(arr)))
#else
#define lengthof(arr) (sizeof(arr))/(sizeof(*(arr)))
#endif

struct testData{
	struct rb_node	rb_node;
	int		key;
	char		*name;
};

struct testData *testSearch(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;
	while(node){
		struct testData *data = container_of(node, struct testData, rb_node);
		if(key<data->key)
			node = node->rb_left;
		else if(key>data->key)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int testInsert(struct rb_root *root, struct testData *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	while(*new){
		struct testData *this = container_of(*new, struct testData, rb_node);
		parent = *new;
		if(data->key<this->key)
			new = &((*new)->rb_left);
		else if(data->key>this->key)
			new = &((*new)->rb_right);
		else
			return 0;
	}

	rb_link_node(&data->rb_node, parent, new);
	rb_insert_color(&data->rb_node, root);

	return 1;
}

int main(int argc, char **argv)
{
	struct rb_root testtree = RB_ROOT;
	int tarr[] = {2,5,1,6,8,9,3,4};
	char namearr[][10] = {"jack","mike","oven","rachel","kally","larry","ql","bell"};
	//char tarr[] = "abc";
	printf("debug:length of tarr[%ld]\n", lengthof(tarr));
	printf("debug:length of namearr[%ld]\n", lengthof(namearr));
	int tarr_n = lengthof(tarr);
	int i;
	for(i=0;i<tarr_n;++i){
		struct testData *td = malloc(sizeof(struct testData));
		td->key = tarr[i];
		td->name = namearr[i];
		//printf("debug:key:[%d],name[%s]\n",td->key,td->name);
		if(!testInsert(&testtree, td)){
			perror("rbtree insert err!");
			continue;
		}
	}
	struct rb_node *rnd;
	struct testData *tdp;
	for(rnd = rb_first(&testtree);rnd;rnd=rb_next(rnd)){
		tdp = rb_entry(rnd, struct testData, rb_node);
		printf("key:[%d],name[%s]\n",tdp->key,tdp->name);
		//rb_erase(rnd, &testtree);
		//free(tdp);
	}

	printf("testtree[%d]=%s\n",5,testSearch(&testtree, 5)->name);
	tdp = testSearch(&testtree,5);
	rb_erase(&(testSearch(&testtree,5)->rb_node), &testtree);
	free(tdp);

	for(rnd = rb_first(&testtree);rnd;rnd=rb_next(rnd)){
		tdp = rb_entry(rnd, struct testData, rb_node);
		printf("key:[%d],name[%s]\n",tdp->key,tdp->name);
		//rb_erase(rnd, &testtree);
		//free(tdp);
	}

	while(!RB_EMPTY_ROOT(&testtree)){
		rnd = rb_first(&testtree);
		tdp = rb_entry(rnd, struct testData, rb_node);
		rb_erase(rnd, &testtree);
		free(tdp);
	}
	return 0;
}
