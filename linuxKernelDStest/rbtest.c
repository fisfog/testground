/* rbtest.c */

#include "rbtree.h"

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
	return 0;
}
