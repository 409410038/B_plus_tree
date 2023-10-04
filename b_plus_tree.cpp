using namespace std;
#include <iostream>
#include <cstring>
#include <vector>

class node {
public:
    node(int capacity, bool is_index): capacity(capacity), left(nullptr), right(nullptr), parent(nullptr), is_index(is_index) {}
    int capacity;
    int check_capacity();
    void copy_up(int, int);
    int delete_val(int num) {
        int indx = -1;
        for (int i = 0; i < val.size(); i++) {
            if (val[i] == num)
                indx = i;
            if (indx >= 0 && indx < i)
                val[i-1] = val[i];
        }
        if (indx >= 0)  val.pop_back();
        return indx;
    }
    int insert_val(int num) {
        vector<int>::iterator it = val.begin();
        for (int i = 0; i < val.size(); i++) {
            if (val[i] > num) {
                val.insert(it+i, num);
                return i;
            }
            else if (val[i] == num)
                return -1;
        }
        val.push_back(num);
        return val.size()-1;
    }
    void print_node(int);
    void add_left_neighbor(node* l) {
        left = l;
        return;
    }
    void delete_left_neighbor() {
        left = nullptr;
        return;
    }
    void delete_right_neighbor() {
        right = nullptr;
        return;
    }
    void add_right_neighbor(node* r) {
        right = r;
        return;
    }
    void delete_node();
    bool insert_re_distribution(int);
    bool delete_re_distribution();
    bool check_repeated(int);
    friend node* handdle_overflow(node*);
    friend node* handdle_underflow(node*);
    friend node* find_leaf_pos(node*, int);
    friend void print_val_list(node*, string, int);

private:
    bool is_index;  //Whether it is a index node or data node
    node* left;
    node* right;
    vector<int> val;  // val[i-1] <= children[i]->val < val[i]
    vector<node*> children;
    node* parent;

};
int node::check_capacity() {
    return capacity-val.size();
}

void node::delete_node() {
    for (int i = 0; i < children.size(); i++) {
        children[i]->delete_node();
    }
    delete this;
    return;
}

void node::print_node(int depth) {

    for (int i = 0; i < depth; i++)
        cout << "     ";

    if (is_index == true) {

        cout << '(';

        for (int i = 0; i < val.size(); i++) {
            cout << val[i];
            if (i != capacity-1)
                cout << ":";
        }
        for (int i = val.size(); i < capacity; i++) {
            cout << "_";
            if (i != capacity-1)
                cout << ":";
        }

        cout << ')' << endl;

        for (int i = 0; i < children.size(); i++) {
            children[i]->print_node(depth+1);
        }
    }

    else {
        cout << '[';

        for (int i = 0; i < val.size(); i++) {
            cout << val[i];
            if (i != capacity-1)
                cout << ",";
        }
        for (int i = val.size(); i < capacity; i++) {
            cout << "_";
            if (i != capacity-1)
                cout << ",";
        }

        cout << ']' << endl;
    }
    return;
}

bool node::check_repeated(int num) {
    for (int i = 0; i < val.size(); i++) {
        if (val[i] == num)
            return true;
    }
    return false;
}

node* find_leaf_pos(node* root, int num) {
    node* cur = root;
    while(cur->is_index) {
        for (int i = 0; i < cur->children.size(); i++) {
            if (i == 0) {
                if (num < cur->val[i])  cur = cur->children[i];
            }
            else if (i == cur->children.size()-1) {
                if (num >= cur->val[i-1])  cur = cur->children[i];
            }
            else {
                if (num >= cur->val[i-1] && num < cur->val[i])  cur = cur->children[i];
            }
        }
    }

    return cur;
}

void node::copy_up(int replaced_num, int copy_num) {
    // just replace
    // for redistribution
    for (int i = 0; i < val.size(); i++) {
        if (val[i] == replaced_num) {
            val[i] = copy_num;
            return;
        }
    }

    cout << "Error: copy up failed: " << replaced_num << ";" << copy_num << endl;
    exit(0);
    return;
}

