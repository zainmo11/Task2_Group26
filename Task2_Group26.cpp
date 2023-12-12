#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <bitset>
#include <regex>
using namespace std;

// Function to evaluate a logical expression
// Takes a logical expression and a map of variable values
// Returns the result of evaluating the expression
bool evaluateLogicalExpression(const std::string& expression, const std::unordered_map<char, bool>& variableValues);

// Function to generate truth table cases for a logical expression
// Takes a logical expression
// Returns a vector of boolean values representing all possible combinations of input variables
vector<bool> generateTruthTableCases(const std::string& expression);

// Function to generate and display a truth table based on the results
// Takes a vector of boolean values representing the results of a logical expression
void generateTruthTable(const std::vector<bool>& results);

// Function to check if two truth tables are equivalent
// Takes two vectors of boolean values representing truth tables
// Returns true if the tables are equivalent, false otherwise
bool CheckEquivalent(vector<bool> & truthTableCases,const vector<bool>& simplify_truthTableCases);

// Function to replace gates in a logical expression based on a truth table
// Takes a logical expression and a vector of boolean values representing a truth table
// Returns the modified logical expression
string replaceGate(string& expression, vector<bool> &truthTableCases);

// Function to find indices where two vectors are the same
void findMatchingIndices(const std::vector<bool>& vector1, const std::vector<bool>& vector2);


void HowToUse();

