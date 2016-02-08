// CMPE 180-92
// Homework 5 

// TODO
// get rid of white spaces and handle "10+" or "1/0,10" circumstances


#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <stack>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <sstream>


using namespace std;

string remove_whitespace(string aString);
string remove_whitespace(const char* aString);

class Stack_Calculator {
public:
    Stack_Calculator(stack<string> a_stack);
    void Set_stack(stack<string> a_stack);
    bool calculate();
    void print_result();
    friend ostream &operator<<(ostream &output, const Stack_Calculator& calculator);

protected:
    stack<string> data_stack;
    int result;
};

stack<string> parse_string(string parsable_string, string dilimiter);
void print_stack(stack<string> a_stack);

int main(int argc, char** argv) {

    stack<string> arithmetic_stack; 
    
    extern char *optarg;
    extern int optind;
    
    
    int command = 0, err = 0;
    
    if (argc < 3) {
        cerr << "Incorrect Format: This program requires one argument" << endl;
        return -1;
    }
    
    if (argc > 3) {
        cerr << "Incorrect Format: Too many arguments" << endl;
        return -1;
    }
    
    string stack_string; // This string will hold the original unparsed parameter
    
    while (( command = getopt( argc, argv, "e:")) != -1) {
        
        switch(command) {
            case 'e':
                // load the stack with components
                stack_string = string(optarg);

                for(int i=0;i<stack_string.size();i++){
                       
                    if ((optarg[i]<'0' || optarg[i]>'9') && optarg[i] != '-' && optarg[i] != '+' && optarg[i] != '/' && optarg[i] != '*' && optarg[i] != ',' && optarg[i] != ' '){
                        cerr << "Invalid format" << endl;
                        return -1;
                    }
                    
                }
                break;
            case '?':
                err = 1;
                break;
            default:
                err = 1;
                break;
        }
    }
    
    if (err) {
        cerr << "Unknown or incomplete command line argument(s) passed to program" << endl;
        return -1;
    }
    
    arithmetic_stack = parse_string(stack_string,",");
    
    Stack_Calculator calculator(arithmetic_stack);
    if(!calculator.calculate())
        return -1;
    
    cout << calculator;
    
    return 0;
}

string remove_whitespace(string aString){
    
    string whitespaces(" \t\f\v\n\r");
    
    size_t found_location = aString.find_last_not_of(whitespaces);
    if (found_location != string::npos){
        aString.erase(found_location + 1);
        
    }
    
    found_location = string::npos;
    
    found_location = aString.find_first_not_of(" \t\f\v\n\r");
    
    if (found_location != string::npos){
        aString.erase(0,found_location);
        
    }
    
    return aString;
}

string remove_whitespace(const char* aString){
    return remove_whitespace(string(aString));
    
}


stack<string> parse_string(string parsable_string, string delimiter) {
    stack<string> parsed_stack;
    stack<string> reversed_stack;
    size_t pos = 0;
    
    while((pos = parsable_string.find(delimiter)) != string::npos) {
        parsed_stack.push(parsable_string.substr(0,pos));
        parsable_string.erase(0, pos + delimiter.length());
    }
    
    parsed_stack.push(parsable_string);         // Push the last item after all delimiters are gone

    while(!parsed_stack.empty()){
        reversed_stack.push(parsed_stack.top());
        parsed_stack.pop();
    }
    
    return reversed_stack;
}

void print_stack(stack<string> a_stack) {
    int i;
    
    cout << endl << "STACK BEGIN" << endl << "----------" << endl;
    while(!a_stack.empty()){
        cout << a_stack.top() << endl;
        a_stack.pop();
    }
    cout << "-----------" << endl << endl;
}

Stack_Calculator::Stack_Calculator(stack<string> a_stack) {
    data_stack = a_stack;
}
void Stack_Calculator::Set_stack(stack<string> a_stack){
    data_stack = a_stack;
}
bool Stack_Calculator::calculate(){
    
    stack<int> operands;
    int local_result;
    stringstream op_converter;
    while(!data_stack.empty()){
        
        local_result = 0; 
        
        bool ADDITON_OP = data_stack.top().compare("+") == 0;
        bool SUBTRAC_OP = data_stack.top().compare("+") == 0;
        bool MULTIPL_OP = data_stack.top().compare("*") == 0;
        bool DIVIDE__OP = data_stack.top().compare("/") == 0;
        
        if(ADDITON_OP || SUBTRAC_OP || MULTIPL_OP || DIVIDE__OP){
            
            char the_operator = data_stack.top()[0];
            
            // Calculate
            if(operands.empty()) {
                cerr << "Not enough operands to complete calculation" << endl;
                return 0;
            }
                
            int op2 = operands.top();
            operands.pop();
            
            if(operands.empty()) {
                cerr << "Not enough operands to complete calculation" << endl;
                return 0;
            }
            
            int op1 = operands.top();
            operands.pop();

            // Add
            if (the_operator == '+')
                local_result = op1 + op2;
            
            // Subtract
            if (the_operator == '-')
                local_result = op1 - op2;
            
            // Multiply
            if (the_operator == '*')
                local_result = op1 * op2;
            
            // Divide
            if (the_operator == '/')
                local_result = op1 / op2;
            
            operands.push(local_result);
            
        } else {
            int op;
            op_converter.clear();
            op_converter.str(data_stack.top());

            op_converter >> op;
            operands.push(op);
        }
        
        data_stack.pop();
    }
    if(operands.size() > 1){
        cerr << "Not enough operators to complete calculations" << endl;
        return 0;
    } else {
        result = operands.top();
    }
    return 1;
}
void Stack_Calculator::print_result(){
    cout << result << endl;
}

ostream &operator<<(ostream &output, const Stack_Calculator &calculator){
    output << calculator.result;
    return output;
}