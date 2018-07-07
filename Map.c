#include "Map.h"

uint hash_RGB(uint R, uint G, uint B)
{
	//Set result to be 0xRRGGBB
	uint result = 0x000000;
	R = (R << 16);
	G = (G << 8);
	B = (B << 0);
	result = R | G | B;
	return result;
}

byte operator==(struct kv_pair p1, struct kv_pair p2)
{
	return p1.key == p2.key && p1.value == p2.value;
}

byte operator!=(struct kv_pair p1, struct kv_pair p2)
{
	return !(p1 == p2);
}

uint& rb_tree::operator[](uint key)
{
	if(!is_key_in_tree(this, key)) insert_kv_pair(this, kv_pair{key, 0});
	return get_pair_of_key(this, key)->value;
}





struct rb_tree* create_rb_tree()
{
	struct rb_tree* t = new rb_tree();
	t->root = NULL;
	return t;
}

void destroy_rb_tree(struct rb_tree* t)
{
	delete t;
}





uint count_nodes(struct rb_tree* t)
{
	uint c = 0;
	for(uint i = 0; i < MAX_NODES; i++) if(t->is_in_use[i]) c++;
	return c;
}

struct node* get_next_available_node(struct rb_tree* t)
{
	for(uint i = 0; i < MAX_NODES; i++) 
	{
		if(!t->is_in_use[i])
		{//If node i isn't in use
			struct node* 	n 		= t->nodes + i;
					n->parent 	= NULL;
					n->left_child 	= NULL;
					n->right_child 	= NULL;

			t->is_in_use[i] = 1; //node i now in use
			
			return n;
		}
	}
	assert(0 && "Execution shouldn't reach here!");
}





byte node_contains_key(struct node* n, uint key)
{
	return n->pair.key == key;	
}

struct kv_pair* get_pair_of_key(struct node* n, uint key)
{
	if(!n) 				return NULL; 					else
	if(node_contains_key(n, key)) 	return &(n->pair); 				else
	if(key < n->pair.key) 		return get_pair_of_key(n->left_child, key); 	else
	if(key > n->pair.key)		return get_pair_of_key(n->right_child, key);

	return NULL; //Shouldn't get here but stops ycm plugin from complaining
}

struct kv_pair* get_pair_of_key(struct rb_tree* t, uint key)
{
	return get_pair_of_key(t->root, key);
}





byte is_key_in_tree(struct node* n, uint key)
{
	if(!n) return 0;
	else
	{
		if(key < n->pair.key) 		return is_key_in_tree(n->left_child, key); 	else
		if(key > n->pair.key) 		return is_key_in_tree(n->right_child, key);
		else 				return 1;
	}
}

byte is_key_in_tree(struct rb_tree* t, uint key)
{
	return is_key_in_tree(t->root, key);
}





//Bubble sort for length 3 array for trinode restructuring (array should contain k, p, g)
void sort_trinode_array(struct node* tree_nodes[3])
{
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
}

struct node* find_sibling_node(struct node* n)
{
	if(n->parent) 	return (n->parent->left_child != n) ? n->parent->left_child : n->parent->right_child;
	else 		return NULL;
}

byte sibling_colour(struct node* n)
{
	struct node* s = find_sibling_node(n);
	
	if(s) 	return s->colour;
	else 	return BLACK;
}

//Returns address of left_child or right_child variable in p which points to n
struct node** get_address_of_child_node_var(struct node* n, struct node* p)
{
	return (p->left_child == n) ? &(p->left_child) : &(p->right_child);
}

