#include <iostream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <map>
#include <limits>

using namespace std;

const char CMD_INSERT = 'I';
const char CMD_DELETE = 'D';

template <typename key_t>
class DualAVLTree {
private:
    static const key_t KEY_NEG_INF = numeric_limits<key_t>::min();
    static const key_t KEY_POS_INF = numeric_limits<key_t>::max();

    static const int EMPTY_NODE_HEIGHT = -1;
    static const int NODE_HEIGHT = 1 + EMPTY_NODE_HEIGHT;
    static const int MAX_INTERNAL_NODE_KEYS = 2;
    static const int MAX_LEAF_KEYS = 3;

    static const int DEF_RANGE_STEP = 1;

    struct Node {
        vector<key_t> keys;
        int height;
        Node *left, *right;

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
                if (k == key) return false;
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

    int height(Node *n) {
        return n == nullptr ? EMPTY_NODE_HEIGHT : n->height;
    }

    void update_height(Node *n) {
        n->height = max(height(n->left), height(n->right)) + 1;
    };

    Node* left_node_rotate(Node *n) {
        Node *new_root = n->right;
        n->right = new_root->left;
        new_root->left = n;

        update_height(n);
        update_height(new_root);

        return new_root;
    }

    Node* right_node_rotate(Node *n) {
        Node *new_root = n->left;
        n->left = new_root->right;
        new_root->right = n;

        update_height(n);
        update_height(new_root);

        return new_root;
    }

    Node* left_key_rotate(Node *n) {
        // add the minimal key from the right subtree
        Node *r = find_min(n->right);
        n->push(r->min());
        n->right = remove(r->min(), n->right);

        // remove the minimum key and add it to the left subtree
        if (!n->is_leaf()) n->left = insert(n->pop_min(), n->left);
        update_height(n);

        return n;
    }

    Node* right_key_rotate(Node *n) {
        Node *l = find_max(n->left);
        n->push(l->max());
        n->left = remove(l->max(), n->left);

        if (!n->is_leaf()) n->right = insert(n->pop_max(), n->right);
        update_height(n);

        return n;
    }

    Node* balance(Node *n) {
        update_height(n);

        if (!n->is_leaf()) {
            if (n->left == nullptr) {
                n = left_key_rotate(n);
            }
            else if (n->right == nullptr) {
                n = right_key_rotate(n);
            }
        }

        switch (height(n->right) - height(n->left)) {
            case -2:
                if (height(n->left->left) < height(n->left->right)) n->left = left_node_rotate(n->left);
                n = right_node_rotate(n);
            break;
            case 2:
                if (height(n->right->left) > height(n->right->right)) n->right = right_node_rotate(n->right);
                n = left_node_rotate(n);
            break;
        }

        return n;
    }

    Node* find_min(Node *n) {
        if (n->left == nullptr) return n;
        return find_min(n->left);
    }

    Node* find_max(Node *n) {
        if (n->right == nullptr) return n;
        return find_max(n->right);
    }

    Node* insert(key_t k, Node *n) {
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

        return balance(n);
    }

    Node* remove(key_t k, Node *n) {
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

        return balance(n);
    }

    void get_statistics(vector<int> &stat, const Node *n) const {
        if (n == nullptr) return;
        stat[0]++;
        if (n->is_leaf()) stat[n->size()]++;
        get_statistics(stat, n->left);
        get_statistics(stat, n->right);
    }

    void clear(Node *n) {
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

    friend ostream& operator<<(ostream &os, const DualAVLTree<key_t> &davl) {
        if (!davl.empty()) {
            typedef DualAVLTree<key_t>::Node node_t;
            map<node_t*, int> im;
            int counter = 0;

            // inorder traversal, map each node to it's x coordinate
            auto fill_map = [&](auto &self, node_t *n) {
                if (n == nullptr) return;
                self(self, n->left);
                im[n] = counter;
                counter += int(n->size());
                self(self, n->right);
            };
            fill_map(fill_map, davl.root);

            queue<node_t*> q, next_q;
            q.push(davl.root);
            int setw_val = int(to_string(davl.find_max()).length());
            int x = 0;

            while (!q.empty()) {
                node_t *n = q.front();
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
                    next_q = queue<node_t*>();
                    if (!q.empty()) os << '\n';
                    x = 0;
                }
            }
        }

        return os;
    }
};

int main() {
    DualAVLTree<int> davl;
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
        for (int i = 0; i < int(stat.size()); ++i) {
        cout << stat[i] << (i < int(stat.size()) - 1 ? " " : "\n");
    }

    return 0;
}