bool node::insert_re_distribution(int num) {
    //return false; // debug

    if (left != nullptr) {

        int target = val.front();
        if (left->check_capacity() > 0) {
            left->insert_val(target);
            val.erase(val.begin());

            // copy up
            int copy_num = val.front();
            node* left_parent = left->parent;
            node* cur_parent = parent;
            while(left_parent != cur_parent) {
                left_parent = left_parent->parent;
                cur_parent = cur_parent->parent;
                if (cur_parent == nullptr || left_parent == nullptr) {
                    cout << "Error: please check the code. " << endl;
                }
            }
            int replaced_num;
            // left branch end with large value
            for (int i = left_parent->val.size()-1; i >= 0; i--) {
                if (left_parent->val[i] < copy_num) {
                    replaced_num = left_parent->val[i];
                    break;
                }
            }
            left_parent->copy_up(replaced_num, copy_num);
            return true;
        }
    }
    if (right != nullptr) {

        int target = val.back();
        if (right->check_capacity() > 0) {
            right->insert_val(target);
            val.pop_back();

            //copy up
            int copy_num = right->val.front();
            node* right_parent = right->parent;
            node* cur_parent = parent;
            while (right_parent != cur_parent) {
                right_parent = right_parent->parent;
                cur_parent = cur_parent->parent;
                if (cur_parent == nullptr || right_parent == nullptr) {
                    cout << "Error: please check the code. " << endl;
                    exit(0);
                }
            }
            int replaced_num;
            // right branch starts with lower value
            for (int i = 0; i < right_parent->val.size(); i++) {
                if (right_parent->val[i] > copy_num) {
                    replaced_num = right_parent->val[i];
                    break;
                }
            }
            right_parent->copy_up(replaced_num, copy_num);
            return true;
        }
    }
    // if failed
    return false;
}

bool node::delete_re_distribution() {
    int target, replaced_num, copy_num;

    if (left != nullptr) {
        if (left->check_capacity()+1 <= capacity/2) {

            //  find nearest common parent
            node* left_parent = left->parent;
            node* cur_parent = parent;
            while(left_parent != cur_parent) {
                left_parent = left_parent->parent;
                cur_parent = cur_parent->parent;
                if (cur_parent == nullptr || left_parent == nullptr) {
                    cout << "Error: please check the code. " << endl;
                    exit(0);
                }
            }

            copy_num = left->val.back();


            for (int i = 0; i < left_parent->val.size(); i++) {
                if (left_parent->val[i] > copy_num) {
                    replaced_num = left_parent->val[i];
                    break;
                }
            }
            if (is_index) {
                // need to adjust children
                target = replaced_num;

                node* child = left->children.back();
                left->children.pop_back();

                this->children.insert(children.begin(), child);
                child->parent = this;

            }
            else {
                target = copy_num;
            }

            this->insert_val(target);
            left->val.pop_back();

            left_parent->copy_up(replaced_num, copy_num);
            return true;
        }

    }
    if (right != nullptr) {

        if (right->check_capacity()+1 <= capacity/2) {

            node* right_parent = right->parent;
            node* cur_parent = parent;
            while (right_parent != cur_parent) {
                right_parent = right_parent->parent;
                cur_parent = cur_parent->parent;
                if (cur_parent == nullptr || right_parent == nullptr) {
                    cout << "Error: please check the code. " << endl;
                    exit(0);
                }
            }

            copy_num = right->val.front();

            for (int i = right_parent->val.size()-1; i >= 0; i--) {
                if (right_parent->val[i] <= copy_num) { //
                    replaced_num = right_parent->val[i];
                    break;
                }
            }

            if (is_index) {
                target = replaced_num;

                node* child = right->children.front();
                right->children.erase(right->children.begin());

                this->children.push_back(child);
                child->parent = this;
            }
            else {
                target = right->val.front();
                copy_num = right->val[1];
            }

            this->insert_val(target);
            right->val.erase(right->val.begin());

            right_parent->copy_up(replaced_num, copy_num);
            return true;
        }
    }
    // if failed
    return false;
}

node* handdle_overflow(node* cur) {
    if (cur->check_capacity() >= 0) {
        if (cur->parent == nullptr)
            return cur;  //no parent node means root

        return handdle_overflow(cur->parent);
    }

    int target = -1;
    node* new_node;
    target = cur->val[cur->capacity/2];  // the value to push up or copy up
    if (cur->is_index) {

        new_node = new node(cur->capacity, true);
        // does not remain middle value
        for (int i = cur->capacity/2+1; i < cur->val.size(); i++) {
            new_node->val.push_back(cur->val[i]);
        }
        for (int i = cur->children.size()/2; i < cur->children.size(); i++) {
            new_node->children.push_back(cur->children[i]);
            cur->children[i]->parent = new_node;
        }
        for (int i = 0; i < new_node->val.size()+1; i++) {
            cur->val.pop_back();
        }
        for (int i = 0; i < new_node->children.size(); i++) {
            cur->children.pop_back();
        }
    }
    else {
        new_node = new node(cur->capacity, false);
        // add new val to parent
        // remain middle value
        for (int i = cur->capacity/2; i < cur->val.size(); i++) {
            new_node->val.push_back(cur->val[i]);
        }
        for (int i = 0; i < new_node->val.size(); i++) {
            cur->val.pop_back();
        }
    }
    if (cur->right != nullptr) {
        cur->right->add_left_neighbor(new_node);
        new_node->add_right_neighbor(cur->right);
    }

    cur->add_right_neighbor(new_node);
    new_node->add_left_neighbor(cur);

    if (cur->parent == nullptr) {
        // need a new root
        node* new_parent = new node(cur->capacity, true);
        new_parent->children.push_back(cur);
        cur->parent = new_parent;
    }
    node* parent = cur->parent;
    int pos = parent->insert_val(target);  // add new value to be parent's val
    if (pos == -1) {
        cout << "Error: insert repeat value" << endl;
        exit(0);
        return cur;
    }
    new_node->parent = parent;
    parent->children.insert(parent->children.begin()+pos+1, new_node); // add new node to be parent's child

    return handdle_overflow(parent);
}

