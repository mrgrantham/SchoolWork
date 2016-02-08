#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdlib.h>
#include <cstring>

using namespace std;

class Register {
public:
    
    void shiftRight(int shiftAmount);
    void shiftLeft(int shiftAmount);
    
    // This is the value that will be shifted into the register 
    // When a shiftLeft() or shiftRight() command is executed)
    void setShiftValue(char sVal);
    
    // This will print the value of the register using cout command
    void printValue();
    
    // This sets the size of the shift register, if larger than current values 
    // it will fill the new allocated space with 0's
    void shiftRegSize(int size);
    
    // This will use the value from the -i argument to set the register value
    void setRegister(string value); 
    Register();
    
private:
    
    vector<bool> registerBits;
    char sVal;
    bool sizeSet;    //  Indicated if the -s was used yet so initial value command 
                    //  know whether it should determine register size or defer 
                    //  to existing constraints
};

bool excecuteOperations(vector<char>& commands, vector<int>& parameters, Register& aRegister);


int main(int argc, char** argv) {

    Register aRegister;
    vector<int> parameters;
    vector<char> commands;
    
    extern char *optarg;
    extern int optind;
    
    int command = 0, err = 0;
    
    if (argc < 2) {
        cerr << "Incorrect Format: This program requires at minimum one argument" << endl;
        return -1;
    }
    
    bool PRINT_FLAG = false;
    
        
    while (( command = getopt( argc, argv, "i:s:r:l:v:p")) != -1) {
        switch(command) {
            case 'i':
                // Sets the initial register value
                for(int i=0;i<string(optarg).size();i++){
                    if(optarg[i]!='0' && optarg[i]!='1'){
                        cerr << "Invalid initial register value. Please enter only an combination of 0's and 1's" << endl;
                        return -1;
                    }
                }
                aRegister.setRegister(string(optarg));
                break;
            case 'p':
                // Sets print flag to indicate whether to print results at end
                PRINT_FLAG = true;
                break;
            case 'l':
                //
                if(optarg[0] != '0' && atoi(optarg) == 0) {
                    cerr << "Invalid parameter passed for -l : Must be a valid number" << endl;
                    return -1;
                } else if(atoi(optarg) < 0){
                    cerr << "Invalid parameter passed for -l : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('l');
                parameters.push_back(atoi(optarg));
                break;
            case 'r':
                if(optarg[0] != '0' && atoi(optarg) == 0) {
                    cerr << "Invalid parameter passed for -r : Must be a valid number" << endl;
                    return -1;
                } else if(atoi(optarg) < 0){
                    cerr << "Invalid parameter passed for -r : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('r');
                parameters.push_back(atoi(optarg));
                break;   
            case 's':
                // Specifies the number of bits in the shift register
                int shiftBits ;
                shiftBits = atoi(optarg);
                if (shiftBits <= 0) {
                    cerr << "Error: please specify a register size greater than 0" << endl;
                    return -1;
                }
                aRegister.shiftRegSize(shiftBits);
                break;
            case 'v':
                // Value to inject in vacated bit if other than default
                if((strlen(optarg) > 1) || (optarg[0] != '0' && optarg[0] != '1')){
                    cerr << "Invalid parameter: \"" << optarg << "\" given with -v argument" << endl << "Please enter 0 or 1 as parameter" << endl;
                    cerr << "Value must be 0 or 1" << endl;
                    return -1;
                }                           
                aRegister.setShiftValue(atoi(optarg));               
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
    
    if(!excecuteOperations(commands, parameters, aRegister)) {
        cerr << "Error executing shift operations" << endl;
        return -1;
    }
    
    if(PRINT_FLAG){
        aRegister.printValue();
    }
    
    return 0;
}

void Register::shiftRight(int shiftAmount){
    for(int i=0; i < shiftAmount;i++){
            for(vector<bool>::iterator it = registerBits.begin(); it != registerBits.end(); ++it){
                vector<bool>::iterator next = it + 1;

                if(next != registerBits.end()){
                    *it = *next;
                } else
                    *it = sVal;
            }
    }

}

void Register::shiftLeft(int shiftAmount){
    for(int i=0; i < shiftAmount;i++){
            for(vector<bool>::reverse_iterator it = registerBits.rbegin(); it != registerBits.rend(); ++it){
                vector<bool>::reverse_iterator next = it + 1;

                if(next != registerBits.rend()){
                    *it = *next;
                } else
                    *it = sVal;
            }
    }
}

void Register::setShiftValue(char sVal){
    this->sVal = sVal;
}

void Register::printValue(){
    for(vector<bool>::reverse_iterator it = registerBits.rbegin(); it != registerBits.rend(); ++it){
        cout << +*it ;
    }
    cout << endl;
    
}

void Register::shiftRegSize(int size){
    registerBits.resize(size,0);
    sizeSet = true;
}


void Register::setRegister(string strValue) { // This will use the value from the -i argument
    if(!sizeSet){
        registerBits.resize(strValue.size());
    }
    
    int str;
    int reg;
    // If string has more locations than the set 
    // cut off the most significant bits to fit in the register space
    if(strValue.size() > registerBits.size() ){
        for(str=registerBits.size()-1, reg=0; str >= 0; str--,reg++ ) {
            registerBits[reg] = strValue[str]-'0';
        }
    } else {
        for(str=strValue.size()-1, reg=0; str >= 0; str--,reg++ ) {
            registerBits[reg] = strValue[str]-'0';
        }
    }

}

Register::Register(){
    sVal = 0;
    sizeSet = false;
    registerBits = vector<bool>(1,0);
}

bool excecuteOperations(vector<char>& commands, vector<int>& parameters, Register& aRegister) {
        
    for (int i = 0; i < commands.size(); i++) {
        switch(commands[i]) {
            case 'l':
                aRegister.shiftLeft(parameters[i]);
                break;
            case 'r':
                aRegister.shiftRight(parameters[i]);
                break;   
            default:
                return false;
                
        }
        
    }
    return true;
}
