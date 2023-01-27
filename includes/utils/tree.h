#ifndef SEMANTIC_AST_TREE_H_
#define SEMANTIC_AST_TREE_H_

#include "system.h"

#include <memory>
#include <utility>

NAMESPACE_BEGIN

namespace semantic
{
    template <class T>
    class Tree
    {
        template <class TT>
        class TreeNode
        {
            using NodeType        = TreeNode<TT>;
            using NodeTypePointer = TreeNode *;

            using ValueType       = TT;

            ValueType *value;
            NodeTypePointer left;
            NodeTypePointer right;

        public:
            TreeNode<TT>() : value(nullptr), left(nullptr), right(nullptr) {}

            template <class... Args>
            TreeNode<TT>(Args &&...args) : TreeNode<T>()
            {
                value = new TT(std::forward<Args>(args)...);
            }
        };

    public:
        TreeNode<T> root;
    };

}  // namespace semantic
NAMESPACE_END

#endif
