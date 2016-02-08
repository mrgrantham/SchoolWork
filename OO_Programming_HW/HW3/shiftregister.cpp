#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cstddef> 
#include <sstream>
#include <bitset>


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

    
protected:
    
    vector<bool> registerBits;
    char sVal;
    bool sizeSet;    //  Indicated if the -s was used yet so initial value command 
                    //  know whether it should determine register size or defer 
                    //  to existing constraints
};
 

enum printConfiguration { NoAddOn, HexAddon, DecAddon };

class RegisterOverload : public Register {
public:
    RegisterOverload operator+(const RegisterOverload& aRegister);
    RegisterOverload operator-(const RegisterOverload& aRegister);
    RegisterOverload();
    ~RegisterOverload();
    string valHex() const;
    int valDec() const;
    void setPrintSettings(int setting);
    int getPrintSettings() const;
    void setRegisterDec(int value);
    friend ostream &operator<<(ostream &output, const RegisterOverload& aRegister);
    int getSize() const;
    bool getBit(int index) const;
    void shiftRegSize(int size);
    void flip();

    
private:
    string binToHex();
    int binToDec();
    string hexVal;      // Hex value of register
    int Dec;            // Decimal value of register
    int printSettings;
    
    
};

RegisterOverload executeOperations(vector<char>& commands, vector<int>& parameters, RegisterOverload& aRegister, RegisterOverload& bRegister);

int myPower(int base, int exponent);

string removeSurroundingWhitespace(string aString);
string removeSurroundingWhitespace(const char* aString);
int cleanIntegerFromCharStar(const char* aCharStar);
bool logicalXOR(bool a, bool b);
bool halfAdder(bool a, bool b, bool cin);
bool fullAdder(bool a, bool b, bool cin);

