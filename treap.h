#include <algorithm>
#include <cstdint>
#include <stack>
#include <cstdio>
#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <stack>

using namespace std;

class RandGenerator {
private:
    uint64_t seed = 0;
    constexpr static int32_t BASE = 1103515245;
    constexpr static int32_t OFFSET = 12345;
    constexpr static int32_t MOD = 1e9 + 7;

public:
    uint32_t treap_rand() {
        seed = (BASE * seed + OFFSET) % MOD;
        return seed;
    }

    void reset() {
        seed = 0;
    }
};

template<typename T>
class Treap;

template<typename T>
class TreapNode {

private:
    T val;
    TreapNode<T> *left;
    TreapNode<T> *right;
    int32_t count; /* number of repeated elements contained in current node */
    int32_t size; /* number of nodes in current subtree */
    uint32_t weight;

    friend class Treap<T>;

public:
    TreapNode() : val(T()), left(nullptr), right(nullptr), count(0), size(0), weight(0) {}

    TreapNode(T val, TreapNode *left, TreapNode *right,
              int32_t count, int32_t size, uint32_t weight) :
            val(val), left(left), right(right),
            count(count), size(size), weight(weight) {}
};

template<typename T>
class Treap {

private:

    RandGenerator rand;

    TreapNode<T> *treap_root = nullptr;
    
    void update(TreapNode<T> *now) {
        int num1,num2;
        if(now->left) num1=now->left->size;
        else num1=0;
        if(now->right) num2=now->right->size;
        else num2=0;
        now->size = num2 + num1 + now->count;
    }

    void split(TreapNode<T> *now,T value,TreapNode<T>*&x,TreapNode<T>*&y){
        if(now==NULL) {
            x=y=nullptr;
            return;}
        else{
            if(now->val<=value){
                x=now;
                split(now->right,value,now->right,y);
            }
            else{
                y=now;
                split(now->left,value,x,now->left);
            }
            update(now);
        }
    }

    TreapNode<T> *merge(TreapNode<T> *x, TreapNode<T> *y) {
        if (x == nullptr || y == nullptr) {
            if (x == nullptr) return y;
            else return x;
        }
        if (x->weight > y->weight) {
            x->right = merge(x->right, y);
            update(x);
            return x;
        } else {
            y->left = merge(x, y->left);
            update(y);
            return y;
        }
    }

    TreapNode<T> *search(TreapNode<T> *x, T v) {
        if (x == nullptr) return nullptr;
        TreapNode<T> *newNode = x;
        if (newNode->val == v) return newNode;
        if (newNode->val < v) return search(newNode->right, v);
        if (newNode->val > v) return search(newNode->left, v);
    };

    bool searchIns(T v) {
        TreapNode<T> *res = search(treap_root, v);
        if (res == nullptr) return false;
        else {
            res->count++;
            res->size++;
            return true;
        }
    }

    bool searchDel(T v) {
        if (treap_root == nullptr) return true;
        TreapNode<T> *res = search(treap_root, v);
        if (res == nullptr || res->count == 1) return false;
        else {
            res->count--;
            res->size--;
            return true;
        }
    }

    bool find(T v) {
        TreapNode<T> *res = search(treap_root, v);
        return (res != nullptr);
    }

    TreapNode<T> *newNode(T x) {
        auto *node = new TreapNode<T>(x, nullptr, nullptr, 0, 1, 0);
        node->weight = rand.treap_rand();
        node->count = 1;
        return node;
    }

public:
    
    void insert(T val) {
        /* Your code here. */
        if (searchIns(val)) return;
        TreapNode<T> *x = nullptr;
        TreapNode<T> *y = nullptr;
        split(treap_root, val, x, y);
        TreapNode<T> *res = merge(x, newNode(val));
        treap_root = merge(res, y);
    }

    void remove(T val) {
        /* Your code here. */
        if(!find(val)) return;
            if (treap_root == nullptr) return;
            TreapNode<T> *res = search(treap_root, val);
            if (res != nullptr && res->count != 1) {
                res->count--;
                res->size--;
                return;
            }
            else {
                TreapNode<T> *x = nullptr;
                TreapNode<T> *y = nullptr;
                TreapNode<T> *z = nullptr;

                split(treap_root, val, x, z);
                split(x, val - 1, x, y);
                y = merge(y->left, y->right);
                treap_root = merge(merge(x, y), z);
            }
    }

    T pre_element(T val) {
       TreapNode<T> *x = nullptr;
            TreapNode<T> *y = nullptr;
            TreapNode<T> *tmp = nullptr;
            split(treap_root, val - 1, x, y); //左子树的最大值即为pre_element
            tmp=x;
            if (tmp == nullptr) {
            merge(x, y);
            return -1;
            }
            while (tmp->right) tmp = tmp->right;//递归寻找最大值
            merge(x, y);//合并左右两个子树
            return tmp->val;
    }

    T suc_element(T val) { //和pre_element相似
        /* Your code here. */
        TreapNode<T> *x = nullptr;
        TreapNode<T> *y = nullptr;
        split(treap_root, val, x, y);
        TreapNode<T> *cur = y;
        if (cur == nullptr) {
            merge(x, y);
            return -1;
        }
        while (cur->left) cur = cur->left;
        merge(x, y);
        return cur->val;
    }

    int32_t rank(T val) {
        /* Your code here. */
        if (!find(val)) return -1;
        TreapNode<T> *x=nullptr;
        TreapNode<T> *y=nullptr;
        split(treap_root, val - 1, x, y); //某数的排名即为左子树的大小+1
        int32_t ans = x->size + 1;
        merge(x, y);
        return ans;
    }

    int32_t kth_element(int32_t rk) {
        /* Your code here. */
            if (rk > treap_root->size) return -1;
            TreapNode<T> *now = treap_root;
            while (now) {
                if (now->left) {
                    if (rk >= now->left->size + 1&&rk <= now->left->size + now->count)
                        break;
                } else {
                    if (1 <= rk && rk <= now->count)
                        break;
                }
            
                if (now->left && now->left->size >= rk)
                    now = now->left;
                else {
                if (now->left) rk -= now->left->size + now->count;
                else rk -= now->count;
                now = now->right;
                }
            }
        return now->val;
    }

     void clearNode(TreapNode<T> *node) {
            if (!node) return;
            clearNode(node->left);
            clearNode(node->right);
            delete node;
        }
    void clear() {
        /* Your code here. */
        clearNode(treap_root);
        treap_root->left = NULL;
        treap_root->right = NULL;
        treap_root = NULL;
        rand.reset();
        }

    std::string pre_traverse() {
        /* Your code here. */
            if (!treap_root) return "empty";
            stack<TreapNode<T>> sta;
            string str="";
            sta.push(*treap_root);
            while (!sta.empty()) {
                TreapNode<T> node = sta.top();
                sta.pop();
                str = str + to_string(node.val) + " ";
                if (node.right != nullptr) {
                    sta.push(*node.right);
                }
                if (node.left != nullptr) {
                    sta.push(*node.left);
                }
            }
        return str;
    }
};