node* handdle_underflow(node* cur) {
    // recursively merge

    if (cur->check_capacity() <= cur->capacity/2 || cur->parent == nullptr) {
        if (cur->parent == nullptr) {
            if (cur->val.empty() && cur->is_index) {
                node* child = cur->children.front();
                cur->children.pop_back();
                child->parent = nullptr;
                cur->delete_node();
                return child;
            }
            else  return cur;  //no parent node means root
        }
        return handdle_underflow(cur->parent);
    }

    if (cur->delete_re_distribution() == true)
        return handdle_underflow(cur->parent);

    node* left = cur->left;
    node* right = cur->right;
    node* parent = cur->parent;

    if (left != nullptr && left->parent == parent) {
        // merge with left sibling

        for (int i = 0; i < parent->val.size(); i++) {
            if (parent->val[i] > left->val.back()) {
                if (cur->is_index) {
                    // have to pull down
                    left->val.push_back(parent->val[i]);
                }
                parent->val.erase(parent->val.begin()+i);
                parent->children.erase(parent->children.begin()+i+1);
                break;
            }
        }

        for (int i = 0; i < cur->val.size(); i++) {
            left->val.push_back(cur->val[i]);
        }
        for (int i = 0; i < cur->children.size(); i++) {
            left->children.push_back(cur->children[i]);
            cur->children[i]->parent = left;
        }
        cur->children.clear();

        left->add_right_neighbor(right);
        if (right != nullptr) {
            right->add_left_neighbor(left);
        }

        cur->delete_node();

        cur = left;
    }

    else if (right != nullptr && right->parent == parent) {
        // merge with right sibling
        for (int i = parent->val.size()-1; i >= 0; i--) {

            if (parent->val[i] <= right->val.front()) {

                if (cur->is_index) {
                    cur->val.push_back(parent->val[i]);
                }
                parent->val.erase(parent->val.begin()+i);
                parent->children.erase(parent->children.begin()+i+1);
                break;
            }
        }

        for (int i = 0; i < right->val.size(); i++) {
            cur->val.push_back(right->val[i]);
        }
        for (int i = 0; i < right->children.size(); i++) {
            cur->children.push_back(right->children[i]);
            right->children[i]->parent = cur;
        }
        right->children.clear();

        cur->add_right_neighbor(right->right);
        if (right->right != nullptr) {
            right->right->add_left_neighbor(cur);
        }
        right->delete_node();
    }
    else {
        cout << "Error: this node has parent but doesn't have sibling. " << endl;
        exit(0);
    }

    if (cur->parent == nullptr)
        return cur;

    return handdle_underflow(cur->parent);
}

node* Insert(node* root, vector<int>& nums) {

    node* cur;

    for (int i = 0; i < nums.size(); i++) {

        //find a leaf place
        cur = find_leaf_pos(root, nums[i]);

        if (cur->check_repeated(nums[i])) {
            cout << "ignore the repeated value: " << nums[i] << endl;
            continue;
        }

        cout << endl << "Insert key: " << nums[i] << endl;

        if (cur->check_capacity() > 0) {
            cur->insert_val(nums[i]);
        }
        else if (cur->check_capacity() < 0) {
            cout << "error: ";
            cur->print_node(0);
            exit(0);
        }
        else {
            // try to do redistribution

            cur->insert_val(nums[i]);  //make it overflow
            if (cur->insert_re_distribution(nums[i]) == false) {
                root = handdle_overflow(cur);
            }

        }
        root->print_node(0);

    }

    return root;
}