int main() {
    // Display instructions for using the program
    HowToUse();

    // Define the original logical expression and a simplified version
    string logicalExpression = "(((!(A|!B)) & (A & B) ) & C)";
    string simplify_logicalExpression;
    getline(cin,simplify_logicalExpression) ;
    if  (simplify_logicalExpression == "yes" ||simplify_logicalExpression == "YES" ) {
        simplify_logicalExpression = "(A | !(A))";
    }
    if (!regex_search(simplify_logicalExpression, regex("^[a-zA-Z&|!()\\s]+$"))){
        std::cerr << "Error: The string is not valid and  must contains only alphabet, &, |, and ! characters." << std::endl;
        return 1; // Return an error code
    }
    else {
        // Generate truth table cases for the original and simplified expressions
        vector<bool> truthTableCases = generateTruthTableCases(logicalExpression);
        vector<bool> simplify_truthTableCases = generateTruthTableCases(simplify_logicalExpression);

        // Display the truth table for the original logical expression
        cout << "\nTruth Table for logic circuit expression: " << logicalExpression << endl;
        generateTruthTable(truthTableCases);

        // Display the truth table for the simplified logical expression
        cout << "Truth Table for simplified logic expression: " << simplify_logicalExpression << endl;
        generateTruthTable(simplify_truthTableCases);

        bool isEquivalent = CheckEquivalent(truthTableCases, simplify_truthTableCases);
        // Check if the two expressions are equivalent and display the result
        cout << "Are the two expressions equivalent?\n" << std::boolalpha << isEquivalent << "\n\n";

        // Find matching indices
        findMatchingIndices(truthTableCases, simplify_truthTableCases);
        if (!isEquivalent) {
            // Replace gates in the simplified expression based on the original truth table
            simplify_logicalExpression = replaceGate(simplify_logicalExpression, truthTableCases);

            // Regenerate the truth table for the original and modified simplified expressions
            simplify_truthTableCases = generateTruthTableCases(simplify_logicalExpression);
            cout << "Truth Table for logic circuit expression: " << logicalExpression << endl;
            generateTruthTable(truthTableCases);
            cout << "Truth Table for simplified logic expression after gate replacement: " << simplify_logicalExpression
                 << endl;

            // Check equivalence again after replacing gates and display the result
            bool replacegate = CheckEquivalent(truthTableCases, simplify_truthTableCases);
            generateTruthTable(simplify_truthTableCases);
            cout << "Are the two expressions equivalent after gate replacement?\n" << std::boolalpha << replacegate
                 << "\n\n";

            // Display the final result based on equivalence
            replacegate ? cout << "This is the equivalent logical Expression: " << simplify_logicalExpression << endl :
            cout << "There is no equivalent logical Expression by editing this expression.";
        }
    }
    return 0;
}
bool evaluateLogicalExpression(const std::string& expression, const std::unordered_map<char, bool>& variableValues) {
    stack<char> operators;
    stack<bool> operands;

    for (char i : expression) {
        if (isalpha(i)) {
            // If the character is a variable, push its value onto the operand stack
            char variable = i;
            bool value = variableValues.at(variable);
            operands.push(value);
        } else if (i == '0' || i == '1') {
            // If the character is a '0' or '1', push its value onto the operand stack
            bool value = i == '1';
            operands.push(value);
        } else if (i == '&' || i == '|' || i == '!') {
            // If the character is a logical operator, perform operations
            while (!operators.empty() && operators.top() != '(' && ((i == '!' && operators.top() == '!') || (i != '!' && (operators.top() == '|' || operators.top() == '&')))) {
                bool operand2 = operands.top();
                operands.pop();
                if (operators.top() != '!') {
                    bool operand1 = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    bool result;
                    switch (op) {
                        case '&':
                            result = operand1 && operand2;
                            break;
                        case '|':
                            result = operand1 || operand2;
                            break;
                    }
                    operands.push(result);
                } else {
                    operators.pop(); // Pop '!' from the operator stack
                    operands.push(!operand2); // Perform unary NOT operation
                }
            }
            operators.push(i);
        } else if (i == '(') {
            operators.push(i);
        } else if (i == ')') {
            while (!operators.empty() && operators.top() != '(') {
                bool operand2 = operands.top();
                operands.pop();
                if (operators.top() != '!') {
                    bool operand1 = operands.top();
                    operands.pop();
                    char op = operators.top();
                    operators.pop();
                    bool result;
                    switch (op) {
                        case '&':
                            result = operand1 && operand2;
                            break;
                        case '|':
                            result = operand1 || operand2;
                            break;
                    }
                    operands.push(result);
                } else {
                    operators.pop(); // Pop '!' from the operator stack
                    operands.push(!operand2); // Perform unary NOT operation
                }
            }
            if (!operators.empty()) {
                operators.pop(); // Pop '(' from the operator stack
            }
        }
    }

    // Evaluate any remaining operators and operands
    while (!operators.empty()) {
        bool operand2 = operands.top();
        operands.pop();
        if (operators.top() != '!') {
            bool operand1 = operands.top();
            operands.pop();
            char op = operators.top();
            operators.pop();
            bool result;
            switch (op) {
                case '&':
                    result = operand1 && operand2;
                    break;
                case '|':
                    result = operand1 || operand2;
                    break;
            }
            operands.push(result);
        } else {
            operators.pop(); // Pop '!' from the operator stack
            operands.push(!operand2); // Perform unary NOT operation
        }
    }

    // Return the final result
    return operands.top();
}

vector<bool> generateTruthTableCases(const std::string& expression) {
    // Extract variable names from the expression
    std::unordered_set<char> variables;
    for (char c : expression) {
        if (isalpha(c)) {
            variables.insert(c);
        }
    }

    // Create a vector to store the truth table cases
    std::vector<bool> truthTableCases;

    // Generate truth table cases
    int numVariables = 3;
    for (int i = 0; i < (1 << numVariables); i++) {
        std::unordered_map<char, bool> currentValues;
        int mask = 1;
        for (char variable : variables) {
            currentValues[variable] = (i & mask) != 0;
            mask <<= 1;
        }

        // Evaluate the expression for the current variable values
        bool result = evaluateLogicalExpression(expression, currentValues);

        // Append the result to the vector
        truthTableCases.push_back(result);
    }

    // Return the vector containing all truth table cases
    return truthTableCases;
}

