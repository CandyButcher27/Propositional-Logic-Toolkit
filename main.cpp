#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <set>
#include <cctype>
#include<chrono>

// Using standard namespace for convenience
using namespace std;
using namespace std::chrono;

// --- Node Structure and Core Tree Utilities ---

// Standardized Node structure for the parse tree
struct Node {
    char data;
    Node* left;
    Node* right;

    Node(char val)
        : data(val), left(nullptr), right(nullptr) {}
};

// Helper function to check if a character is an operator
bool isOperator(char c) {
    return c == '+' || c == '*' || c == '>' || c == '~';
}

// Recursively deletes the tree to prevent memory leaks
void deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Recursively copies a tree to prevent memory errors when duplicating sub-expressions
Node* copyTree(Node* root) {
    if (!root) return nullptr;
    Node* newNode = new Node(root->data);
    newNode->left = copyTree(root->left);
    newNode->right = copyTree(root->right);
    return newNode;
}


// --- Section 1: Infix, Prefix, and Parse Tree Core Functions ---

string infixToPrefix(string infix);
Node* buildParseTree(const string& prefix);
void printTree(const Node* root);
int getTreeHeight(const Node* node);
string parseTreeToInfix(Node* root);

// Returns the precedence of an operator
int getPrecedence(char op) {
    switch (op) {
        case '>': return 1;
        case '+': return 2;
        case '*': return 3;
        case '~': return 4;
        default:  return 0;
    }
}

// Finds the main operator in a sub-expression
int findMainOperator(const string& infix) {
    int paren_level = 0;
    int main_op_pos = -1;
    int main_op_precedence = 99;

    for (int i = 0; i < infix.length(); ++i) {
        char c = infix[i];
        if (c == '(') paren_level++;
        else if (c == ')') paren_level--;

        int current_precedence = getPrecedence(c);
        if (current_precedence > 0 && paren_level == 0) {
            if (current_precedence <= main_op_precedence) {
                main_op_precedence = current_precedence;
                main_op_pos = i;
            }
        }
    }
    return main_op_pos;
}

// Checks if an expression is fully enclosed in parentheses
bool isFullyEnclosed(const string& infix) {
    if (infix.length() < 2 || infix.front() != '(' || infix.back() != ')') {
        return false;
    }
    int paren_level = 0;
    for (size_t i = 0; i < infix.length(); ++i) {
        if (infix[i] == '(') paren_level++;
        else if (infix[i] == ')') paren_level--;
        if (paren_level == 0 && i < infix.length() - 1) return false;
    }
    return (paren_level == 0);
}

// Main function to convert an infix expression to prefix
string infixToPrefix(string infix) {
    infix.erase(remove(infix.begin(), infix.end(), ' '), infix.end());
    if (infix.length() <= 1) return infix;

    if (isFullyEnclosed(infix)) {
        return infixToPrefix(infix.substr(1, infix.length() - 2));
    }
    int operator_pos = findMainOperator(infix);
    if (operator_pos != -1) {
        char op = infix[operator_pos];
        if (op == '~' && operator_pos == 0) {
            return op + infixToPrefix(infix.substr(1));
        } else {
            string left = infix.substr(0, operator_pos);
            string right = infix.substr(operator_pos + 1);
            return op + infixToPrefix(left) + infixToPrefix(right);
        }
    }
    return infix;
}

// Recursive helper to build the parse tree
Node* buildParseTreeRecursive(const string& prefix, int& index) {
    if (index >= prefix.length()) return nullptr;
    char currentChar = prefix[index++];
    Node* node = new Node(currentChar);

    if (isOperator(currentChar)) {
        if (currentChar == '~') {
            node->left = nullptr;
            node->right = buildParseTreeRecursive(prefix, index);
        } else {
            node->left = buildParseTreeRecursive(prefix, index);
            node->right = buildParseTreeRecursive(prefix, index);
        }
    }
    return node;
}

// Creates the parse tree from a prefix expression
Node* buildParseTree(const string& prefix) {
    if (prefix.empty()) return nullptr;
    int index = 0;
    return buildParseTreeRecursive(prefix, index);
}

