# Propositional Logic Toolkit

A C++ console application for working with **propositional logic expressions**. This toolkit allows users to convert expressions between **infix**, **prefix**, and **parse tree** formats, evaluate truth values, generate truth tables, convert formulas to **Conjunctive Normal Form (CNF)**, and check CNF validity.

---

## Features - main.cpp

1. **Convert Infix to Prefix**  
   - Input a logical expression in infix form (e.g., `(P + Q) * ~R`) and convert it to prefix notation.  
   - Supports operators:  
     - `+` : OR  
     - `*` : AND  
     - `~` : NOT  
     - `>` : IMPLICATION

2. **Visualize Parse Tree**  
   - Convert a prefix expression into a parse tree.  
   - Display the tree visually in the console.

3. **Parse Tree to Infix**  
   - Convert a prefix expression to a fully parenthesized infix expression using the parse tree.

4. **Calculate Parse Tree Height**  
   - Determines the height of the parse tree generated from a prefix expression.

5. **Evaluate Truth Value & Generate Truth Table**  
   - Evaluate a formula for specific truth assignments to variables.  
   - Generate a full truth table for all variables in the formula.

6. **Convert Formula to CNF**  
   - Transform any propositional formula into **Conjunctive Normal Form** using:  
     - Implication elimination  
     - Negation Normal Form (NNF)  
     - Distribution rules for CNF

7. **Check Validity of CNF Formula**  
   - Determines if a CNF formula is a **tautology** (always true).

8. **Timing**  
   - Displays both **computation time** and **total time** (including input/output).

---

## Supported Operators

| Operator | Symbol | Meaning        |
|----------|--------|----------------|
| OR       | `+`    | Logical OR     |
| AND      | `*`    | Logical AND    |
| NOT      | `~`    | Logical NOT    |
| Imply    | `>`    | Implication    |

---

## Usage

1. **Compile the program:**

```bash
g++ -std=c++17 -o propositional_logic main.cpp
```
2. **Run the program:**

### Linux/macOS:
```bash
./propositional_logic
```
### Windows
```bash
propositional_logic.exe
```
## Example Workflow

1. Select **1** to convert an infix expression to prefix notation.  
2. Select **2** to visualize a parse tree from a prefix expression.  
3. Select **3** to convert a parse tree back to infix notation.  
4. Select **4** to calculate the height of a parse tree.  
5. Select **5** to evaluate truth values and generate a truth table.  
6. Select **6** to convert a formula to Conjunctive Normal Form (CNF).  
7. Select **7** to check the validity of a CNF formula.  
8. Select **8** to exit the program.


## Key Functions

- **`infixToPrefix()`**: Converts infix to prefix notation.  
- **`buildParseTree()`**: Creates a parse tree from prefix expression.  
- **`parseTreeToInfix()`**: Converts a parse tree back to fully parenthesized infix.  
- **`getTreeHeight()`**: Computes the height of the parse tree.  
- **`evaluate()`**: Evaluates the logical formula based on truth assignments.  
- **`impl_free()`**: Replaces implication operators for CNF conversion.  
- **`nnf()`**: Converts formula to Negation Normal Form.  
- **`cnf()`**: Converts formula to Conjunctive Normal Form.  
- **`check_cnf_valid()`**: Validates a CNF formula for tautology.

---

## Notes

- Variables must be single uppercase or lowercase letters (e.g., `P`, `Q`, `R`).  
- Parentheses must be correctly balanced.  
- Input is **case-insensitive** for truth values (`T/t/1` = True, `F/f/0` = False).  
- The program uses a **visual tree representation** for better understanding of formula structure.  
- Computation times are reported in **microseconds** for the algorithm and **milliseconds** for total time including I/O.



## Documentation

The project documentation is generated using **Doxygen**.


### To regenerate documentation locally

1. Make sure **Doxygen** is installed.  
2. Navigate to the project folder in the terminal:  

```bash
doxygen Doxyfile
```

# Project Members and Details
1. **Punya Pratap Singh** (2023B3A70280H)
2. **Tarun Pradeep Krishnamurthy** (2023B3A70398H)
3. **Aditya Bhat** (2023B3A70448H)
4. **Aryaman Srivastava** (2023B5A70764H)
5. **Lohit Vijayabaskar** (2023B5A70778H)