node* Delete(node* root, vector<int>& nums) {

    node* cur;

    for (int i = 0; i < nums.size(); i++) {

        //find a leaf place
        cur = find_leaf_pos(root, nums[i]);

        if (cur->check_repeated(nums[i]) == false) {
            cout << "ignore the non-existed value: " << nums[i] << endl;
            continue;
        }

        cout << endl << "Delete key: " << nums[i] << endl;

        if (cur->check_capacity()+1 <= cur->capacity/2) { // can delete key directly
            cur->delete_val(nums[i]);
        }
        else if (cur->check_capacity() < 0) {
            cout << "error: ";
            cur->print_node(0);
            exit(0);
        }
        else {
            // try to do redistribution

            cur->delete_val(nums[i]);  // make it underflow

            root = handdle_underflow(cur);
        }
        root->print_node(0);

    }

    return root;
}

bool IsNumber(string s) {
    for (int i = 0; i < s.size(); i++) {
        if (i == 0) {
            if (!isdigit(s[i]) && s[i] != '-')
                return false;
        }
        else if (!isdigit(s[i]))
            return false;
    }

    return true;
}
void print_exception(string input) {
    cout << endl << "Something unexpected: " << input << endl;
    cout << "Please check your command. " << endl;
    cin.sync();
    return;
}

void print_val_list(node* root, string op, int target) {
    node* cur = find_leaf_pos(root, target);
    cout << endl;
    while (cur != nullptr) {
        for (int i = 0; i < cur->val.size(); i++) {
            if (op == ">=") {
               if (cur->val[i] >= target)  cout << cur->val[i] << " ";
            }
            else if (op == ">") {
               if (cur->val[i] > target)  cout << cur->val[i] << " ";
            }
            else if (op == "<=") {
               if (cur->val[i] <= target)  cout << cur->val[i] << " ";
            }
            else if (op == "<") {
               if (cur->val[i] < target)  cout << cur->val[i] << " ";
            }
        }
        if (op == ">=" || op == ">") {
            cur = cur->right;
        }
        else if (op == "<=" || op == "<") {
            cur = cur->left;
        }
        else  break;
    }
    cout << endl;
}
void print_manual() {
    cout << endl << "Command Usage: " << endl;
    cout << '\t' << "'Insert {positive integers}': Insert a list of positive integer. " << endl;
    cout << '\t' << "'Delete {positive integers}': Delete a list of positive integer. " << endl;
    cout << '\t' << "'Find {>=, >, <, <=} {positive integer}': Find a list of data in the specific range. " << endl;
    cout << '\t' << "'Restart': Construct a new B+tree and delete old one. " << endl;
    cout << '\t' << "'Quit': Leave the program. " << endl;

    return;
}
int main() {

    string input_command;
    int capacity = 0;
    node* tree_root = nullptr;
    bool set_order = false;
    cout << "If you want to exit the program, just type 'Quit'." << endl;
    cout << "Input the order of B+ tree: ";
    cin >> input_command;

    while(1) {

        if (input_command == "Quit" || input_command == "quit") {
            cout << "Bye" << endl;
            return 0;
        }
        else if (input_command == "Insert" || input_command == "insert"){
            vector<int> nums;
            string num;

            cin >> num;
            while (IsNumber(num)) {
                nums.push_back(stoi(num));
                if (cin.get() == '\n')
                    break;
                cin >> num;
            }
            if (IsNumber(num) == false || nums.empty())  print_exception(num);

            else  tree_root = Insert(tree_root, nums);
        }
        else if (input_command == "Delete" || input_command == "delete"){
            vector<int> nums;
            string num;

            cin >> num;
            while (IsNumber(num)) {
                nums.push_back(stoi(num));
                if (cin.get() == '\n')
                    break;
                cin >> num;
            }
            if (IsNumber(num) == false || nums.empty())  print_exception(num);

            else  tree_root = Delete(tree_root, nums);
        }
        else if (input_command == "Restart" || input_command == "restart") {
            if (tree_root != nullptr)
                tree_root->delete_node();
            tree_root = nullptr;
            cout << "Input the order of B+ tree: ";
            set_order = false;
            cin >> input_command;
            continue;
        }
        else if (input_command == "Find" || input_command == "find") {
            cin >> input_command;

            string target;
            if (input_command == ">=" || input_command == ">" || input_command == "<=" || input_command == "<") {
                cin >> target;

                if (IsNumber(target) == false)  print_exception(target);

                else {
                    print_val_list(tree_root, input_command, stoi(target));
                }
            }
            else {
                print_exception(input_command);
            }
        }
        else if (IsNumber(input_command)) {
            if (set_order == false && stoi(input_command) > 0) {
                capacity = 2*stoi(input_command);
                set_order = true;
                tree_root = new node(capacity, false);
                print_manual();
            }
            else {
                if (stoi(input_command) <= 0)
                    cout << "Order must be positive integer. " << endl;
                print_exception(input_command);
            }
        }
        else {
            print_exception(input_command);
        }
        cout << endl << "-----------------------------------------------------------------------" << endl;
        cin >> input_command;

    }

    return 0;

}
