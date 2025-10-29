#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include <vector>
#include <set>
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <sstream> 
#include <cmath>

using namespace std;

struct Node {
    string data; 
    Node* left;
    Node* right;

    Node(string val) : data(val), left(nullptr), right(nullptr) {}
};

using Literal = int;
using Clause = vector<Literal>;
using DimacsCNF = vector<Clause>;

Node* buildParseTree_helper(string& expression); 

bool isOperator(const string& s) {
    return s == "+" || s == "*" || s == "~" || s == ">";
}

bool isOperand(const string& s) {
    return !s.empty() && !isOperator(s) && s != "(" && s != ")";
}

int getPrecedence(const string& op) {
    if (op == "~") return 3;
    if (op == "*") return 2;
    if (op == "+") return 1;
    if (op == ">") return 0;
    return -1; 
}

vector<string> tokenize(const string& infix) {
    vector<string> tokens;
    string current_token;
    for (size_t i = 0; i < infix.length(); ++i) {
        char c = infix[i];
        if (isspace(c)) continue;

        string s_c = string(1, c);
        if (c == '(' || c == ')' || c == '+' || c == '*' || c == '~' || c == '>') {
            tokens.push_back(s_c);
        } else {
            current_token = c;
            while (i + 1 < infix.length() && !isspace(infix[i+1]) &&
                   infix[i+1] != '(' && infix[i+1] != ')' && infix[i+1] != '+' &&
                   infix[i+1] != '*' && infix[i+1] != '~' && infix[i+1] != '>') {
                current_token += infix[i+1];
                i++;
            }
            tokens.push_back(current_token);
        }
    }
    return tokens;
}

string infixToPrefix(string infix) {
    vector<string> tokens = tokenize(infix);
    reverse(tokens.begin(), tokens.end());

    for (string& token : tokens) {
        if (token == "(")
            token = ")";
        else if (token == ")")
            token = "(";
    }

    stack<string> s;
    string prefix; 

    for (const string& token : tokens) {
        if (isOperand(token)) {
            prefix += token + " ";
        } else if (token == "(") {
            s.push(token);
        } else if (token == ")") {
            while (!s.empty() && s.top() != "(") {
                prefix += s.top() + " ";
                s.pop();
            }
            if (!s.empty()) s.pop(); 
        } else if (isOperator(token)) {
            while (!s.empty() && getPrecedence(s.top()) > getPrecedence(token)) {
                prefix += s.top() + " ";
                s.pop();
            }
            s.push(token);
        }
    }

    while (!s.empty()) {
        prefix += s.top() + " ";
        s.pop();
    }

    stringstream ss(prefix);
    string token;
    vector<string> prefix_tokens;
    while (ss >> token) {
        prefix_tokens.push_back(token);
    }
    reverse(prefix_tokens.begin(), prefix_tokens.end());
    
    string final_prefix;
    for(const string& t : prefix_tokens) {
        final_prefix += t + " ";
    }
    if (!final_prefix.empty()) {
        final_prefix.pop_back(); 
    }
    return final_prefix;
}

string getNextToken(string& expression) {
    expression.erase(0, expression.find_first_not_of(" \t\n\r"));
    if (expression.empty()) return "";
    
    size_t first_space = expression.find(' ');
    if (first_space == string::npos) {
        string token = expression;
        expression.clear();
        return token;
    }
    
    string token = expression.substr(0, first_space);
    expression.erase(0, first_space);
    return token;
}

Node* buildParseTree(string expression) {
    if (expression.empty()) return nullptr;
    return buildParseTree_helper(expression); 
}

Node* buildParseTree_helper(string& expression) {
    if (expression.empty()) return nullptr;

    string token = getNextToken(expression);
    if (token.empty()) return nullptr;

    Node* node = new Node(token);

    if (isOperator(token)) {
        node->left = buildParseTree_helper(expression);
        if (token != "~") { 
            node->right = buildParseTree_helper(expression);
        }
    }
    return node;
}