int main(int argc, char** argv) {

    RegisterOverload aRegister;
    RegisterOverload bRegister;
    
    vector<int> parameters;
    vector<char> commands;
    
    extern char *optarg;
    extern int optind;
    
    enum operation { ADDITION, SUBTRACTION, NO_OPERATION };
    
    int command = 0, err = 0;
    
    if (argc < 2) {
        cerr << "Incorrect Format: This program requires at minimum one argument" << endl;
        return -1;
    }
    int operation = NO_OPERATION;
    bool PRINT_FLAG = false;
    bool BIN_FLAG1 = true;
    bool BIN_FLAG2 = true;
    
    while (( command = getopt( argc, argv, "I:S:R:L:i:s:r:l:v:po:dh")) != -1) {
        string initializingString1;
        string initializingString2;
        
        switch(command) {
            case 'i':
                // Sets the initial register value
                initializingString1 = string(removeSurroundingWhitespace(optarg));
                 if (initializingString1[initializingString1.size()-1] == 'D'){
                        BIN_FLAG1 = false;
                        initializingString1.erase(initializingString1.size()-1);
                    }
                    
                for(int i=0;i<initializingString1.size();i++){
                    
                    if (optarg[0] == '-'){
                        BIN_FLAG1 = false;
                    }
                    
                   
                    
                                        
                    if ((optarg[i]<'0' || optarg[i]>'9') && optarg[0] != '-'){
                        cerr << "Invalid initial register value. Please enter a valid number" << endl;
                        return -1;
                    }
                    
                    if(optarg[i]!='0' && optarg[i]!='1'){

                        BIN_FLAG1 = false;
                    }
                   
                }
                if(BIN_FLAG1){
                    aRegister.setRegister(initializingString1);
                } else {
                    int i;
                    istringstream(initializingString1) >> i;
                    aRegister.setRegisterDec(i);
                    
                }
                break;
            case 'l':
                //
                if(optarg[0] != '0' && cleanIntegerFromCharStar(optarg) == 0) {
                    cerr << "Invalid parameter passed for -l : Must be a valid number" << endl;
                    return -1;
                } else if(cleanIntegerFromCharStar(optarg) < 0){
                    cerr << "Invalid parameter passed for -l : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('l');
                parameters.push_back(cleanIntegerFromCharStar(optarg));
                break;
            case 'r':
                if(optarg[0] != '0' && cleanIntegerFromCharStar(optarg) == 0) {
                    cerr << "Invalid parameter passed for -r : Must be a valid number" << endl;
                    return -1;
                } else if(cleanIntegerFromCharStar(optarg) < 0){
                    cerr << "Invalid parameter passed for -r : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('r');
                parameters.push_back(cleanIntegerFromCharStar(optarg));
                break;   
            case 's':
                // Specifies the number of bits in the shift register
                int shiftBits ;
                shiftBits = cleanIntegerFromCharStar(optarg);
                if (shiftBits <= 0) {
                    cerr << "Error: please specify a register size greater than 0" << endl;
                    return -1;
                }
                aRegister.shiftRegSize(shiftBits);
                break;
            case 'I':
                // Sets the initial register value
                initializingString2 = string(removeSurroundingWhitespace(optarg));
                if (initializingString2[initializingString2.size()-1] == 'D'){
                        BIN_FLAG2 = false;
                        initializingString2.erase(initializingString2.size()-1);
                    }
                    
                    
                    
                for(int i=0;i<initializingString2.size();i++){
                    
                    if (optarg[0] == '-'){
                        BIN_FLAG2 = false;
                    }
                    
                    
                                        
                    if ((optarg[i]<'0' || optarg[i]>'9') && optarg[0] != '-'){
                        cerr << "Invalid initial register value. Please enter a valid number" << endl;
                        return -1;
                    }
                    
                    if(optarg[i]!='0' && optarg[i]!='1'){

                        BIN_FLAG2 = false;
                    }
                   
                }
                if(BIN_FLAG2){
                    bRegister.setRegister(initializingString2);
                } else {
                    int i;
                    istringstream(initializingString2) >> i;
                    bRegister.setRegisterDec(i);
                }
                break;
            case 'L':
                //
                if(optarg[0] != '0' && cleanIntegerFromCharStar(optarg) == 0) {
                    cerr << "Invalid parameter passed for -l : Must be a valid number" << endl;
                    return -1;
                } else if(cleanIntegerFromCharStar(optarg) < 0){
                    cerr << "Invalid parameter passed for -l : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('L');
                parameters.push_back(cleanIntegerFromCharStar(optarg));
                break;
            case 'R':
                if(optarg[0] != '0' && cleanIntegerFromCharStar(optarg) == 0) {
                    cerr << "Invalid parameter passed for -r : Must be a valid number" << endl;
                    return -1;
                } else if(cleanIntegerFromCharStar(optarg) < 0){
                    cerr << "Invalid parameter passed for -r : Must be a non-negative number" << endl;
                    return -1;
                }
                commands.push_back('R');
                parameters.push_back(cleanIntegerFromCharStar(optarg));
                break;   
            case 'S':
                // Specifies the number of bits in the shift register
                int shiftBits2 ;
                shiftBits2 = cleanIntegerFromCharStar(optarg);
                if (shiftBits2 <= 0) {
                    cerr << "Error: please specify a register size greater than 0" << endl;
                    return -1;
                }
                bRegister.shiftRegSize(shiftBits2);
                break;
            case 'p':
                // Sets print flag to indicate whether to print results at end
                PRINT_FLAG = true;
                break;
            case 'v':
                // Value to inject in vacated bit if other than default
                if((strlen(optarg) > 1) || (optarg[0] != '0' && optarg[0] != '1')){
                    cerr << "Invalid parameter: \"" << optarg << "\" given with -v argument" << endl << "Please enter 0 or 1 as parameter" << endl;
                    cerr << "Value must be 0 or 1" << endl;
                    return -1;
                }                           
                aRegister.setShiftValue(cleanIntegerFromCharStar(optarg));  
                bRegister.setShiftValue(cleanIntegerFromCharStar(optarg));               

                break;        
            case 'd':
                aRegister.setPrintSettings(DecAddon);
                bRegister.setPrintSettings(DecAddon);
                

                break;
            case 'h':
                aRegister.setPrintSettings(HexAddon);
                bRegister.setPrintSettings(HexAddon);
         

                break;
                
            case 'o':
                commands.push_back('o');
                if(*optarg == '-'){
                    operation = SUBTRACTION;
                    parameters.push_back(1);

                } else if(*optarg == '+'){
                    operation = ADDITION;
                    parameters.push_back(0);

                } else {
                    cerr << "Invalid operator passed as parameter. Program will only accept '-' or '+'" << endl;
                    return -1;
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
    
    RegisterOverload cRegister = executeOperations(commands, parameters, aRegister, bRegister);
    
    if(PRINT_FLAG){
        cout << aRegister << endl;
        cout << bRegister << endl;
        if(operation != NO_OPERATION){
            cout << cRegister << endl;
        }
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



RegisterOverload executeOperations(vector<char>& commands, vector<int>& parameters, RegisterOverload& aRegister, RegisterOverload& bRegister) {
    
    RegisterOverload newRegister;
    
    for (int i = 0; i < commands.size(); i++) {
        switch(commands[i]) {
            case 'l':
                aRegister.shiftLeft(parameters[i]);
                break;
            case 'r':
                aRegister.shiftRight(parameters[i]);
                break;
            case 'L':
                bRegister.shiftLeft(parameters[i]);
                break;
            case 'R':
                bRegister.shiftRight(parameters[i]);
                break;
            case 'o':
                if(parameters[i] == 1)
                    newRegister = aRegister - bRegister;
                if(parameters[i] == 0)
                    newRegister = aRegister + bRegister;
                break;
            default:
                cerr << "Invalid Command"<< endl;
                
        }
        
    }
    
    return newRegister;
}

int myPower(int base, int exponent){
    int power = base;
    if(exponent == 0){
        return 1;
    } else {
        for (int j = 1;j < exponent ; j++){
            power = 2 * power; 
        }
    }
    return power;
}

string removeSurroundingWhitespace(string aString){
    
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

string removeSurroundingWhitespace(const char* aString){
    return removeSurroundingWhitespace(string(aString));
    
}

int cleanIntegerFromCharStar(const char* aCharStar){
    int i;
    istringstream(removeSurroundingWhitespace(optarg)) >> i;
    return i;
}

bool logicalXOR(bool a, bool b){
    return !((a && b) || (!a && !b));
}

bool halfAdderSum(bool a, bool b){
    return logicalXOR(a,b);
}

bool halfAdderCout(bool a, bool b){
    return (a && b);
}

bool fullAdderSum(bool a, bool b, bool cin){
    return halfAdderSum(cin,halfAdderSum(a,b));
}

bool fullAdderCout(bool a, bool b, bool cin){
    return halfAdderCout(cin,halfAdderSum(a,b)) || halfAdderCout(a,b);
}

// RegisterOverload Class

    RegisterOverload RegisterOverload::operator+(const RegisterOverload& aRegister){
        RegisterOverload newRegister;
        
        vector<bool> aRegisterBits = aRegister.registerBits;
        vector<bool> bRegisterBits = registerBits;

        newRegister.setPrintSettings(printSettings);
        
        // Resize and fill added locations with most significant bit to preserve sign
        if(aRegisterBits.size() > bRegisterBits.size()){
            newRegister.shiftRegSize(aRegisterBits.size());
            bRegisterBits.resize(aRegisterBits.size(),bRegisterBits[bRegisterBits.size()-1]);
            
        } else {
            newRegister.shiftRegSize(bRegisterBits.size());
            aRegisterBits.resize(bRegisterBits.size(),aRegisterBits[aRegisterBits.size()-1]);
        }
        
        vector<bool> carryBits(newRegister.registerBits.size(),false);
        
        
        for(int i=0; i < newRegister.registerBits.size(); i++) {
            newRegister.registerBits[i] = fullAdderSum(aRegisterBits[i], bRegisterBits[i], carryBits[i]);
            if((i + 1) < carryBits.size()){
                carryBits[i+1] = fullAdderCout(aRegisterBits[i],bRegisterBits[i],carryBits[i]);
            }
        }
        
        return newRegister;
    }
    
    RegisterOverload RegisterOverload::operator-(const RegisterOverload& aRegister){
        
        RegisterOverload holdRegister;
        holdRegister = aRegister;
        RegisterOverload newRegister;

        RegisterOverload onesRegister;
        // If negative
        if(aRegister.registerBits[aRegister.registerBits.size()-1]){
            onesRegister.setRegisterDec(-1);
            newRegister = holdRegister + onesRegister;
            newRegister.flip();
        } else { // If positive
            holdRegister.flip(); 
            onesRegister.setRegisterDec(1);
            newRegister = holdRegister + onesRegister;
        }

        newRegister = *this + newRegister;
        
        
        
        return newRegister;
    }
    
    ostream &operator<<(ostream &output, const RegisterOverload &aRegister){
        for(int i = aRegister.getSize()-1;i >= 0; i--){
            if(aRegister.getBit(i)){
                output << "1";
            } else {
                output << "0";
            }
        }
        if(aRegister.getPrintSettings() == DecAddon){
            output << "(" << aRegister.valDec() << ")";
        }
        if(aRegister.getPrintSettings() == HexAddon){
            string hexItem = aRegister.valHex();
            output << "(" << hexItem << ")";
        }
        return output;
    }

    
    RegisterOverload::RegisterOverload(){
        printSettings = NoAddOn;
    }
    
    RegisterOverload::~RegisterOverload(){
        
    }
    
    string RegisterOverload::valHex() const{
        string hexVal = "";
        
        string temp;
        for(int i =0;i<=registerBits.size();i++){
            if(temp.size() == 4){
                
                if(i != 0){
                    
                    if(temp == "0000")
                        hexVal = "0" + hexVal;
                    if(temp == "0001")
                        hexVal = "1" + hexVal;
                    if(temp == "0010")
                        hexVal = "2" + hexVal;                    
                    if(temp == "0011")
                        hexVal = "3" + hexVal;                    
                    if(temp == "0100")
                        hexVal = "4" + hexVal;
                    if(temp == "0101")
                        hexVal = "5" + hexVal;
                    if(temp == "0110")
                        hexVal = "6" + hexVal;
                    if(temp == "0111")
                        hexVal = "7" + hexVal;
                    if(temp == "1000")
                        hexVal = "8" + hexVal;
                    if(temp == "1001")
                        hexVal = "9" + hexVal;
                    if(temp == "1010")
                        hexVal = "A" + hexVal;
                    if(temp == "1011")
                        hexVal = "B" + hexVal;
                    if(temp == "1100")
                        hexVal = "C" + hexVal;
                    if(temp == "1101")
                        hexVal = "D" + hexVal;
                    if(temp == "1110")
                        hexVal = "E" + hexVal;
                    if(temp == "1111")
                        hexVal = "F" + hexVal;
                    

                    temp = "";
                }
                
            }
            if(registerBits[i]){
                temp = "1" + temp;
            } else {
                temp = "0" + temp;
            }
            if(i == (registerBits.size()-1)){
                int zerosToAppend=0;
                if((4-((i+1) % 4))!= 4){
                    zerosToAppend = 4-((i+1)%4);
                }


                for(int j = 1; j <= zerosToAppend; j++){
                    temp = "0" + temp;
                }
                
            }


        }
        if(hexVal == "")
            hexVal = hexVal + "0x0";
        else
            hexVal = "0x" + hexVal;
        
        return hexVal;
    }
    
    int RegisterOverload::valDec() const {
        int temp=0;
        for(int i = 0;i < registerBits.size()-1; i++){
            if(registerBits[i]){
                temp = myPower(2,i) + temp;
                
            }
        }
        
        if(registerBits[registerBits.size()-1]){
            
            temp = (myPower(2,registerBits.size()-1) - temp) * -1;
            
        }
        
        return temp;
    }
    
    int RegisterOverload::getPrintSettings() const {
            
        return printSettings;
        
    }

    void RegisterOverload::setPrintSettings(int setting) {
        printSettings = setting;
    }
    
    void RegisterOverload::setRegisterDec(int value){
        string binValue = "";
        string sign;
        if(value < 0)
            sign = "1";
        else
            sign = "0";
        int temp=abs(value);
        
        int power = 0;
        
        
        while(myPower(2,power)<=temp){
            power++;
        }
        power--;
        
        while (power >= 0){

            if(temp>=myPower(2,power)){
                binValue = binValue + "1";
                temp = temp-myPower(2,power);
            } else {
                binValue = binValue + "0";
            }
            
            power--;
        }
        
        
        if(sign == "1"){
            
            if((myPower(2,binValue.size()-1) + value) != 0) {
            
                // Add a digit to handle negative sign if not a direct power of 2
                binValue.resize(binValue.size() + 1 );
                for(int index = binValue.size()-1;index > 0; index--){
                    binValue[index]=binValue[index-1];
                }
                binValue[0] = '0';

                // Flip the bits
                for(int i = 0;i < binValue.size();i++){
                    if(binValue[i] == '1'){
                        binValue[i] = '0';
                    } else {
                        binValue[i] = '1';
                    }
                }
                
                // Add 1
                char carry = '0';

                for(int i = binValue.size()-1;i >= 0;i--){

                    if(i == binValue.size()-1) {
                        if(binValue[i] == '1'){
                            carry = '1';
                            binValue[i] = '0';
                        } else {
                            binValue[i] = '1';
                        }
                    } else if (!(binValue[i] == '1') != !(carry == '1')){
                        binValue[i] = '1';
                        carry = '0';
                    } else if (binValue[i] == '1' && carry == '1'){
                        binValue[i] = '0';
                        carry = '1';
                    }
                }
            }
        } 
             
        
        
        if (sign == "0"){
            binValue.resize(binValue.size() + 1 );
            for(int index = binValue.size()-1;index > 0; index--){
                binValue[index]=binValue[index-1];
            }
            binValue[0] = '0';
        }

        if(!sizeSet){ 
            if(sign == "0"){
                if(binValue[binValue.size()-1] == 1){
                    registerBits.resize(binValue.size()+1);
                    binValue = sign + binValue;

                } else {
                    registerBits.resize(binValue.size());
                }

            }
            if(sign == "1"){
                registerBits.resize(binValue.size());

            }
        } else { // Size is explicitly stated so value must be truncated if too long
            if(registerBits.size()<binValue.size()){
                
                binValue.resize(registerBits.size());
                binValue[0] = sign.c_str()[0];
            }
            if(registerBits.size()>binValue.size()){
                if(sign == "1"){
                    int diff = registerBits.size() - binValue.size();
                    binValue.resize(diff);
                    for(int k = binValue.size();k>diff;k--){
                        binValue[k] = binValue[k-diff];
                    }
                    for(int k = 0;k < diff; k++){
                        binValue[k] = '1';
                    }
                }
            }
        }
        
        
        this->setRegister(binValue);
        
        
    }
    
    int RegisterOverload::getSize() const {
        return registerBits.size();
    }
    
    
    bool RegisterOverload::getBit(int index) const {
        return registerBits[index];
    }
    
    void RegisterOverload::shiftRegSize(int size){
    registerBits.resize(size,registerBits[registerBits.size()-1]);
    sizeSet = true;
}
    
        
    void RegisterOverload::flip(){
        for(int i = 0; i < registerBits.size(); i ++){
            if(registerBits[i]){
                registerBits[i] = false;
            } else {
                registerBits[i] = true;
            }
        }
    }