// Calculates the height of the tree
int getTreeHeight(const Node* node) {
    if (node == nullptr) return 0;
    return 1 + max(getTreeHeight(node->left), getTreeHeight(node->right));
}

// Converts a parse tree back to a fully parenthesized infix string
string parseTreeToInfix(Node* root) {
    if (!root) return "";
    if (!isOperator(root->data)) return string(1, root->data);
    string result = "";
    if (root->data == '~') {
        result += "(~" + parseTreeToInfix(root->right) + ")";
    } else {
        result += "(" + parseTreeToInfix(root->left) + root->data + parseTreeToInfix(root->right) + ")";
    }
    return result;
}

// Calculates the width of a subtree for printing
int getSubtreeWidth(const Node* node) {
    if (node == nullptr) return 0;
    if (node->left == nullptr && node->right == nullptr) return 1;
    if (node->data == '~') return getSubtreeWidth(node->right);
    return getSubtreeWidth(node->left) + getSubtreeWidth(node->right) + 3;
}

// Recursively fills a 2D grid to represent the tree structure for printing
int fillGridAndGetCenter(const Node* node, vector<string>& grid, int row, int col_start) {
    if (node == nullptr) return -1;

    if (node->left == nullptr && node->right == nullptr) {
        if (row < grid.size() && col_start < grid[row].length()) {
            grid[row][col_start] = node->data;
        }
        return col_start;
    }

    int left_center = fillGridAndGetCenter(node->left, grid, row + 2, col_start);
    int right_start_col = (node->left != nullptr) ? col_start + getSubtreeWidth(node->left) + 3 : col_start;
    int right_center = fillGridAndGetCenter(node->right, grid, row + 2, right_start_col);

    int node_center = (node->data == '~') ? right_center : (left_center + right_center) / 2;
    if (row < grid.size() && node_center >= 0 && node_center < grid[row].length()) {
        grid[row][node_center] = node->data;
    }

    if (row + 1 < grid.size()) {
        if (node->data == '~') {
            if (right_center != -1 && node_center >= 0 && node_center < grid[row + 1].length()) {
                grid[row + 1][node_center] = '|';
            }
        } else {
            if (left_center != -1) {
                int branch_pos = (left_center + node_center) / 2;
                if (branch_pos >= 0 && branch_pos < grid[row + 1].length()) grid[row + 1][branch_pos] = '/';
            }
            if (right_center != -1) {
                int branch_pos = (right_center + node_center) / 2 + 1;
                if (branch_pos >= 0 && branch_pos < grid[row + 1].length()) grid[row + 1][branch_pos] = '\\';
            }
        }
    }
    return node_center;
}

// Main function to print the visual tree
void printTree(const Node* root) {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }
    int height = getTreeHeight(root);
    int width = getSubtreeWidth(root);
    if (width == 0) return;

    vector<string> grid(height * 2 - 1, string(width, ' '));
    fillGridAndGetCenter(root, grid, 0, 0);

    for (const auto& row_str : grid) {
        size_t last = row_str.find_last_not_of(' ');
        if (string::npos != last) {
            cout << row_str.substr(0, last + 1) << endl;
        }
    }
}

// --- Section 2: Truth Value Evaluation (Task 5) ---

// Traverses the tree to find all unique variables (operands)
void getVariables(Node* root, set<char>& vars) {
    if (!root) return;
    if (isalpha(root->data)) {
        vars.insert(root->data);
    }
    getVariables(root->left, vars);
    getVariables(root->right, vars);
}

// Evaluates the truth value of the formula represented by the tree
bool evaluate(Node* root, const unordered_map<char, bool>& values) {
    if (!root) {
        cerr << "Error: Evaluation attempted on a null tree node." << endl;
        return false;
    }
    if (!isOperator(root->data)) {
        auto it = values.find(root->data);
        if (it == values.end()) {
            cerr << "Error: No truth value for variable '" << root->data << "'\n";
            return false;
        }
        return it->second;
    }
    if (root->data == '~') {
        return !evaluate(root->right, values);
    }
    bool leftVal = evaluate(root->left, values);
    bool rightVal = evaluate(root->right, values);

    switch (root->data) {
        case '+': return leftVal || rightVal;
        case '*': return leftVal && rightVal;
        case '>': return (!leftVal) || rightVal;
        default:  return false;
    }
}

