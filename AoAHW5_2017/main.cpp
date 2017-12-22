#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>

using String = std::string;
using UInt64 = uint64_t;
using Int64 = int64_t;
using InputFileStream = std::ifstream;

class Tree {
public:
    //  An enum class holding the color of the node
    enum class Color {
        Red,
        Black
    };
    
    //  An enum class holding the gender of the person
    enum class Gender {
        Male,
        Female
    };
    
private:
    //  Node struct which holds the tree nodes and connections
    struct Node {
        String name;
        Gender gender;
        UInt64 age;
        
        Color color;
        
        UInt64 femaleCount;
        UInt64 maleCount;
        
        Node *parent;
        Node *left;
        Node *right;
    };
    
    //  BST: Pushes node to the tree with `subroot` root
    static void enqueue(Node *incoming, Node *subroot) noexcept {
        //  Compare keys of the incoming node versus subroot
        if (incoming->name < subroot->name) {
            //  Check if left side of the subroot is nil
            if (!subroot->left) {
                subroot->left = incoming;
                incoming->parent = subroot;
            } else {
                enqueue(incoming, subroot->left);
            }
        } else {
            //  Check if right side of the subroot is nil
            if (!subroot->right) {
                subroot->right = incoming;
                incoming->parent = subroot;
            } else {
                enqueue(incoming, subroot->right);
            }
        }
    }
    
    //  Counting number of the man in subroot
    static inline UInt64 countMen(Node *subroot) {
        UInt64 sum = 0LL;
        
        if (subroot->left) sum += countMen(subroot->left);
        if (subroot->right) sum += countMen(subroot->right);
        
        if (subroot->gender == Gender::Male) {
            subroot->maleCount = sum + 1;
        } else {
            subroot->maleCount = sum;
        }
        
        return subroot->maleCount;
    }
    
    static inline UInt64 countWomen(Node *subroot) {
        UInt64 sum = 0LL;
        
        if (subroot->left) sum += countWomen(subroot->left);
        if (subroot->right) sum += countWomen(subroot->right);
        
        if (subroot->gender == Gender::Female) {
            subroot->femaleCount = sum + 1;
        } else {
            subroot->femaleCount = sum;
        }
        
        return subroot->femaleCount;
    }
    
    static Node *findManWithIndex(Node *subroot, const UInt64 index) noexcept {
        UInt64 relevance = subroot->left ? subroot->left->maleCount : 0;
        
        if (subroot->gender == Gender::Male) relevance += 1;
        
        if (index == relevance) {
            return subroot->gender == Gender::Male ? subroot : subroot->left;
        } else if (index < relevance) {
            return findManWithIndex(subroot->left, index);
        } else {
            return findManWithIndex(subroot->right, index - relevance);
        }
    }
    
    //  Finding the womam with given index.
    static Node *findWomanWithIndex(Node *subroot, const UInt64 index) noexcept {
        UInt64 relevance = subroot->left ? subroot->left->femaleCount : 0;
        
        if (subroot->gender == Gender::Female) relevance += 1;
        
        if (index == relevance) {
            return subroot->gender == Gender::Female ? subroot : subroot->left;
        } else if (index < relevance) {
            return findWomanWithIndex(subroot->left, index);
        } else {
            return findWomanWithIndex(subroot->right, index - relevance);
        }
    }
            
    //  Prints the node with given depth using inorder typing
    inline void printNode(Node *subroot, UInt64 level = 0, bool leftHanded = false) const noexcept {
        if (subroot->left) printNode(subroot->left, level + 1, true);
            
        for (UInt64 i = 0; i < level; ++i) {
            std::printf("\t");
        }
            
        if (level != 0) {
            if (leftHanded) {
                std::printf("┌");
            } else {
                std::printf("└");
            }
        }
            
        for (UInt64 i = 0; i < level; ++i) {
            std::printf("-");
        }
        
        std::printf("(%c)%s-%llu-%c\n", subroot->color == Color::Red ? 'R':'B', subroot->name.c_str(), subroot->age, subroot->gender == Gender::Male ? 'M':'F');
        
        if (subroot->right) printNode(subroot->right, level + 1, false);
    }
            
    //  Rotates the pivot to the left (slightly changed version of the one written on textbook).
    inline void rotateLeft(Node *pivot) noexcept {
        Node *temporary = pivot->right;
        
        if (temporary) {
            if (temporary->left) {
                pivot->right = temporary->left;
                temporary->left->parent = pivot;
            } else {
                pivot->right = nullptr;
            }
            
            if (pivot->parent) {
                temporary->parent = pivot->parent;
            }
            
            if (pivot->parent) {
                if (pivot == pivot->parent->left) {
                    pivot->parent->left = temporary;
                } else {
                    pivot->parent->right = temporary;
                }
            } else {
                root = temporary;
            }
            
            temporary->left = pivot;
            pivot->parent = temporary;
        }
    }
            