void deleteTree(Node* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

void printTree(Node* root, const string& prefix = "", bool isLeft = false) {
    if (root != nullptr) {
        cout << prefix;
        cout << (isLeft ? "|-- " : "L-- ");
        cout << root->data << endl;

        printTree(root->left, prefix + (isLeft ? "|   " : "    "), true);
        printTree(root->right, prefix + (isLeft ? "|   " : "    "), false);
    }
}

string parseTreeToInfix(Node* root) {
    if (root == nullptr) return "";
    if (isOperand(root->data)) return root->data;

    string left = parseTreeToInfix(root->left);
    string right = parseTreeToInfix(root->right);

    if (root->data == "~") {
        if(root->left && (root->left->data == "+" || root->left->data == "*" || root->left->data == ">")) {
             return "~(" + left + ")"; 
        }
        return "~" + left;
    }
    return "(" + left + " " + root->data + " " + right + ")";
}

int getTreeHeight(Node* root) {
    if (root == nullptr) {
        return -1;
    }
    return 1 + max(getTreeHeight(root->left), getTreeHeight(root->right));
}

void getVariables(Node* root, set<string>& vars) {
    if (root == nullptr) return;
    if (isOperand(root->data)) {
        vars.insert(root->data);
    }
    getVariables(root->left, vars);
    getVariables(root->right, vars);
}

bool evaluate(Node* root, const unordered_map<string, bool>& values) {
    if (root == nullptr) throw runtime_error("Invalid expression tree.");
    if (isOperand(root->data)) {
        if (values.find(root->data) == values.end()) {
            throw runtime_error("No truth value for variable '" + root->data + "'");
        }
        return values.at(root->data);
    }

    if (root->data == "~") {
        return !evaluate(root->left, values);
    }
    
    bool left_val = evaluate(root->left, values);
    
    if (root->data == "+" || root->data == "*" || root->data == ">") {
        bool right_val = evaluate(root->right, values);
        if (root->data == "+") return left_val || right_val;
        if (root->data == "*") return left_val && right_val;
        if (root->data == ">") return !left_val || right_val;
    }

    throw runtime_error("Unknown operator in tree: " + root->data);
}

Node* impl_free(Node* root) {
    if (!root) return nullptr;
    if (root->data == ">") {
        Node* new_or = new Node("+");
        Node* new_not = new Node("~");
        
        new_not->left = impl_free(root->left);
        new_or->left = new_not;
        new_or->right = impl_free(root->right);
        
        delete root;
        return new_or;
    }
    root->left = impl_free(root->left);
    root->right = impl_free(root->right);
    return root;
}

Node* nnf(Node* root) {
    if (!root) return nullptr;
    
    if (isOperand(root->data) || (root->data == "~" && isOperand(root->left->data))) return root;

    if (root->data == "~") {
        Node* child = root->left;
        
        if (child->data == "~") { 
            Node* grandchild = child->left;
            child->left = nullptr;
            delete root;
            delete child;
            return nnf(grandchild);
        }
        
        if (child->data == "+") { 
            Node* new_and = new Node("*");
            Node* notA = new Node("~");
            Node* notB = new Node("~");

            notA->left = child->left; 
            notB->left = child->right;
            
            child->left = nullptr; 
            child->right = nullptr;

            new_and->left = nnf(notA);
            new_and->right = nnf(notB);

            delete root;
            delete child;
            return new_and;
        }
        if (child->data == "*") { 
            Node* new_or = new Node("+");
            Node* notA = new Node("~");
            Node* notB = new Node("~");
            
            notA->left = child->left; 
            notB->left = child->right;

            child->left = nullptr; 
            child->right = nullptr;

            new_or->left = nnf(notA);
            new_or->right = nnf(notB);

            delete root;
            delete child;
            return new_or;
        }
    }
    
    root->left = nnf(root->left);
    root->right = nnf(root->right);
    return root;
}

Node* copyTree(Node* root) {
    if (!root) return nullptr;
    Node* newNode = new Node(root->data);
    newNode->left = copyTree(root->left);
    newNode->right = copyTree(root->right);
    return newNode;
}

Node* cnf(Node* root) {
    if (!root) return nullptr;
    if (isOperand(root->data) || root->data == "~") return root;

    root->left = cnf(root->left);
    root->right = cnf(root->right);

    if (root->data == "+") {
        // Distributive Law: (A * B) + C -> (A + C) * (B + C)
        if (root->left && root->left->data == "*") {
            Node* l = root->left;
            Node* r = root->right;
            
            Node* p = l->left;
            Node* q = l->right;

            Node* new_and = new Node("*");

            Node* p_plus_r = new Node("+");
            p_plus_r->left = p;
            p_plus_r->right = r;

            Node* q_plus_r_copy = new Node("+");
            q_plus_r_copy->left = copyTree(q);
            q_plus_r_copy->right = copyTree(r); 

            l->left = nullptr;
            l->right = nullptr;
            root->right = nullptr;

            delete l;
            delete root;

            new_and->left = cnf(p_plus_r);
            new_and->right = cnf(q_plus_r_copy); 

            return new_and;
        }
        // Distributive Law: C + (A * B) -> (C + A) * (C + B)
        if (root->right && root->right->data == "*") {
            Node* l = root->left;
            Node* r = root->right;

            Node* p = l;
            Node* q = r->left;
            Node* s = r->right; 

            Node* new_and = new Node("*");

            Node* p_copy_plus_q = new Node("+");
            p_copy_plus_q->left = copyTree(p);
            p_copy_plus_q->right = q;

            Node* p_plus_s = new Node("+");
            p_plus_s->left = p;
            p_plus_s->right = s;
            
            root->left = nullptr;
            r->left = nullptr;
            r->right = nullptr;

            delete r;
            delete root;

            new_and->left = cnf(p_copy_plus_q);
            new_and->right = cnf(p_plus_s);
            
            return new_and;
        }
    }
    return root;
}

string treeToInfixString(Node* root) {
    if (root == nullptr) return "";
    if (isOperand(root->data)) return root->data;

    string left = treeToInfixString(root->left);
    string right = treeToInfixString(root->right);

    if (root->data == "~") {
        if(root->left && (root->left->data == "+" || root->left->data == "*" || root->left->data == ">")) {
             return "~(" + left + ")"; 
        }
        return "~" + left;
    }
    return "(" + left + " " + root->data + " " + right + ")";
}

void printInfixFromTree(Node* root) {
    if (root == nullptr) return;
    if (isOperand(root->data)) {
        cout << root->data;
        return;
    }
    if (root->data == "~") {
        cout << "~";
        if(root->left && (root->left->data == "+" || root->left->data == "*" || root->left->data == ">" || root->left->data == "~")) {
             cout << "(";
             printInfixFromTree(root->left);
             cout << ")";
        } else {
             printInfixFromTree(root->left);
        }
        return;
    }
    cout << "(";
    printInfixFromTree(root->left);
    cout << " " << root->data << " ";
    printInfixFromTree(root->right);
    cout << ")";
}

bool check_cnf_valid(const string& cnf_formula) {
    string formula = cnf_formula;
    
    size_t start = 0;
    size_t end = formula.find(" * ");

    while(start < formula.length()) {
        string clause_str = formula.substr(start, end - start);
        if(clause_str.empty()) {
             if(end == string::npos) break;
             start = end + 3; 
             end = formula.find(" * ", start);
             continue;
        }
        
        if(clause_str.front() == '(' && clause_str.back() == ')') {
            clause_str = clause_str.substr(1, clause_str.length() - 2);
        }

        vector<string> tokens = tokenize(clause_str);
        set<string> literals;
        set<string> negated_literals;

        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i] == "~") {
                if (i + 1 < tokens.size() && isOperand(tokens[i+1])) {
                    negated_literals.insert(tokens[i+1]);
                    i++;
                }
            } else if (isOperand(tokens[i])) {
                literals.insert(tokens[i]);
            }
        }
        
        bool has_complement = false;
        for(const string& lit : literals) {
            if(negated_literals.count(lit)) {
                has_complement = true;
                break;
            }
        }
        
        if(!has_complement) {
            return false; 
        }

        if(end == string::npos) break;
        start = end + 3; 
        end = formula.find(" * ", start);
    }

    return true; 
}

