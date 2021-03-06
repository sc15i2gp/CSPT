#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "Pair.h"

typedef unsigned char byte;


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

#define MAX_NODES 512

struct rb_tree
{
	struct node nodes[MAX_NODES];
	byte is_in_use[MAX_NODES];
	struct node* root;
	uint& operator[](uint key);
};

uint hash_RGB(uint R, uint G, uint B);
byte operator==(struct kv_pair, struct kv_pair);
byte operator!=(struct kv_pair, struct kv_pair);
struct rb_tree* create_rb_tree();
void destroy_rb_tree(struct rb_tree*);
struct kv_pair* get_pair_of_key(struct rb_tree*, uint key);
byte is_balanced(struct rb_tree*);
byte insert_kv_pair(struct rb_tree*, struct kv_pair pair);
byte is_key_in_tree(struct rb_tree*, uint key);
void print_kv_pairs(struct rb_tree*, const char* formatted_str);
uint count_nodes(struct rb_tree*);
#endif
