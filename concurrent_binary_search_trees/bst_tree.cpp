#include <functional>
#include "bst_tree.h"
#include <atomic>
 
 
void bst_tree::insert(long long data) {
    node * new_node = new node(data);
    node * temp = nullptr;
 
 
 
    if(root == nullptr){
        //root = new_node;
        //return;
        if(root.compare_exchange_strong(temp, new_node)) return;
 
    }
    node * placeholder = root;
    while(1){
        if(data< placeholder->data && placeholder->left != nullptr){
            placeholder = placeholder->left;
        }else if(data < placeholder->data && placeholder->left == nullptr){
            if(placeholder->left.compare_exchange_strong(temp, new_node)) break;
        }else if(data > placeholder->data && placeholder->right == nullptr){
            if(placeholder->right.compare_exchange_strong(temp, new_node))break;
        }else if(data > placeholder->data){
            placeholder = placeholder->right;
        }
 
    }
 
 
 
    // Naimplementujte zde vlaknove-bezpecne vlozeni do binarniho vyhledavaciho stromu
}
 
bst_tree::~bst_tree() {
    // Rekurzivni funkce pro pruchod stromu a dealokaci pameti prirazene jednotlivym
    // uzlum
    std::function<void(node*)> cleanup = [&](node * n) {
        if(n != nullptr) {
            cleanup(n->left);
            cleanup(n->right);
 
            delete n;
        }
    };
    cleanup(root);
}
