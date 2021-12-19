#include <iostream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <map>
#include <limits>

using namespace std;

const char CMD_INSERT = 'I';
const char CMD_DELETE = 'D';

class DualAVLTree {
private:
    typedef int key_t;

    static const key_t KEY_NEG_INF = numeric_limits<key_t>::min();
    static const key_t KEY_POS_INF = numeric_limits<key_t>::max();

    static const int EMPTY_NODE_HEIGHT = -1;
    static const int NODE_HEIGHT = 1 + EMPTY_NODE_HEIGHT;
    static const int MAX_INTERNAL_NODE_KEYS = 2;
    static const int MAX_LEAF_KEYS = 3;

    static const int DEF_RANGE_STEP = 1;

    struct Node {
        vector<key_t> keys;
        Node *left, *right;
        int height;

        explicit Node(key_t k) : height(NODE_HEIGHT), left(nullptr), right(nullptr) {
            keys.push_back(k);
        }

        bool is_leaf() const {
            return left == nullptr && right == nullptr;
        }

        size_t size() const {
            return keys.size();
        }

        key_t min() const {
            return keys[0];
        }

        key_t max() const {
            return keys[keys.size() - 1];
        }

        bool push(key_t k) {
            for (const auto &key: keys) {
                if (k == key) {
                    return false;
                }
            }
            keys.push_back(k);
            sort(keys.begin(), keys.end());
            return true;
        }

        key_t pop_min() {
            key_t mn = min();
            keys.erase(keys.begin());
            return mn;
        }

        key_t pop_max() {
            key_t mx = max();
            keys.pop_back();
            return mx;
        }

        bool erase(key_t k) {
            for (auto it = keys.begin(); it != keys.end(); ++it) {
                if (*it == k) {
                    keys.erase(it);
                    return true;
                }
            }
            return false;
        }
    };

    Node *root;

    static int height(Node *n) {
        return n == nullptr ? EMPTY_NODE_HEIGHT : n->height;
    }