DimacsCNF readDIMACSCNF(const string& dimacsInput, int& numVars, int& numClauses) {
    DimacsCNF formula;
    stringstream ss(dimacsInput);
    string line;
    numVars = 0;
    numClauses = 0;
    bool p_line_found = false;

    while (getline(ss, line)) {
        if (line.empty() || line[0] == 'c') continue; 
        
        if (line[0] == 'p') {
            stringstream ps(line.substr(1));
            string type;
            if (!(ps >> type >> numVars >> numClauses) || type != "cnf") {
                cerr << "Error: Invalid or missing 'p cnf' line." << endl;
                return {};
            }
            p_line_found = true;
            continue;
        }
        
        if (!p_line_found) continue;

        stringstream cs(line);
        Literal lit;
        Clause currentClause;
        bool clause_ended = true;

        while (cs >> lit) {
            if (lit == 0) {
                if (!currentClause.empty() || clause_ended == false) {
                     formula.push_back(currentClause);
                }
                currentClause.clear();
                clause_ended = true;
            } else {
                currentClause.push_back(lit);
                clause_ended = false;
            }
        }
    }
    return formula;
}

bool is_dimacs_clause_true(const Clause& clause) {
    set<int> positiveLits;
    for (Literal lit : clause) {
        if (lit > 0) {
            positiveLits.insert(lit);
        }
    }
    for (Literal lit : clause) {
        if (lit < 0) {
            if (positiveLits.count(-lit)) {
                return true;
            }
        }
    }
    return false;
}