void printInfixFromTree(Node* root) {
    if (!root) return;
    if (!root->left && !root->right) {
        cout << root->data;
        return;
    }
    if (root->data == '~') {
        cout << "~";
        if (root->right && isOperator(root->right->data)) {
            cout << "("; printInfixFromTree(root->right); cout << ")";
        } else {
            printInfixFromTree(root->right);
        }
        return;
    }
    cout << "(";
    printInfixFromTree(root->left);
    cout << " " << root->data << " ";
    printInfixFromTree(root->right);
    cout << ")";
}

// Step 1: Replace implication (P > Q) with (~P + Q)
Node* impl_free(Node* root) {
    if (!root) return nullptr;
    root->left = impl_free(root->left);
    root->right = impl_free(root->right);

    if (root->data == '>') {
        Node* notP = new Node('~');
        notP->right = root->left;
        Node* orNode = new Node('+');
        orNode->left = notP;
        orNode->right = root->right;
        root->left = root->right = nullptr;
        delete root;
        return orNode;
    }
    return root;
}

// Step 2: Convert to Negation Normal Form (NNF)
Node* nnf(Node* root) {
    if (!root) return nullptr;
    if (root->data == '~') {
        Node* sub = root->right;
        if (sub->data == '~') {
            Node* newRoot = nnf(sub->right);
            sub->right = nullptr;
            delete root; delete sub;
            return newRoot;
        }
        if (sub->data == '+') { // De Morgan's Law: ~(A + B) => ~A * ~B
            Node* andNode = new Node('*');
            Node* notA = new Node('~'); notA->right = sub->left;
            Node* notB = new Node('~'); notB->right = sub->right;
            andNode->left = nnf(notA);
            andNode->right = nnf(notB);
            sub->left = sub->right = nullptr;
            delete root; delete sub;
            return andNode;
        }
        if (sub->data == '*') { // De Morgan's Law: ~(A * B) => ~A + ~B
            Node* orNode = new Node('+');
            Node* notA = new Node('~'); notA->right = sub->left;
            Node* notB = new Node('~'); notB->right = sub->right;
            orNode->left = nnf(notA);
            orNode->right = nnf(notB);
            sub->left = sub->right = nullptr;
            delete root; delete sub;
            return orNode;
        }
    }
    root->left = nnf(root->left);
    root->right = nnf(root->right);
    return root;
}

// ** CORRECTED FUNCTION **
// Step 3 Helper: Distribute OR over AND for CNF
Node* distribute(Node* a, Node* b) {
    if (a->data == '*') { // (X*Y)+Z => (X+Z)*(Y+Z)
        Node* left = distribute(a->left, copyTree(b));
        Node* right = distribute(a->right, b);
        Node* andNode = new Node('*');
        andNode->left = left;
        andNode->right = right;
        a->left = a->right = nullptr; // Decouple children before deleting
        delete a;
        return andNode;
    }
    if (b->data == '*') { // X+(Y*Z) => (X+Y)*(X+Z)
        Node* left = distribute(copyTree(a), b->left);
        Node* right = distribute(a, b->right);
        Node* andNode = new Node('*');
        andNode->left = left;
        andNode->right = right;
        b->left = b->right = nullptr; // Decouple children before deleting
        delete b;
        return andNode;
    }
    // Base Case: A + B
    Node* orNode = new Node('+');
    orNode->left = a;
    orNode->right = b;
    return orNode;
}

// Step 3: Convert NNF to CNF by distribution
Node* cnf(Node* root) {
    if (!root) return nullptr;
    root->left = cnf(root->left);
    root->right = cnf(root->right);

    if (root->data == '+') {
        Node* distributed_tree = distribute(root->left, root->right);
        root->left = root->right = nullptr;
        delete root;
        return distributed_tree;
    }
    return root;
}


