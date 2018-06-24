#include "Map.h"

byte operator==(struct kv_pair p1, struct kv_pair p2)
{
	return p1.key == p2.key && p1.value == p2.value;
}

byte operator!=(struct kv_pair p1, struct kv_pair p2)
{
	return !(p1 == p2);
}

uint count_nodes(struct rb_tree* t)
{
	uint c = 0;
	for(uint i = 0; i < MAX_NODES; i++) if(t->is_in_use[i]) c++;
	return c;
}

uint& rb_tree::operator[](uint key)
{
	if(is_key_in_tree(this, key)) 
	{
		return get_pair_of_key(this, key)->value;
	}
	else
	{
		struct kv_pair p = {key, 0};
		byte b = insert_kv_pair(this, p);
		return get_pair_of_key(this, key)->value;
	}
}

struct rb_tree* create_rb_tree()
{
	struct rb_tree* tree = (struct rb_tree*)malloc(sizeof(struct rb_tree));
	tree->root = NULL;
	return tree;
}

void destroy_rb_tree(struct rb_tree* tree)
{
	free(tree);
}

struct node* get_next_available_node(struct rb_tree* tree)
{
	for(uint i = 0; i < MAX_NODES; i++) 
	{
		if(!tree->is_in_use[i])
		{
			struct node* n = tree->nodes + i;
			n->parent = NULL;
			n->left_child = NULL;
			n->right_child = NULL;
			tree->is_in_use[i] = 1;
			return n;
		}
	}
	assert(0 && "Execution shouldn't reach here!");
}

struct kv_pair* get_pair_of_key(struct node* n, uint key)
{
	if(!n) 			return NULL; else
	if(n->pair.key == key) 	return &(n->pair); else
	if(key < n->pair.key) 	return get_pair_of_key(n->left_child, key); else
	if(key > n->pair.key)	return get_pair_of_key(n->right_child, key);

	return NULL; //Shouldn't get here but stops ycm plugin from complaining
}

struct kv_pair* get_pair_of_key(struct rb_tree* t, uint key)
{
	return get_pair_of_key(t->root, key);
}

byte is_root_property_upheld(struct rb_tree* tree)
{
	return (tree->root) ? tree->root->colour == BLACK : 1;
}

byte is_black_property_upheld(struct node* node)
{
	if(!node) return 1;
	else
	{
		uint b_height_left = is_black_property_upheld(node->left_child);
		if(!b_height_left) 			return 0;
		
		uint b_height_right = is_black_property_upheld(node->right_child);
		if(!b_height_right) 			return 0;
		
		if(b_height_left != b_height_right) 	return 0;
		else if(node->colour == BLACK) 		return b_height_left+1;
		else 					return b_height_left;
	}
}

//Checks that the number of black nodes on all paths from the root to the leaves are equal
byte is_black_property_upheld(struct rb_tree* tree)
{
	return is_black_property_upheld(tree->root);
}

byte is_red_property_upheld(struct node* node)
{
	if(!node) return BLACK;
	else
	{
		uint left_red_property_upheld = is_red_property_upheld(node->left_child);
		if(!left_red_property_upheld) 	return 0;
		
		uint right_red_property_upheld = is_red_property_upheld(node->right_child);
		if(!right_red_property_upheld) 	return 0;
		
		//Node exists and both subtrees uphold red property
		//Check if current node upholds red property
		byte red_property_upheld = (node->colour == BLACK || left_red_property_upheld == right_red_property_upheld == BLACK);
		return (red_property_upheld) ? node->colour : 0;
	}
}

//Checks that all red nodes have black children
byte is_red_property_upheld(struct rb_tree* tree)
{
	return is_red_property_upheld(tree->root);
}

byte is_balanced(struct rb_tree* tree)
{
	return is_root_property_upheld(tree) && is_black_property_upheld(tree) && is_red_property_upheld(tree);	
}