    static void update_height(Node *n) {
        n->height = max(height(n->left), height(n->right)) + 1;
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

    static Node* balance_nodes(Node *n) {
        update_height(n);

        switch (height(n->right) - height(n->left)) {
            case -2:
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

    static Node* balance_keys(Node *n) {
        if (n->left == nullptr && n->right != nullptr || n->left != nullptr && n->right == nullptr) {
            if (n->left != nullptr) {
                n->push(n->left->max());
                n->left = remove(n->left->max(), n->left);
                if (n->left != nullptr) {
                    n->right = insert(n->pop_max(), n->right);
                }
            } else {
                n->push(n->right->min());
                n->right = remove(n->right->min(), n->right);
                if (n->right != nullptr) {
                    n->left = insert(n->pop_min(), n->left);
                }
            }
        }

        return n;
    }

    static Node* find_min(Node *n) {
        if (n->left == nullptr) return n;
        return find_min(n->left);
    }

    static Node* find_max(Node *n) {
        if (n->right == nullptr) return n;
        return find_max(n->right);
    }

    static Node* insert(key_t k, Node *n) {
        if (n == nullptr) {
            return new Node(k);
        }
        else if (n->is_leaf()) {
            if (n->push(k) && n->size() > MAX_LEAF_KEYS) {
                n->left = insert(n->pop_min(), n->left);
                n->right = insert(n->pop_max(), n->right);
            }
        }
        else if (k < n->min()) {
            n->left = insert(k, n->left);
        }
        else if (k > n->max()) {
            n->right = insert(k, n->right);
        }
        else {
            if (n->push(k) && n->size() > MAX_INTERNAL_NODE_KEYS) {
                if (height(n->left) <= height(n->right)) {
                    n->left = insert(n->pop_min(), n->left);
                }
                else {
                    n->right = insert(n->pop_max(), n->right);
                }
            }
        }

        return balance_nodes(n);
    }

    static Node* remove(key_t k, Node *n) {
        if (n == nullptr) {
            return n;
        }

        if (n->is_leaf()) {
            if (n->erase(k) && n->size() == 0) {
                delete n;
                n = nullptr;
                return n;
            }
        }
        else if (k < n->min()) {
            n->left = remove(k, n->left);
        }
        else if (k > n->max()) {
            n->right = remove(k, n->right);
        }
        else {
            if (n->erase(k)) {
                if (height(n->left) <= height(n->right)) {
                    Node *r = find_min(n->right);
                    n->push(r->min());
                    n->right = remove(r->min(), n->right);
                } else {
                    Node *l = find_max(n->left);
                    n->push(l->max());
                    n->left = remove(l->max(), n->left);
                }
            }
        }

        return balance_nodes(balance_keys(n));
    }

    void get_statistics(vector<int> &stat, Node *n) const {
        if (n == nullptr) return;
        stat[0]++;
        if (n->is_leaf()) stat[n->size()]++;
        get_statistics(stat, n->left);
        get_statistics(stat, n->right);
    }

    static void clear(Node *n) {
        if (n == nullptr) return;
        clear(n->left);
        clear(n->right);
        delete n;
    }

public:
    DualAVLTree() : root(nullptr) {}

    bool empty() const {
        return root == nullptr;
    }

    void insert(key_t k) {
        root = insert(k, root);
    }

    void insert(key_t start, key_t end, key_t step=DEF_RANGE_STEP) {
        for (int k = start; k <= end; k += step) {
            insert(k);
        }
    }

    void remove(key_t k) {
        root = remove(k, root);
    }

    void remove(key_t start, key_t end, key_t step=DEF_RANGE_STEP) {
        for (int k = start; k <= end; k += step) {
            remove(k);
        }
    }

    key_t find_min() const {
        return empty() ? KEY_POS_INF : find_min(root)->min();
    }

    key_t find_max() const {
        return empty() ? KEY_NEG_INF : find_max(root)->max();
    }

    vector<int> get_statistics() const {
        vector<int> stat(1 + MAX_LEAF_KEYS, 0);
        get_statistics(stat, root);
        return stat;
    }

    void clear() {
        clear(root);
        root = nullptr;
    }

    ~DualAVLTree() {
        clear();
    }

    friend ostream& operator<<(ostream &os, const DualAVLTree &davl);
};

ostream& operator<<(ostream &os, const DualAVLTree &davl) {
    if (!davl.empty()) {
        map<DualAVLTree::Node*, int> im;
        int counter = 0;

        // inorder traversal, map each node to it's x coordinate
        auto fill_map = [&](auto &self, DualAVLTree::Node *n) {
            if (n == nullptr) return;
            self(self, n->left);
            im[n] = counter;
            counter += int(n->size());
            self(self, n->right);
        };
        fill_map(fill_map, davl.root);

        queue<DualAVLTree::Node*> q, next_q;
        q.push(davl.root);
        int setw_val = int(to_string(davl.find_max()).length());
        int x = 0;

        while (!q.empty()) {
            DualAVLTree::Node *n = q.front();
            q.pop();

            for (; x < im[n]; ++x) {
                os << setw(setw_val + 1) << setfill(' ') << ' ';
            }

            for (int i = 0; i < n->size(); ++i, ++x) {
                os << setw(setw_val) << setfill(' ') << n->keys[i] << (i < n->size() - 1 ? ',' : ' ');
            }

            if (n->left != nullptr) next_q.push(n->left);
            if (n->right != nullptr) next_q.push(n->right);

            if (q.empty()) {
                q = next_q;
                next_q = queue<DualAVLTree::Node*>();
                if (!q.empty()) os << '\n';
                x = 0;
            }
        }
    }

    return os;
}

int main() {
    DualAVLTree davl;
    int n_commands;
    cin >> n_commands;

    for (int i = 0; i < n_commands; ++i) {
        char cmd;
        int start, step, end;
        cin >> cmd >> start >> step >> end;

        switch (cmd) {
            case CMD_INSERT:
                davl.insert(start, end, step);
            break;
            case CMD_DELETE:
                davl.remove(start, end, step);
            break;
            default:
                cerr << "Unknown command" << '\n';
            break;
        }
    }

    auto stat = davl.get_statistics();
    for (int i = 0; i < stat.size(); ++i) {
        cout << stat[i] << (i < stat.size() - 1 ? " " : "\n");
    }

    return 0;
}