bool check_dimacs_valid_formula(const DimacsCNF& formula) {
    int count_false = 0;
    for (const auto& cls : formula) {
        if (!is_dimacs_clause_true(cls)) {
            count_false++;
        }
    }
    cout << "Number of non-tautology clauses: " << count_false << endl;
    cout << "Number of tautology clauses: " << formula.size() - count_false << endl;
    return count_false == 0; 
}

string dimacsToInfix(const DimacsCNF& formula) {
    if (formula.empty()) return "";

    stringstream infix_ss;
    for (size_t i = 0; i < formula.size(); ++i) {
        const auto& clause = formula[i];
        if (clause.empty()) continue;

        infix_ss << "(";
        for (size_t j = 0; j < clause.size(); ++j) {
            Literal lit = clause[j];
            
            if (lit < 0) {
                infix_ss << "~P" << -lit;
            } else {
                infix_ss << "P" << lit;
            }
            if (j < clause.size() - 1) {
                infix_ss << " + ";
            }
        }
        infix_ss << ")";

        if (i < formula.size() - 1) {
            infix_ss << " * ";
        }
    }
    return infix_ss.str();
}

string getExpressionFromInput(string requiredFormat) { 
    int choice;
    string expression;
    cout << "\n  Choose input format:" << endl;
    cout << "  1. Manual " << requiredFormat << " entry" << endl;
    cout << "  2. DIMACS format (converts to " << requiredFormat << ")" << endl;
    cout << "  Enter choice: ";
    
    while (!(cin >> choice) || (choice != 1 && choice != 2)) {
        cout << "Invalid input. Please enter 1 or 2." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    if (choice == 1) {
        cout << "\nEnter " << requiredFormat << " expression: ";
        getline(cin, expression);
        return expression;
    } else { 
        cout << "\nEnter DIMACS CNF input (paste lines, finish with a blank line): \n";
        string dimacsInput, line;
        while (getline(cin, line) && !line.empty()) {
            dimacsInput += line + "\n";
        }
        
        int numVars, numClauses;
        DimacsCNF formula = readDIMACSCNF(dimacsInput, numVars, numClauses);
        string infix_expr = dimacsToInfix(formula);
        
        cout << "Generated infix from DIMACS: " << infix_expr << endl;

        if (requiredFormat == "infix") {
            return infix_expr;
        } else { 
            string prefix_expr = infixToPrefix(infix_expr);
            cout << "Converted to prefix: " << prefix_expr << endl;
            return prefix_expr;
        }
    }
}

void run_automated_case(int case_num, const string& title, const string& input_data, const string& format = "infix") {
    cout << "\n==========================================================================" << endl;
    cout << title << endl;
    
    try {
        if (case_num == 1) { 
            string prefix = infixToPrefix(input_data);
            cout << prefix << endl;
        } else if (case_num == 2) { 
            string prefix_copy = input_data;
            Node* root = buildParseTree(prefix_copy);
            cout << "Parse Tree:\n";
            printTree(root);
            deleteTree(root);
        } else if (case_num == 3) { 
            string prefix_copy = input_data;
            Node* root = buildParseTree(prefix_copy);
            cout << "Infix: " << parseTreeToInfix(root) << endl;
            deleteTree(root);
        } else if (case_num == 4) { 
            string prefix_copy = input_data;
            Node* root = buildParseTree(prefix_copy);
            cout << "Tree Height: " << getTreeHeight(root) << endl;
            deleteTree(root);
        } else if (case_num == 5) { 
            string prefix = infixToPrefix(input_data);
            Node* root = buildParseTree(prefix);
            set<string> vars;
            getVariables(root, vars);
            
            cout << "\n--- Truth Table ---" << endl;
            vector<string> varList(vars.begin(), vars.end());
            int n = varList.size();

            if (n > 18) {
                cout << "Cannot generate truth table for formulas with more than 18 variables (Found: " << n << ")." << endl;
                deleteTree(root);
                return;
            }
            
            for (const string& v : varList) {
                cout << v << "\t";
            }
            cout << "Result" << endl;

            long long row_count = 1LL << n; 

            for (long long i = 0; i < row_count; ++i) {
                unordered_map<string, bool> rowValues;
                for (int j = 0; j < n; ++j) {
                    bool val = (i >> (n - 1 - j)) & 1;
                    rowValues[varList[j]] = val;
                    cout << (val ? "T" : "F") << "\t";
                }
                try {
                    cout << (evaluate(root, rowValues) ? "T" : "F") << endl;
                } catch(const runtime_error& e) {
                     cout << "ERROR" << endl;
                }
            }
            deleteTree(root);
        } else if (case_num == 6) { 
            string prefix = infixToPrefix(input_data);
            Node* root = buildParseTree(prefix);
            
            cout << "CNF Conversion Steps:" << endl;
            
            Node* impl_free_root = impl_free(root);
            cout << "  1. Implication-Free: ";
            printInfixFromTree(impl_free_root);
            cout << endl;

            Node* nnf_root = nnf(impl_free_root);
            cout << "  2. Negation Normal Form (NNF): ";
            printInfixFromTree(nnf_root);
            cout << endl;

            Node* final_cnf_root = cnf(nnf_root);
            string final_cnf_infix = treeToInfixString(final_cnf_root);
            cout << "  3. Conjunctive Normal Form (CNF): " << final_cnf_infix << endl;
            
            cout << "\n--- Validity Check on Generated CNF ---" << endl;
            if (final_cnf_infix.empty()) {
                cout << "  Result: CNF is empty." << endl;
            } else if (check_cnf_valid(final_cnf_infix)) {
                cout << "  Result: The CNF formula is valid (a tautology)." << endl;
            } else {
                cout << "  Result: The CNF formula is NOT valid (not a tautology)." << endl;
            }
            
            deleteTree(final_cnf_root);
        } else if (case_num == 7) { 
            bool valid = check_cnf_valid(input_data);
            if (valid)
                cout << "The CNF formula is valid (a tautology)." << endl;
            else
                cout << "The CNF formula is NOT valid (not a tautology)." << endl;
        } else if (case_num == 8) { 
            int numVars, numClauses;
            DimacsCNF formula = readDIMACSCNF(input_data, numVars, numClauses);

            bool valid = check_dimacs_valid_formula(formula);
            
            if (valid)
                cout << "The DIMACS CNF formula is valid (a tautology)." << endl;
            else
                cout << "The DIMACS CNF formula is NOT valid (contains non-tautology clauses)." << endl;
        } else if (case_num == 9) { 
            int numVars, numClauses;
            DimacsCNF formula = readDIMACSCNF(input_data, numVars, numClauses);
            cout << "Infix String: " << dimacsToInfix(formula) << endl;
        }
    } catch (const exception& e) {
        cout << "ERROR during analysis: " << e.what() << endl;
    }
    cout << "==========================================================================" << endl;
}

void run_automated_workflow() {
    cout << "\n==========================================================================" << endl;
    cout << "AUTOMATED WORKFLOW DEMONSTRATION" << endl;
    cout << "Runs a series of analysis steps on a single formula you provide." << endl;
    cout << "==========================================================================" << endl;

    int choice;
    cout << "\n  Choose input format for the workflow:" << endl;
    cout << "  1. Infix Formula (e.g., (A > B) * ~C)" << endl;
    cout << "  2. DIMACS CNF Format" << endl;
    cout << "  Enter choice: ";
    
    while (!(cin >> choice) || (choice != 1 && choice != 2)) {
        cout << "Invalid input. Please enter 1 or 2." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    string input_data_raw;
    string final_infix;

    if (choice == 1) {
        cout << "\nEnter Infix formula: ";
        getline(cin, input_data_raw);
        final_infix = input_data_raw;
    } else {
        cout << "\nEnter DIMACS CNF input (paste lines, finish with a blank line): \n";
        string line;
        while (getline(cin, line) && !line.empty()) {
            input_data_raw += line + "\n";
        }
        
        int numVars, numClauses;
        DimacsCNF formula = readDIMACSCNF(input_data_raw, numVars, numClauses);
        final_infix = dimacsToInfix(formula);
        
        cout << "Converted to Infix: " << final_infix << endl;
    }

    if (final_infix.empty()) {
         cout << "\nError: Could not process formula or formula is empty. Aborting workflow." << endl;
         return;
    }
    
    string final_prefix = infixToPrefix(final_infix);

    cout << "\n--- ANALYSIS STEPS STARTING ---" << endl;
    
    run_automated_case(2, "ANALYSIS A: Parse Tree Visualization", final_prefix, "prefix");
    run_automated_case(4, "ANALYSIS B: Tree Height Calculation", final_prefix, "prefix");
    run_automated_case(5, "ANALYSIS C: Truth Table Generation", final_infix);
    run_automated_case(6, "ANALYSIS D: Full CNF Conversion and Validity Check", final_infix);

    cout << "\n==========================================================================" << endl;
    cout << "USER-DRIVEN WORKFLOW COMPLETE." << endl;
    cout << "==========================================================================" << endl;
}

void showMenu() {
    cout << "\n--- Propositional Logic Toolkit ---" << endl;
    cout << "Operators: + (OR), * (AND), ~ (NOT), > (implication)" << endl;
    cout << "Variables: Single letters (A) or P-notation (P1, P10, etc.)" << endl;
    cout << "1. Convert Infix to Prefix" << endl;
    cout << "2. Convert Prefix to Parse Tree (Visual)" << endl;
    cout << "3. Convert Parse Tree back to Infix (from Prefix)" << endl;
    cout << "4. Calculate Height of Parse Tree (from Prefix)" << endl;
    cout << "5. Evaluate Truth Value / Generate Truth Table" << endl;
    cout << "6. Convert Formula to CNF (Impl-free, NNF, CNF) and Check Validity" << endl;
    cout << "7. Check Validity of a CNF Formula (Infix String)" << endl;
    cout << "8. Check Validity of CNF (DIMACS Format)" << endl;
    cout << "9. Convert DIMACS to Infix String" << endl;
    cout << "----------------------------------------------------------------" << endl;
    cout << "11. Run User-Driven Workflow Demonstration" << endl;
    cout << "12. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    int choice;

    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        switch (choice) {
            case 1: { 
                string expression = getExpressionFromInput("infix");
                cout << "Prefix Expression: " << infixToPrefix(expression) << endl;
                break;
            }
            case 2: { 
                string expression = getExpressionFromInput("prefix");
                Node* root = buildParseTree(expression);
                cout << "\nParse Tree:\n";
                printTree(root);
                deleteTree(root);
                break;
            }
            case 3: { 
                string expression = getExpressionFromInput("prefix");
                Node* root = buildParseTree(expression);
                cout << "Infix: " << parseTreeToInfix(root) << endl;
                deleteTree(root);
                break;
            }
            case 4: { 
                string expression = getExpressionFromInput("prefix");
                Node* root = buildParseTree(expression);
                cout << "Tree Height: " << getTreeHeight(root) << endl;
                deleteTree(root);
                break;
            }
            case 5: { 
                string expression = getExpressionFromInput("infix");
                string prefix_eval = infixToPrefix(expression);
                Node* root = buildParseTree(prefix_eval);
                set<string> vars;
                getVariables(root, vars);
                unordered_map<string, bool> values;

                cout << "\n--- Evaluation Mode ---" << endl;
                if (!vars.empty()) {
                    cout << "Enter truth values (T/F or 1/0) for a specific row, or skip for full table:" << endl;
                    for (const string& var : vars) {
                        char val_char;
                        cout << "  " << var << " = ";
                        cin >> val_char;
                        values[var] = (val_char == 'T' || val_char == 't' || val_char == '1');
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    try {
                        cout << "Result for specified values: " << (evaluate(root, values) ? "True" : "False") << endl;
                    } catch (const exception& e) {
                        cout << "Evaluation Error: " << e.what() << endl;
                    }
                }

                if (!vars.empty()) {
                    char generate_table;
                    cout << "\nGenerate full truth table? (Y/N): ";
                    cin >> generate_table;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

                    if (generate_table == 'Y' || generate_table == 'y') {
                        if (vars.size() > 18) {
                            cout << "Cannot generate truth table for formulas with more than 18 variables (Found: " << vars.size() << ")." << endl;
                            deleteTree(root);
                            break;
                        }
                        
                        cout << "\n--- Truth Table ---" << endl;
                        vector<string> varList(vars.begin(), vars.end());
                        int n = varList.size();

                        for (const string& v : varList) {
                            cout << v << "\t";
                        }
                        cout << "Result" << endl;

                        long long row_count = 1LL << n; 

                        for (long long i = 0; i < row_count; ++i) {
                            unordered_map<string, bool> rowValues;
                            for (int j = 0; j < n; ++j) {
                                bool val = (i >> (n - 1 - j)) & 1;
                                rowValues[varList[j]] = val;
                                cout << (val ? "T" : "F") << "\t";
                            }
                            try {
                                cout << (evaluate(root, rowValues) ? "T" : "F") << endl;
                            } catch (const exception& e) {
                                cout << "ERROR" << endl;
                            }
                        }
                    }
                } else {
                     cout << "Expression has no variables. Result: " << (evaluate(root, values) ? "True" : "False") << endl;
                }
                
                deleteTree(root);
                break;
            }
            case 6: { 
                string expression = getExpressionFromInput("infix");
                string prefix_cnf = infixToPrefix(expression);
                Node* root = buildParseTree(prefix_cnf);
                
                cout << "\n--- CNF Conversion ---" << endl;
                
                Node* impl_free_root = impl_free(root);
                cout << "  1. Implication-Free: ";
                printInfixFromTree(impl_free_root);
                cout << endl;

                Node* nnf_root = nnf(impl_free_root);
                cout << "  2. Negation Normal Form (NNF): ";
                printInfixFromTree(nnf_root);
                cout << endl;

                Node* final_cnf_root = cnf(nnf_root);
                string final_cnf_infix = treeToInfixString(final_cnf_root);
                cout << "  3. Conjunctive Normal Form (CNF): " << final_cnf_infix << endl;

                cout << "\n--- Validity Check on Generated CNF ---" << endl;
                if (final_cnf_infix.empty()) {
                    cout << "  Result: CNF is empty." << endl;
                } else if (check_cnf_valid(final_cnf_infix)) {
                    cout << "  Result: The CNF formula is valid (a tautology)." << endl;
                } else {
                    cout << "  Result: The CNF formula is NOT valid (not a tautology)." << endl;
                }
                
                deleteTree(final_cnf_root);
                break;
            }
            case 7: { 
                cout << "\n(Input must be in CNF, e.g., (A + B) * (~A + B))" << endl;
                string expression = getExpressionFromInput("infix");
                if (check_cnf_valid(expression))
                    cout << "The CNF formula is valid (a tautology)." << endl;
                else
                    cout << "The CNF formula is NOT valid." << endl;
                break;
            }
            case 8: { 
                cout << "\nEnter DIMACS CNF input (paste lines, finish with a blank line): \n";
                string dimacsInput, line;
                while (getline(cin, line) && !line.empty()) {
                    dimacsInput += line + "\n";
                }
                
                int numVars, numClauses;
                DimacsCNF formula = readDIMACSCNF(dimacsInput, numVars, numClauses);
                
                cout << "\n--- DIMACS Analysis ---" << endl;
                bool valid = check_dimacs_valid_formula(formula);
                
                if (valid)
                    cout << "The DIMACS CNF formula is valid (a tautology)." << endl;
                else
                    cout << "The DIMACS CNF formula is NOT valid (has non-tautology clauses)." << endl;
                break;
            }
            case 9: { 
                cout << "\nEnter DIMACS CNF input (paste lines, finish with a blank line): \n";
                string dimacsInput, line;
                while (getline(cin, line) && !line.empty()) {
                    dimacsInput += line + "\n";
                }
                
                int numVars, numClauses;
                DimacsCNF formula = readDIMACSCNF(dimacsInput, numVars, numClauses);
                
                cout << "\n--- DIMACS to Infix ---" << endl;
                cout << "Infix String: " << dimacsToInfix(formula) << endl;
                break;
            }
            case 11: {
                run_automated_workflow();
                break;
            }
            case 12: { 
                cout << "Exiting program." << endl;
                return 0;
            }
            default: {
                cout << "Invalid choice. Please try again." << endl;
                break;
            }
        }
    }
    return 0;
}
