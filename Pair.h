#ifndef PAIR_H
#define PAIR_H

typedef unsigned int uint;

struct kv_pair
{
	uint key;
	uint value;
};

static struct kv_pair pairs[2] = 
{
	kv_pair{0, 0},
	kv_pair{1, 0}
};

#endif