// --- Section 4: CNF Validity Check (Task 7) ---

// Checks if a single clause is a tautology (e.g., contains P + ~P)
bool is_clause_true(string clause) {
    vector<string> literals;
    clause.erase(remove(clause.begin(), clause.end(), '('), clause.end());
    clause.erase(remove(clause.begin(), clause.end(), ')'), clause.end());
    clause.erase(remove(clause.begin(), clause.end(), ' '), clause.end());

    string token = "";
    for (char c : clause) {
        if (c == '+') {
            if (!token.empty()) literals.push_back(token);
            token = "";
        } else {
            token += c;
        }
    }
    if (!token.empty()) literals.push_back(token);

    bool clause_true = false;
    for (int i = 0; i < literals.size(); i++) {
        if (literals[i][0] == '~') {
            for (int j = 0; j < literals.size(); j++) {
                if (literals[j][0] == literals[i][1]) {
                    clause_true = true;
                    break;
                }
            }
        }
        if (clause_true) break;
    }

    return clause_true;
}

// Checks if an entire CNF formula is valid (a tautology)
bool check_cnf_valid(string formula) {
    vector<string> clauses;
    string clause = "";
    int paren_level = 0;
    for (char c : formula) {
        if (c == '(') paren_level++;
        if (c == ')') paren_level--;
        if (c == '*' && paren_level == 0) {
            if (!clause.empty()) clauses.push_back(clause);
            clause = "";
        } else {
            clause += c;
        }
    }
    if (!clause.empty()) clauses.push_back(clause);
    if (clauses.empty() && !formula.empty()) clauses.push_back(formula);
    if (clauses.empty()) return false;
    int count_false = 0;
    for (const string& cls : clauses) {
        if (!is_clause_true(cls)) count_false++;
    }

    cout<<"Number of false clauses : "<<count_false<<endl; 
    cout<<"Number of true clauses : "<<clauses.size()-count_false<<endl;
    return count_false == 0;
}


void showMenu() {
    cout << "\n--- Propositional Logic Toolkit ---" << endl;
    cout << "Operators: + (OR), * (AND), ~ (NOT), > (implication)" << endl;
    cout << "1. Convert Infix to Prefix" << endl;
    cout << "2. Convert Prefix to Parse Tree (Visual)" << endl;
    cout << "3. Convert Parse Tree back to Infix (from Prefix)" << endl;
    cout << "4. Calculate Height of Parse Tree (from Prefix)" << endl;
    cout << "5. Evaluate Truth Value of a Formula/Generate a Truth Table" << endl;
    cout << "6. Convert Formula to CNF" << endl;
    cout << "7. Check Validity of a CNF Formula" << endl;
    cout << "8. Exit" << endl;
    cout << "Enter your choice: ";
}

#include <iostream>
#include <chrono>
#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;
using namespace std::chrono;