void generateTruthTable(const std::vector<bool>& results) {
    if (results.size() != 8) {
        std::cerr << "Error: The size of the results vector must be 8." << std::endl;
        return;
    }
    std::cout << "-------------------------------------------\n";
    std::cout << "| Input 3  | Input 2  | Input 1  | Output |\n";
    std::cout << "|----------|----------|----------|--------|\n";

    int resultIndex = 0;

    for (int a = 0; a <= 1; ++a) {
        for (int b = 0; b <= 1; ++b) {
            for (int c = 0; c <= 1; ++c) {
                // Print the row of the truth table with 1 or 0 for true or false
                std::cout << "|    " << a << "     |    " << b << "     |    " << c << "     |    " << static_cast<int>(results[resultIndex]) << "   |\n";
                resultIndex++;
            }
        }
    }
    std::cout << "-------------------------------------------\n\n";
}

bool CheckEquivalent(vector<bool> & truthTableCases,const vector<bool>& simplify_truthTableCases){
    return (truthTableCases == simplify_truthTableCases);
}

string replaceGate(string& expression, vector<bool> &truthTableCases) {
    string modifiedExpression = expression;
    size_t pos = 0;

    // Find and replace the gate in the expression

    while (pos != string::npos) {
        pos = modifiedExpression.find_first_of("!&|",pos+1);
        // Replace '!' with an empty string to remove it
        if (modifiedExpression[pos]== '!') {
            modifiedExpression.erase(pos, 1);
            if( CheckEquivalent(truthTableCases,generateTruthTableCases(modifiedExpression))) break;
            modifiedExpression = expression;
        }
            // Replace '&' with '|' and vice versa
        else if (modifiedExpression[pos] == '&' || modifiedExpression[pos] == '|') {
            char replacement = (modifiedExpression[pos] == '&') ? '|' : '&';
            modifiedExpression.replace(pos, 1, 1, replacement);
            if( CheckEquivalent(truthTableCases,generateTruthTableCases(modifiedExpression))) break;
            modifiedExpression = expression;
        }
    }

    return modifiedExpression;
}

void HowToUse() {
    // Welcome message and instructions for using the program
    cout << "\n\t\tWelcome to Digital Logic Circuit Simplifier\n";
    cout << "\nTo use this program, consider the following:\n"
         << "  - Use '&' for AND, '|' for OR, and '!' for NOT.\n"
         << "  - Pay attention to parentheses, e.g., (((!(A|!B)) & (A & B) ) & C) , (A | !(A))\n";
    // Additional information about input placement in the truth table
    cout << "If there is one input, it will be placed in the Input 1 column in the truth table.\n"
         << "If there are two inputs like A, B, then A will be in Input 2 and B will be in Input 1 (shifted left).\n\n";
    cout << "The logical expression for Team 26's circuit is (((!(A|!B)) & (A & B) ) & C)\n\n"

         << "What is your input expression? if you want an examples type 'yes' or 'YES' \n";
}
void findMatchingIndices(const std::vector<bool>& vector1, const std::vector<bool>& vector2) {
    std::vector<size_t> matchingIndices;

    // Ensure both vectors have the same size
    if (vector1.size() != vector2.size()) {
        std::cerr << "Error: Vectors must have the same size." << std::endl;

    }

    // Find matching indices
    for (size_t i = 0; i < vector1.size(); ++i) {
        if (vector1[i] == vector2[i]) {
            matchingIndices.push_back(i);
        }
    }
    std::cout << "the value of the inputs which makes each of the 2 logic circuit expression satisfiable :\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "|       input         |       output      |\n";
    std::cout << "|---------------------|-------------------|\n";

    for (size_t i : matchingIndices) {
        std::bitset<3> binaryRepresentation(i); // Convert index to binary with three bits
        std::cout << "|         " << binaryRepresentation << "         |        " << vector1 [i]<< "      |\n";
    }

    std::cout << "-------------------------------------------\n\n";
}