void rebalance_tree(struct rb_tree* tree, struct node* k, struct node* p)
{
	byte parent_sibling_colour = BLACK;
	struct node* g = p->parent;
	struct node* s = (g->left_child != p) ? g->left_child : g->right_child;
	if(g->left_child == p)
	{
		if(g->right_child) parent_sibling_colour = g->right_child->colour;
	}
	else
	{
		if(g->left_child) parent_sibling_colour = g->left_child->colour;
	}
	//Rebalance
	if(parent_sibling_colour == BLACK)
	{
		//Trinode restructure
		struct node* tree_nodes[3] = {k, p, g};
		uint swaps = -1;
		while(swaps > 0)
		{
			swaps = 0;
			for(uint i = 0; i < 2; i++)
			{
				if(tree_nodes[i]->pair.key > tree_nodes[i+1]->pair.key)
				{
					struct node* t = tree_nodes[i+1];
					tree_nodes[i+1] = tree_nodes[i];
					tree_nodes[i] = t;
					swaps++;
				}
			}
		}
		//There are 2 subtrees which need to be placed back in order
		//One is child of G, one is child of P
		//Set mid tree node as parent of other 2
		struct node* initial_left_child = tree_nodes[1]->left_child;
		struct node* initial_right_child = tree_nodes[1]->right_child;
		
		tree_nodes[1]->parent = g->parent;
		tree_nodes[1]->left_child = tree_nodes[0];
		tree_nodes[1]->right_child = tree_nodes[2];
		struct node** g_p_child = (g->left_child == p) ? &(g->left_child) : &(g->right_child);
		struct node** restruct_new_p_loc = NULL;
		if(tree_nodes[1]->parent)
		{
			restruct_new_p_loc = (tree_nodes[1]->parent->left_child == g) ? &(tree_nodes[1]->parent->left_child) : &(tree_nodes[1]->parent->right_child);
		}
		if(tree_nodes[1] == p)
		{
			//Set g's p child to p's non k child
			struct node** p_non_k_child = (initial_left_child != k) ? &(p->left_child) : &(p->right_child);
			if(initial_left_child != k) *g_p_child = initial_left_child;
			else *g_p_child = initial_right_child;
			g->parent = p;

		}	
		else //k is now parent node
		{
			struct node** p_k_child = (p->left_child == k) ? &(p->left_child) : &(p->right_child);
			*p_k_child = (k->pair.key > p->pair.key) ? initial_left_child : initial_right_child;
			*g_p_child = (k->pair.key > g->pair.key) ? initial_left_child : initial_right_child;
			g->parent = k;
			p->parent = k;
		}
		tree_nodes[1]->colour = BLACK;
		g->colour = RED;
		if(restruct_new_p_loc) *restruct_new_p_loc = tree_nodes[1];
	}
	else if(parent_sibling_colour == RED)
	{
		//Recolouring
		//Set g's colour to red
		if(!g->parent) g->colour = BLACK;
		else g->colour = RED;
		//Set p and s's colours to black
		p->colour = BLACK;
		s->colour = BLACK;
		if(g->colour == RED && g->parent->colour == RED) rebalance_tree(tree, g, g->parent);
	}
	struct node* n = tree->root;
	for(; n->parent != NULL; n = n->parent);
	tree->root = n;
}

//Returns 0 if key already exists in tree
byte insert_kv_pair(struct rb_tree* tree, struct kv_pair pair)
{
	//Calculate where pair should be written to
	struct node** location = &(tree->root);
	struct node* parent = NULL;
	while(*location != NULL)
	{
		parent = *location;
		if((*location)->pair.key == pair.key) return 0;
		else if(pair.key < (*location)->pair.key)
		{
			location  = &((*location)->left_child);
		}
		else
		{
			location = &((*location)->right_child);
		}
	}
	//Get next available node
	struct node* new_node = get_next_available_node(tree);
	new_node->pair = pair;
	new_node->colour = RED;
	new_node->left_child = NULL;
	new_node->right_child = NULL;
	new_node->parent = parent;
	*location = new_node;
	if(location == &(tree->root)) (*location)->colour = BLACK;
	else if(parent->colour == RED)
	{
		rebalance_tree(tree, new_node, parent);
	}
	return 1;
}

byte is_key_in_tree(struct rb_tree* t, uint key)
{
	struct node* n = t->root;
	while(n)
	{
		if(key < n->pair.key) n = n->left_child;
		else if(key > n->pair.key) n = n->right_child;
		else return 1;
	}
	return 0;
}

void execute_for_each_pair(struct node* n, void (*func)(struct kv_pair*, uint))
{
	if(n)
	{
		struct kv_pair* p = &(n->pair);
		execute_for_each_pair(n->left_child, func);
		func(p, n->colour);
		execute_for_each_pair(n->right_child, func);
	}
}

void execute_for_each_pair(struct rb_tree* t, void (*func)(struct kv_pair*, uint))
{
	execute_for_each_pair(t->root, func);	
}

void execute_for_each_pair(struct node* n, uint* ptr, void (*func)(struct kv_pair*, uint*))
{
	if(n)
	{
		struct kv_pair* p = &(n->pair);
		execute_for_each_pair(n->left_child, ptr, func);
		func(p, ptr);
		execute_for_each_pair(n->right_child, ptr, func);
	}
}

void execute_for_each_pair(struct rb_tree* t, uint* ptr, void (*func)(struct kv_pair*, uint*))
{
	execute_for_each_pair(t->root, ptr, func);
}