int main() {
    int choice;
    string expression;

    while (true) {
        showMenu();

        // Start total timer (includes user input)
        auto total_start = high_resolution_clock::now();

        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {

            case 1: { // Infix to Prefix
                cout << "\nEnter infix expression: ";
                getline(cin, expression);

                // Start computation timer (code only)
                auto comp_start = high_resolution_clock::now();
                string prefix = infixToPrefix(expression);
                auto comp_stop = high_resolution_clock::now();

                cout << "Prefix Expression: " << prefix << endl;

                // Stop total timer after output
                auto total_stop = high_resolution_clock::now();

                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew) s\n";
                cout << " Total Time (input + computation + output): "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 2: { // Prefix to Parse Tree
                cout << "\nEnter prefix expression: ";
                getline(cin, expression);
                expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());

                auto comp_start = high_resolution_clock::now();
                Node* root = buildParseTree(expression);
                auto comp_stop = high_resolution_clock::now();

                cout << "\nGenerated Parse Tree:\n";
                printTree(root);
                deleteTree(root);

                auto total_stop = high_resolution_clock::now();

                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 3: { // Parse Tree to Infix
                cout << "\nEnter prefix expression: ";
                getline(cin, expression);
                expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());

                auto comp_start = high_resolution_clock::now();
                Node* root = buildParseTree(expression);
                string infix = parseTreeToInfix(root);
                auto comp_stop = high_resolution_clock::now();

                cout << "Generated Infix: " << infix << endl;
                deleteTree(root);

                auto total_stop = high_resolution_clock::now();

                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 4: { // Tree Height
                cout << "\nEnter prefix expression: ";
                getline(cin, expression);
                expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());

                auto comp_start = high_resolution_clock::now();
                Node* root = buildParseTree(expression);
                int height = getTreeHeight(root);
                auto comp_stop = high_resolution_clock::now();

                cout << "Tree Height: " << height << endl;
                deleteTree(root);

                auto total_stop = high_resolution_clock::now();

                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 5: { // Evaluate Truth Value + Truth Table
                cout << "\nEnter infix expression: ";
                getline(cin, expression);
                Node* root = buildParseTree(infixToPrefix(expression));

                set<char> vars;
                getVariables(root, vars);
                unordered_map<char, bool> values;

                if (!vars.empty()) {
                    cout << "Enter truth values (T/F or 1/0) for each variable:\n";
                    for (char var : vars) {
                        char val_char;
                        cout << "  " << var << " = ";
                        cin >> val_char;
                        values[var] = (val_char == 'T' || val_char == 't' || val_char == '1');
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                auto comp_start = high_resolution_clock::now();
                bool result = evaluate(root, values);
                auto comp_stop = high_resolution_clock::now();

                cout << "Result of the expression is: " << (result ? "True" : "False") << endl;

                if (!vars.empty()) {
                    cout << "\nTruth Table:\n";
                    vector<char> varList(vars.begin(), vars.end());
                    int n = varList.size();
                    for (char v : varList) cout << v << "\t";
                    cout << "Result\n";

                    for (int mask = 0; mask < (1 << n); ++mask) {
                        unordered_map<char, bool> rowValues;
                        for (int i = 0; i < n; ++i) {
                            rowValues[varList[i]] = (mask & (1 << (n - i - 1))) != 0;
                            cout << (rowValues[varList[i]] ? "T" : "F") << "\t";
                        }
                        cout << (evaluate(root, rowValues) ? "T" : "F") << "\n";
                    }
                }

                deleteTree(root);

                auto total_stop = high_resolution_clock::now();

                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 6: { // Convert to CNF
                cout << "\nEnter infix expression: ";
                getline(cin, expression);

                auto comp_start = high_resolution_clock::now();
                Node* root = buildParseTree(infixToPrefix(expression));
                root = impl_free(root);
                root = nnf(root);
                root = cnf(root);
                auto comp_stop = high_resolution_clock::now();

                cout << "Conjunctive Normal Form (CNF): ";
                printInfixFromTree(root);
                cout << endl;
                deleteTree(root);

                auto total_stop = high_resolution_clock::now();
                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 7: { // Check CNF Validity
                cout << "\nEnter CNF formula (e.g., (P + ~P) * (Q + ~Q)): ";
                getline(cin, expression);

                auto comp_start = high_resolution_clock::now();
                bool valid = check_cnf_valid(expression);
                auto comp_stop = high_resolution_clock::now();

                if (valid)
                    cout << "The CNF formula is valid (a tautology)." << endl;
                else
                    cout << "The CNF formula is NOT valid." << endl;

                auto total_stop = high_resolution_clock::now();
                cout << " Computation Time: "
                     << duration_cast<microseconds>(comp_stop - comp_start).count()
                     << " (mew)s\n";
                cout << " Total Time: "
                     << duration_cast<milliseconds>(total_stop - total_start).count()
                     << " ms\n";
                break;
            }

            case 8: { // Exit
                cout << "Exiting program." << endl;
                return 0;
            }

            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    }

    return 0;
}
