// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

#include <iostream>
#include <queue>

#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap(); // constructor
    ~MyMap(); // destructor
    void clear(); // deletes	all	of the tree nodes producing an empty tree
    int size() const; // return the number of associations in a map
    
    // The associate method associates one item(key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the tree with that key/value pair. If there is
    // already an association with that key in the tree, then the item
    // associated with that key is replaced by the second parameter (value).
    //Thus, the tree contains no duplicate keys.
    void associate(const KeyType& key, const ValueType& value);
    
    // If no assocaition exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value, and if the map is allowed to be modified, to
    // modify that value directly within the map (the second overload enables this).
    
    // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;
    
    // for a modifiable map, return a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
    }
    
    // C++11 syntax for preventing copying and assignment
    MyMap(const MyMap&) = delete;
    MyMap& operator=(const MyMap&) = delete;
    
private:
    // define Node struct, a node has a key, value, and left and right pointers
    struct Node
    {
        // construct new Node with key/value
        Node(const KeyType &key, const ValueType &value)
        :m_key(key), m_value(value){
            // new nodes have no children
            m_left = m_right = nullptr;
        }
        KeyType m_key;
        ValueType m_value;
        Node *m_left, *m_right;
    };
    // root of the tree
    Node *m_root;
    // helper recursive function to delete tree
    void freeTree(Node* &cur);
};

// implementation of private helper methods
template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::freeTree(MyMap::Node* &cur)
{
    // reached the end, deleted all nodes
    if(cur == nullptr){
        return;
    }
    // call recursive function on left sub tree
    freeTree(cur->m_left);
    // call recursive function on right sub tree
    freeTree(cur->m_right);
    // delete current node
    delete cur;
    // set to nullptr
    cur = nullptr;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
{
    // construct an empty map
    m_root = nullptr;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap()
{
    // implements the recursive helper function to delete and free tree
    freeTree(m_root);
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear()
{
    // implements the recursive helper function to delete and free tree
    freeTree(m_root);
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const
{
    int count = 0;
    // empty map has size 0
    if(m_root == nullptr){
        return count;
    }
    // otherwise, use level order traversal to enumerate tree
    Node* temp;
    // queue of nodes, add root node to the queue
    std::queue<Node*> nodes;
    nodes.push(m_root);
    while(!nodes.empty()){
        // get first node in the queue
        temp = nodes.front();
        nodes.pop();
        // counts number of nodes in the queue
        count++;
        // add children to the queue from left to right
        if(temp->m_left != nullptr){
            nodes.push(temp->m_left);
        }
        if(temp->m_right != nullptr) {
            nodes.push(temp->m_right);
        }
    }
    return count;
}

template<typename KeyType, typename ValueType> void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
    // empty map
    if(m_root == nullptr){
        // create root
        m_root = new Node(key, value);
        return;
    }
    Node *cur = m_root;
    for(;;){
        // if key already exists in map
        if(key == cur->m_key){
            // reassign the value associated with key
            cur->m_value = value;
            return;
        }
        if(key < cur->m_key){
            // left child exists already
            if(cur->m_left != nullptr){
                // keep traversing
                cur = cur->m_left;
            }else{
                cur->m_left = new Node(key, value);
                return;
            }
        }else if(key > cur->m_key){
            // right child exists already
            if(cur->m_right != nullptr){
                // keep traversing
                cur = cur->m_right;
            }else{
                // if a child doesn't exist already, create a new node in this spot
                cur->m_right = new Node(key, value);
                return;
            }
        }
    }
}

template<typename KeyType, typename ValueType> const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const
{
    Node *cur = m_root;
    while(cur != nullptr){
        // if found the association
        if(key == cur->m_key){
            // returns a pointer to the value
            return &cur->m_value;
        }else if(key < cur->m_key){
            cur = cur->m_left;
        }else if(key > cur->m_key){
            cur = cur->m_right;
        }
    }
    // if we hit a null pointer, we didn't find an association
    return nullptr;
}

#endif
