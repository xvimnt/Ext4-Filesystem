#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "node.h"
#include "disk.h"
#include <io_user.h>

class semantic
{
public:
    semantic();
    node *compute(node* node_);
};

#endif // SEMANTIC_H