    //  Rotates the pivot to the right (slightly changed version of the one written on textbook).
    inline void rotateRight(Node *pivot) noexcept {
        Node *temporary = pivot->left;
        
        if (temporary) {
            if (temporary->right) {
                pivot->left = temporary->right;
                temporary->right->parent = pivot;
            } else {
                pivot->left = nullptr;
            }
            
            if (pivot->parent) {
                temporary->parent = pivot->parent;
            }
            
            if (pivot->parent) {
                if (pivot == pivot->parent->left) {
                    pivot->parent->left = temporary;
                } else {
                    pivot->parent->right = temporary;
                }
            } else {
                root = temporary;
            }
            
            temporary->right = pivot;
            pivot->parent = temporary;
        }
    }
    
    //  Triggers the counter function to count adults from root of the tree.
    inline void countMen() noexcept {
        countMen(root);
    }
    
    //  Triggers the counter function to count kids from root of the tree.
    inline void countWomen() noexcept {
        countWomen(root);
    }
            
public:
    Tree() : root(nullptr) {
        //  Blank implementation
    }
    
    //  Inserts element with given attributes
    inline void insert(String name, Gender gender, UInt64 age) noexcept {
        auto incoming = new Node();
        
        incoming->name = name;
        incoming->gender = gender;
        incoming->age = age;
        incoming->color = Color::Red;
        
        //  Make it root if there is no root yet
        if (!root) {
            incoming->color = Color::Black;
            root = incoming;
        } else {
            enqueue(incoming, root);
            
            while (incoming != root && incoming->parent->color == Color::Red) {
                if (incoming->parent == incoming->parent->parent->left) {
                    auto uncle = incoming->parent->parent->right;
                    
                    //  Case I
                    if (uncle && uncle->color == Color::Red) {
                        //  Recolor from the uncle node
                        uncle->color = Color::Black;
                        incoming->parent->color = Color::Black;
                        uncle->parent->color = Color::Red;
                        
                        incoming = uncle->parent;
                    } else {
                        //  Case II
                        if (incoming == incoming->parent->right) {
                            incoming = incoming->parent;
                            rotateLeft(incoming);
                        }
                        
                        //  Case III
                        incoming->parent->color = Color::Black;
                        incoming->parent->parent->color = Color::Red;
                        rotateRight(incoming->parent->parent);
                    }
                } else {
                    auto uncle = incoming->parent->parent->left;
                    
                    if (uncle && uncle->color == Color::Red) {
                        //  Recolor from the uncle node
                        uncle->color = Color::Black;
                        incoming->parent->color = Color::Black;
                        uncle->parent->color = Color::Red;
                        
                        incoming = uncle->parent;
                    } else {
                        if (incoming == incoming->parent->left) {
                            incoming = incoming->parent;
                            rotateRight(incoming);
                        }
                        
                        incoming->parent->color = Color::Black;
                        incoming->parent->parent->color = Color::Red;
                        rotateLeft(incoming->parent->parent);
                    }
                }
                
                root->color = Color::Black;
            }
        }
        
        countMen();
        countWomen();
    }
    
    inline void print() const noexcept {
        if (root) printNode(root);
    }
    
    inline Node *findManWithIndex(UInt64 index) {
        return findManWithIndex(root, index);
    }
    
    inline Node *findWomanWithIndex(UInt64 index) {
        return findWomanWithIndex(root, index);
    }
    
private:
    Node *root = nullptr;
};
    
    
int main(int argc, const char * argv[]) {
    if (argc != 2) return 9;
    
    InputFileStream inputFile(argv[1]);
    
    if (!inputFile.is_open()) return 8;
    
    String buffer;
    Tree tree;
    
    while (std::getline(inputFile, buffer)) {
        String name = buffer.substr(0, buffer.find('\t'));
        
        buffer.erase(0, buffer.find('\t') + 1);
        
        Tree::Gender gender = buffer[0] == 'M' ? Tree::Gender::Male : Tree::Gender::Female;
        
        buffer.erase(0, buffer.find('\t') + 1);
        
        UInt64 age = std::stoll(buffer);
        
        tree.insert(name, gender, age);
    }
    
    tree.print();
    std::cout << "3rd woman: " << tree.findWomanWithIndex(3)->name << std::endl;
    std::cout << "4th man: " << tree.findManWithIndex(4)->name << std::endl;
    
    return 0;
}
