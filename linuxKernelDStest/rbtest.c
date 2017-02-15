/* rbtest.c */

#include "rbtree.h"

#ifdef typeof
#define lengthof(arr) (sizeof(arr))/(sizeof(typeof(arr)))
#else
#define lengthof(arr) (sizeof(arr))/(sizeof(*(arr)))
#endif

struct testData{
	struct rb_node	*rb_node;
	int		key;
	char		name[256];
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
		if(data->key>this->key)
			new = &((*new)->rb_left);
		else if(data->key<this->key)
			new = &((*new)->rb_right);
		else
			return 0;
	}

	rb_link_node(data->rb_node, parent, new);
	rb_insert_color(data->rb_node, root);

	return 1;
}

int main(int argc, char **argv)
{
	struct rb_root testtest = RB_ROOT;
	int tarr[] = {2,5,1,6,8,9,3,4};
	char namearr[][10] = {"jack","mike","oven","rachel","kally","larry","ql","bell"};
	//char tarr[] = "abc";
	printf("debug:length of tarr[%ld]\n", lengthof(tarr));
	printf("debug:length of namearr[%ld]\n", lengthof(namearr));
	int tarr_n = lengthof(tarr);
	int i;
	/*
	for(i=0;i<tarr_n;++i){
		struct testData *td = malloc(sizeof(testData));
	}
	*/

	return 0;
}
