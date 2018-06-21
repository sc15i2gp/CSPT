#include "Map.h"

struct node* null_node(struct rb_tree* tree)
{
	return tree->nodes;
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
	for(uint i = 0; i < 128; i++) if(!tree->is_in_use[i]) return tree->nodes + i;
	assert(0 && "Execution shouldn't reach here!");
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

void rebalance_tree(struct node* k, struct node* p)
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
		tree_nodes[1]->parent = g->parent;
		tree_nodes[1]->left_child = tree_nodes[0];
		tree_nodes[1]->right_child = tree_nodes[2];
		struct node** g_p_child = (g->left_child == p) ? &(g->left_child) : &(g->right_child);
		if(tree_nodes[1] == p)
		{
			//Set g's p child to p's non k child
			struct node** p_non_k_child = (p->left_child != k) ? &(p->left_child) : &(p->right_child);
			*g_p_child = *p_non_k_child;
		}	
		else //k is now parent node
		{
			struct node** p_k_child = (p->left_child == k) ? &(p->left_child) : &(p->right_child);
			*p_k_child = (k->pair.key > p->pair.key) ? k->left_child : k->right_child;
			*g_p_child = (k->pair.key > g->pair.key) ? k->left_child : k->right_child;
		}
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
		if(g->colour == RED && g->parent->colour == RED) rebalance_tree(g, g->parent);
	}
}

//Returns 0 if key already exists in tree
byte insert(struct rb_tree* tree, struct kv_pair pair)
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
	*location = get_next_available_node(tree);
	(*location)->pair = pair;
	(*location)->colour = RED;
	(*location)->left_child = NULL;
	(*location)->right_child = NULL;
	(*location)->parent = parent;
	if(location == &(tree->root)) (*location)->colour = BLACK;
	else if(parent->colour == RED)
	{
		rebalance_tree(*location, parent);
	}
	return 1;
}