void trinode_restructure(struct node* k, struct node* p, struct node* g)
{
	//Step: Set pointers for 3 nodes which make up trinode restructure
	struct node* tree_nodes[3] = {k, p, g}; //These 3 constitute the trinode restructure
	sort_trinode_array(tree_nodes);

	struct node* new_parent_initial_left_child = 	tree_nodes[1]->left_child;
	struct node* new_parent_initial_right_child = 	tree_nodes[1]->right_child;
	
	//Set middle node as parent of other 2
	tree_nodes[1]->parent 		= g->parent;
	tree_nodes[1]->left_child 	= tree_nodes[0];
	tree_nodes[1]->right_child 	= tree_nodes[2];

	tree_nodes[0]->parent 		= tree_nodes[1];
	tree_nodes[2]->parent 		= tree_nodes[1];



	//Step: Set child nodes of the children nodes of the trinode restructure, as well as the child node of g's original parent
	struct node** g_p_child 		= get_address_of_child_node_var(p, g);
	struct node** restruct_new_p_loc 	= (tree_nodes[1]->parent) ? get_address_of_child_node_var(g, tree_nodes[1]->parent) : NULL;	
	
	if(tree_nodes[1] == p) 	
	{// If p is the new parent node
		//The child node ptr which pointed to p will point to p's child which was not k
		struct node* replacement_g_p_child = (k != new_parent_initial_left_child) ? new_parent_initial_left_child : new_parent_initial_right_child;
		*g_p_child = replacement_g_p_child;
		if(replacement_g_p_child) replacement_g_p_child->parent = g;
	}	
	else
	{// If k is the new parent node
		struct node** p_k_child = get_address_of_child_node_var(k, p);
		struct node* replacement_g_p_child = (k->pair.key > g->pair.key) ? new_parent_initial_left_child : new_parent_initial_right_child;
		struct node* replacement_p_k_child = (k->pair.key > p->pair.key) ? new_parent_initial_left_child : new_parent_initial_right_child;
		*p_k_child = replacement_p_k_child;
		if(replacement_p_k_child) replacement_p_k_child->parent = p;
		*g_p_child = replacement_g_p_child;
		if(replacement_g_p_child) replacement_g_p_child->parent = g;
	}
	if(restruct_new_p_loc) *restruct_new_p_loc = tree_nodes[1];


	
	//Step: Paint parent of trinode restructure black and its children red
	tree_nodes[1]->colour 	= BLACK;
	g->colour 		= RED;
}

void recolour(struct node* p, struct node* s, struct node* g)
{
	//Recolouring
	//Set g's colour to red unless g is root of tree
	g->colour = (!g->parent) ? BLACK : RED;
	
	p->colour = BLACK;
	s->colour = BLACK;
}

void reset_root_node(struct rb_tree* t)
{
	struct node* n = t->root;
	for(; n->parent != NULL; n = n->parent);
	t->root = n;
}

//FUTURE NOTE: If reading through this and confused by why non intuitive things are being done, consult http://pages.cs.wisc.edu/~paton/readings/Red-Black-Trees/#insert
//Obvious steps have been removed or changed for logical consequences of the structure of rb_trees and node insertion (such as g always being the max or min value of trinode restructure)
void rebalance_tree(struct rb_tree* t, struct node* k, struct node* p)
{
	byte parent_sibling_colour = sibling_colour(p);
	struct node* g = p->parent;
	
	if(parent_sibling_colour == BLACK)
	{
		trinode_restructure(k, p, g);
	}
	else if(parent_sibling_colour == RED)
	{
		struct node* s = find_sibling_node(p);
		recolour(p, s, g);
		if(g->colour == RED && g->parent->colour == RED) rebalance_tree(t, g, g->parent);
	}

	reset_root_node(t);
}

//Returns 0 if key already exists in tree
byte insert_kv_pair(struct rb_tree* t, struct kv_pair pair)
{
	//Step: Calculate address where a node containing pair should be written
	struct node** 	location 	= &(t->root);
	struct node* 	parent 		= NULL;

	while(*location)
	{
		parent = *location;
		if(node_contains_key(*location, pair.key)) return 0;
		
		else if(pair.key < (*location)->pair.key)
		{
			location  = &((*location)->left_child);
		}
		else
		{
			location = &((*location)->right_child);
		}
	}


	//Step: Write next available node to location 
	struct node* new_node = get_next_available_node(t);
	
	new_node->pair 		= pair;
	new_node->colour 	= RED;
	new_node->parent 	= parent;
	
	*location = new_node;

	//Step: Rebalance tree if it is unbalanced
	if	(*location == t->root)		(*location)->colour = BLACK; 		//Upholds root property
	else if	(parent->colour == RED)		rebalance_tree(t, new_node, parent); 	//Upholds black and red properties
	
	return 1;
}







//Really not a fan of the way I've done this, restructure TODO?
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

