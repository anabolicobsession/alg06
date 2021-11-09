#include <iostream>
#include <iomanip>
#include <limits>
#include <queue>
#include <map>

using namespace std;

class AVLTree {
private:
    typedef int key_t;
    typedef int n_keys_t; // to use for variables that are bounded above by the number of keys

    static const key_t KEY_NEGATIVE_INF = numeric_limits<key_t>::min();
    static const key_t KEY_POSITIVE_INF = numeric_limits<key_t>::max();
    static const n_keys_t DEF_NONEXISTENT_NODE_HEIGHT = -1;
    static const n_keys_t DEF_NODE_HEIGHT = 0;

    class Node {
    public:
        key_t key;
        Node *left, *right;
        int height;

        explicit Node(key_t key) : key(key), left(nullptr), right(nullptr), height(DEF_NODE_HEIGHT) {}
    };

    Node *root;

    static int height(Node *n) {
        return n != nullptr ? n->height : DEF_NONEXISTENT_NODE_HEIGHT;
    }

    static void update_height(Node *n) {
        n->height = 1 + max(height(n->left), height(n->right));
    };

    static Node* left_rotate(Node *n) {
        Node *new_root = n->right;
        n->right = new_root->left;
        new_root->left = n;

        update_height(n);
        update_height(new_root);

        return new_root;
    }

    static Node* right_rotate(Node *n) {
        Node *new_root = n->left;
        n->left = new_root->right;
        new_root->right = n;

        update_height(n);
        update_height(new_root);

        return new_root;

    }

    static Node* balance(Node *n) {
        switch (height(n->right) - height(n->left)) {
            case -2:
                // check if double rotation is needed
                if (height(n->left->left) < height(n->left->right)) n->left = left_rotate(n->left);
                n = right_rotate(n);
            break;
            case 2:
                if (height(n->right->left) > height(n->right->right)) n->right = right_rotate(n->right);
                n = left_rotate(n);
            break;
        }

        return n;
    }

    static Node* insert(Node *n, key_t k) {
        if (n == nullptr) {
            return new Node(k);
        } else if (k < n->key) {
            n->left = insert(n->left, k);
        } else if (k > n->key) {
            n->right = insert(n->right, k);
        }
        update_height(n);

        return balance(n);
    }

    static void clear(Node *n) {
        if (n == nullptr) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }

    static key_t find_max(Node *n) {
        if (n->right == nullptr) return n->key;
        return find_max(n->right);
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() {
        clear(root);
    }

    bool empty() const {
        return root == nullptr;
    }

    void insert(key_t k) {
        root = insert(root, k);
    }

    key_t find_max() const {
        return !empty() ? find_max(root) : KEY_NEGATIVE_INF;
    }

    friend ostream& operator<<(ostream &os, const AVLTree &avl) {
        if (avl.empty()) return os;

        // find x coordinates
        struct FillInorderMap {
            static void fill(Node *n, map<Node*, n_keys_t> &im, n_keys_t &counter) {
                if (n == nullptr) return;
                fill(n->left, im, counter);
                im[n] = counter;
                counter++;
                fill(n->right, im, counter);
            }
        };

        map<Node*, n_keys_t> im;
        n_keys_t inorder_counter = 0;
        FillInorderMap::fill(avl.root, im, inorder_counter);

        queue<Node*> q, next_q;
        if (avl.root != nullptr) q.push(avl.root);
        n_keys_t x_curr = 0;
        int setw_val = int(to_string(avl.find_max()).length());

        while (!q.empty()) {
            Node *n = q.front();
            q.pop();

            while (x_curr < im[n]) {
                os << setw(setw_val) << setfill(' ') << ' ';
                x_curr++;
            }
            os << setw(setw_val) << setfill(' ') << n->key;
            x_curr++;

            if (n->left != nullptr) next_q.push(n->left);
            if (n->right != nullptr) next_q.push(n->right);

            if (q.empty()) {
                q = next_q;
                next_q = queue<Node*>();
                if (!q.empty()) os << '\n';
                x_curr = 0;
            }
        }

        return os;
    }
};

int main() {
    AVLTree avl;
    for (int i = 1; i < 16; ++i) {
        avl.insert(i);
    }
    cout << avl << '\n';

    return 0;
}
