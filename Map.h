#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <assert.h>

typedef unsigned char byte;
typedef unsigned int uint;

struct kv_pair
{
	uint key;
	uint value;
};


#define BLACK 1
#define RED 2

struct node
{
	byte colour;
	struct kv_pair pair;
	struct node* left_child;
	struct node* right_child;
	struct node* parent;
};

struct rb_tree
{
	struct node nodes[128];
	byte is_in_use[128];
	struct node* root;
};

#endif
