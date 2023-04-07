//Balaga Pavan Sai
//21114025 CSE 2nd Year IITR
#include<bits/stdc++.h>
using namespace std;
void regtab_init();
//structs
struct optab_struct{
int format;
string opcode;
bool exists;
optab_struct(){
    format=-1;
    opcode="FF";
    exists=false;
}
};
struct symtab_struct{
    int address;
    bool exists;
    bool isRelative;
    symtab_struct(){
        address=-3;
        exists=false;
        isRelative=false;
    }
};
struct REG_TAB_struct{
    int value;
    bool exists;
    char number;
    REG_TAB_struct(){
        value=0;
        number=-1;
        exists=false;
    }
};
struct littab2_struct{
    string value;//in hex
    int address;
    bool write;
    bool exists;
    littab2_struct(){
        address=0;
        value="";
        write=false;
        exists=false;
    }
};
struct extdef_struct{
  string name;
  string address;
  bool exists;
  extdef_struct()
  {
    name = "undefined";
    address = "0";
    exists = false;
  }
};
struct extref_struct{
  string name;
  bool exists;
  extref_struct(){
    name = "undefined";
    exists = false;
  }
};
struct csect_struct{    
  string name;
  int LOCCTR;
  int section_number;
  int length;
  stack<int> ORG_STACK;
  int start;
  string base;
  map<string, extdef_struct> EXTDEF_TAB;
  map<string, extref_struct> EXTREF_TAB;
  map<string, symtab_struct> SYM_TAB;
  map<string,littab2_struct> LIT_TAB;
  csect_struct()
  { regtab_init();
    name = "DEFAULT";
    LOCCTR = 0;
    section_number = 0;
    length = 0;
    start=0;
    base="";
  }
};
//Global shared data
map<string,optab_struct> OP_TAB;
map<string,REG_TAB_struct> REG_TAB;
map<string,csect_struct> CSECT;
map<string,bool> ASM_DIR;
//utility
void changeString(string& s,int size){
    if(s.length()>size){
        s=s.substr(s.length()-size,size);
    }else {
        string temp="";
        for(int i=1;i<=(size-s.length());i++){
            temp+="0";
        }
        temp+=s;
        s=temp;
    }
}
int convertHexToDec(string hexString){
  stringstream stream;
  int dec;
  stream << hexString;
  stream >> hex >> dec;
  return dec;
}
int convertStringToInt(string str){
    int value;
    stringstream(str) >> value;
    return value;
}
string convertDecToHex(int decimal){
    stringstream stream2;
    stream2 << hex << decimal;
    string result(stream2.str());
    char resultUpper[10];
    strcpy(resultUpper, result.c_str());
    for (int i = 0 ; i < strlen(resultUpper) ; i++){
        resultUpper[i] = toupper(resultUpper[i]);
    }
    return resultUpper;
}
string convertHexToBin(string text){
    int no = 0;
    string binaryResult = "";
    for (int i = 0 ; i < text.length() ; i++){
        string temp = "";
        if (text[i] >= '0' && text[i] <= '9'){
            no = int(text[i]) - 48;
            while (no > 0){
                temp += char(no % 2 + 48);
                no /= 2;
            }
            while (temp.length() != 4){
                temp += '0';
            }
            no = 0;
        }
        else {
            no = text[i] - 'A' + 10;
            while (no > 0){
                temp += char(no % 2 + 48);
                no /= 2;
            }
        }
        for (int j = 3 ; j >= 0 ; j--){
            binaryResult += temp[j];
        }
    }
    return binaryResult;
}
string convertBinToHex(string binText){
    stringstream res;
    string binary_str(binText);
    bitset<12> set(binary_str);
    res << hex << uppercase << set.to_ulong();
    return res.str();
}
bool all_num(string x)
{
  bool iall_num = true;
  int i = 0;
  while (all_num && (i < x.length()))
  {
    iall_num &= isdigit(x[i++]);
  }
  return iall_num;
}
void eval_operand(string s,int& abs,bool& error,ofstream& ferror,int& temp_value,string csect_name,int line){
    string str="",operand;temp_value=0;
    if(s[0]=='X'){//If value is given in hexadecimal 
        int ind=1;
        while(s[ind]!='\'')ind++;ind++;
        while(s[ind]!=' ')str+=s[ind++];
        temp_value=convertHexToDec(str);
    }else{
        stringstream ss(s);
        ss>>operand;
        if(all_num(operand)){
            temp_value+=stoi(operand);
        }else if(CSECT[csect_name].SYM_TAB[operand].exists){
            temp_value+=CSECT[csect_name].SYM_TAB[operand].address;abs++;
        }else{
            error=true;abs+=1000;
            ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
            //cout<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
        }
        while(ss>>operand){
            char oprtr=operand[0];operand=operand.substr(1,operand.length()-1);
            switch(oprtr){
                case '+':   
                if(all_num(operand)){
                    temp_value+=stoi(operand);
                }else if(CSECT[csect_name].SYM_TAB[operand].exists){
                    temp_value+=CSECT[csect_name].SYM_TAB[operand].address;
                    abs++;
                }else{error=true;
                    ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
                    //cout<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
                }
                break;
                case '-':
                if(all_num(operand)){
                    temp_value-=stoi(operand);
                }else if(CSECT[csect_name].SYM_TAB[operand].exists){
                    temp_value-=CSECT[csect_name].SYM_TAB[operand].address;
                    abs--;
                }else{error=true;
                    ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
                    //cout<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined in symbol table\n";
                }
                break;
                case '*':
                case '/':
                error=true;
                ferror<<"ERROR: LINE"<<line<<" "<<oprtr<<" is not usable in expression\n";
                //cout<<"ERROR: LINE"<<line<<" "<<oprtr<<" is not usable in expression\n";
                break;
                default:
                error=true;
                ferror<<"ERROR: LINE"<<line<<" "<<oprtr<<" is not defined\n";
                //cout<<"ERROR: LINE"<<line<<" "<<oprtr<<" is not defined\n";
                break;
            }
        }
    }
}

string eval_expression(string str,bool& indexed,bool& error, ofstream& ferror,string csect_name, int line, int LOC,vector<string>& M,string tm,int& rel,int& tempval){
    string res="";rel=0;tempval=0;
    string operand;bool ext=false; indexed=false;string operand1;
    stringstream ss(str);
    string tempM="M";
    string t88="";
    if(tm=="06")
    t88=convertDecToHex(LOC);
    else if(tm=="05" || tm=="03")    
    t88=convertDecToHex(LOC+1);
    else    
    t88=convertDecToHex(LOC);
    changeString(t88,6);
    tempM+=t88;tempM+=tm;
    ss>>operand;
    operand1=operand;
    if(operand[0]=='#' || operand[0]=='@')operand=operand.substr(1,operand.length()-1);
    if(all_num(operand)){
        tempval+=stoi(operand);
    }else if(CSECT[csect_name].SYM_TAB[operand].exists){
        tempval+=CSECT[csect_name].SYM_TAB[operand].address;
        if(CSECT[csect_name].SYM_TAB[operand].isRelative)
        rel++;
                
    }else if(CSECT[csect_name].EXTREF_TAB[operand].exists){
        //add modification record
        ext=true;
        string sir=operand;
        for(int i=1;i<=6-operand.length();i++)sir+=" ";
        string str=tempM+"+"+sir;
        M.push_back(str);
    }else if(operand[0]=='*' && tm=="06"){
        //locctr
        res=t88;
    }else{
        error=true;rel+=1000;
        ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined \n";   
    }
        
    while(ss>>operand){
        char oprtr=operand[0];operand=operand.substr(1,operand.length()-1);
        switch(oprtr){
            case '+':            
                if(all_num(operand)){
                    tempval+=stoi(operand);
                }else if(CSECT[csect_name].SYM_TAB[operand].exists){
                    tempval+=CSECT[csect_name].SYM_TAB[operand].address;
                    if(CSECT[csect_name].SYM_TAB[operand].isRelative)
                    rel++;
                }else if(CSECT[csect_name].EXTREF_TAB[operand].exists){
                    //add modification record
                    ext=true;
                    string sir=operand;
                    for(int i=1;i<=6-operand.length();i++)sir+=" ";
                    string str=tempM+"+"+sir;
                    M.push_back(str);
                }else{
                    error=true;rel+=1000;
                    ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined \n";   
                }
            break;
            case '-':            
                if(all_num(operand)){
                    tempval-=stoi(operand);
                }else if(CSECT[csect_name].SYM_TAB[operand].exists){
                    tempval-=CSECT[csect_name].SYM_TAB[operand].address;
                    if(CSECT[csect_name].SYM_TAB[operand].isRelative)
                    rel--;
                }else if(CSECT[csect_name].EXTREF_TAB[operand].exists){
                    //add modification record
                    ext=true;
                    string sir=operand;
                    for(int i=1;i<=6-operand.length();i++)sir+=" ";
                    string str=tempM+"-"+sir;
                    M.push_back(str);
                }else{
                    error=true;rel+=1000;
                    ferror<<"ERROR: LINE"<<line<<" "<<operand<<" is not defined \n";   
                }
            break;
            case ',':
            if(operand[0]=='X')indexed=true;
            //did not expect ,
            break;
            case '*':
            case '/':
            error=true;
            ferror<<"ERROR LINE:"<<line<<" "<<oprtr<<"is not usable in the expression\n";
            break;
            default:
            error=true;
            ferror<<"ERROR: LINE"<<line<<" "<<oprtr<<" is not defined\n";
            break;
        }
    }
    
    if(!ext && rel==1){
        string sir=CSECT[csect_name].name;
        for(int i=1;i<=6-operand.length();i++)sir+=" ";
        string str=tempM+"+"+sir;
        if(tm=="05")
        M.push_back(str);
    }else if(!ext && rel==-1){
        string sir=CSECT[csect_name].name;
        for(int i=1;i<=6-operand.length();i++)sir+=" ";
        string str=tempM+"-"+sir;
        if(tm=="05")
        M.push_back(str);
    } else if(!ext && abs(rel)>1){
        error=true;
        ferror<<"ERROR LINE:"<<line<<" Invalid expression \n";
    }
    res=convertDecToHex(tempval);
    return res;
}
string convertQuote(string s,int line, bool& error, ofstream& ferror){
    string str="";
    if(s[0]=='C'){
        int index=2;
        string val="",val637="";
        int ch637;
        while(index<s.length() && s[index]!='\'' && s[index]!=' '){
            val637+=s[index];
            ch637=(int)s[index];
            string te640=convertDecToHex(ch637);
            changeString(te640,2);
            val+=te640;
            index++;
        }
        str=val;
    }else if(s[0]=='X'){
        int index=2;
        while(index<s.length() && s[index]!='\'' && s[index]!=' ')str+=s[index++];
    }else{
       error=true;
       ferror<<"ERROR LINE:"<<line<<" undefined literal type\n";
       //cout<<"ERROR LINE:"<<line<<" undefined literal type\n";
    }
    return str;
}
//initializer
void asmdir_init(){
    ASM_DIR["BASE"]=true;
    ASM_DIR["EQU"]=true; 
    ASM_DIR["END"]=true;   
    ASM_DIR["LTORG"]=true;
    ASM_DIR["EXTDEF"]=true;
    ASM_DIR["EXTREF"]=true;
    ASM_DIR["RESB"]=true;
    ASM_DIR["RESW"]=true;
    ASM_DIR["WORD"]=true;
    ASM_DIR["BYTE"]=true;
    ASM_DIR["ORG"]=true;
    ASM_DIR["NOBASE"]=true;
}
void optab_init(){
  OP_TAB["ADD"].opcode = "18";
  OP_TAB["ADD"].format = 3;
  OP_TAB["ADD"].exists = true;

  OP_TAB["ADDF"].opcode = "58";
  OP_TAB["ADDF"].format = 3;
  OP_TAB["ADDF"].exists = true;

  OP_TAB["ADDR"].opcode = "90";
  OP_TAB["ADDR"].format = 2;
  OP_TAB["ADDR"].exists = true;

  OP_TAB["AND"].opcode = "40";
  OP_TAB["AND"].format = 3;
  OP_TAB["AND"].exists = true;

  OP_TAB["CLEAR"].opcode = "B4";
  OP_TAB["CLEAR"].format = 2;
  OP_TAB["CLEAR"].exists = true;

  OP_TAB["COMP"].opcode = "28";
  OP_TAB["COMP"].format = 3;
  OP_TAB["COMP"].exists = true;

  OP_TAB["COMPF"].opcode = "88";
  OP_TAB["COMPF"].format = 3;
  OP_TAB["COMPF"].exists = true;

  OP_TAB["COMPR"].opcode = "A0";
  OP_TAB["COMPR"].format = 2;
  OP_TAB["COMPR"].exists = true;

  OP_TAB["DIV"].opcode = "24";
  OP_TAB["DIV"].format = 3;
  OP_TAB["DIV"].exists = true;

  OP_TAB["DIVF"].opcode = "64";
  OP_TAB["DIVF"].format = 3;
  OP_TAB["DIVF"].exists = true;

  OP_TAB["DIVR"].opcode = "9C";
  OP_TAB["DIVR"].format = 2;
  OP_TAB["DIVR"].exists = true;

  OP_TAB["FIX"].opcode = "C4";
  OP_TAB["FIX"].format = 1;
  OP_TAB["FIX"].exists = true;

  OP_TAB["FLOAT"].opcode = "C0";
  OP_TAB["FLOAT"].format = 1;
  OP_TAB["FLOAT"].exists = true;

  OP_TAB["HIO"].opcode = "F4";
  OP_TAB["HIO"].format = 1;
  OP_TAB["HIO"].exists = true;

  OP_TAB["J"].opcode = "3C";
  OP_TAB["J"].format = 3;
  OP_TAB["J"].exists = true;

  OP_TAB["JEQ"].opcode = "30";
  OP_TAB["JEQ"].format = 3;
  OP_TAB["JEQ"].exists = true;

  OP_TAB["JGT"].opcode = "34";
  OP_TAB["JGT"].format = 3;
  OP_TAB["JGT"].exists = true;

  OP_TAB["JLT"].opcode = "38";
  OP_TAB["JLT"].format = 3;
  OP_TAB["JLT"].exists = true;

  OP_TAB["JSUB"].opcode = "48";
  OP_TAB["JSUB"].format = 3;
  OP_TAB["JSUB"].exists = true;

  OP_TAB["LDA"].opcode = "00";
  OP_TAB["LDA"].format = 3;
  OP_TAB["LDA"].exists = true;

  OP_TAB["LDB"].opcode = "68";
  OP_TAB["LDB"].format = 3;
  OP_TAB["LDB"].exists = true;

  OP_TAB["LDCH"].opcode = "50";
  OP_TAB["LDCH"].format = 3;
  OP_TAB["LDCH"].exists = true;

  OP_TAB["LDF"].opcode = "70";
  OP_TAB["LDF"].format = 3;
  OP_TAB["LDF"].exists = true;

  OP_TAB["LDL"].opcode = "08";
  OP_TAB["LDL"].format = 3;
  OP_TAB["LDL"].exists = true;

  OP_TAB["LDS"].opcode = "6C";
  OP_TAB["LDS"].format = 3;
  OP_TAB["LDS"].exists = true;

  OP_TAB["LDT"].opcode = "74";
  OP_TAB["LDT"].format = 3;
  OP_TAB["LDT"].exists = true;

  OP_TAB["LDX"].opcode = "04";
  OP_TAB["LDX"].format = 3;
  OP_TAB["LDX"].exists = true;

  OP_TAB["LPS"].opcode = "D0";
  OP_TAB["LPS"].format = 3;
  OP_TAB["LPS"].exists = true;

  OP_TAB["MUL"].opcode = "20";
  OP_TAB["MUL"].format = 3;
  OP_TAB["MUL"].exists = true;

  OP_TAB["MULF"].opcode = "60";
  OP_TAB["MULF"].format = 3;
  OP_TAB["MULF"].exists = true;

  OP_TAB["MULR"].opcode = "98";
  OP_TAB["MULR"].format = 2;
  OP_TAB["MULR"].exists = true;

  OP_TAB["NORM"].opcode = "C8";
  OP_TAB["NORM"].format = 1;
  OP_TAB["NORM"].exists = true;

  OP_TAB["OR"].opcode = "44";
  OP_TAB["OR"].format = 3;
  OP_TAB["OR"].exists = true;

  OP_TAB["RD"].opcode = "D8";
  OP_TAB["RD"].format = 3;
  OP_TAB["RD"].exists = true;

  OP_TAB["RMO"].opcode = "AC";
  OP_TAB["RMO"].format = 2;
  OP_TAB["RMO"].exists = true;

  OP_TAB["RSUB"].opcode = "4F";
  OP_TAB["RSUB"].format = 3;
  OP_TAB["RSUB"].exists = true;

  OP_TAB["SHIFTL"].opcode = "A4";
  OP_TAB["SHIFTL"].format = 2;
  OP_TAB["SHIFTL"].exists = true;

  OP_TAB["SHIFTR"].opcode = "A8";
  OP_TAB["SHIFTR"].format = 2;
  OP_TAB["SHIFTR"].exists = true;

  OP_TAB["SIO"].opcode = "F0";
  OP_TAB["SIO"].format = 1;
  OP_TAB["SIO"].exists = true;

  OP_TAB["SSK"].opcode = "EC";
  OP_TAB["SSK"].format = 3;
  OP_TAB["SSK"].exists = true;

  OP_TAB["STA"].opcode = "0C";
  OP_TAB["STA"].format = 3;
  OP_TAB["STA"].exists = true;

  OP_TAB["STB"].opcode = "78";
  OP_TAB["STB"].format = 3;
  OP_TAB["STB"].exists = true;

  OP_TAB["STCH"].opcode = "54";
  OP_TAB["STCH"].format = 3;
  OP_TAB["STCH"].exists = true;

  OP_TAB["STF"].opcode = "80";
  OP_TAB["STF"].format = 3;
  OP_TAB["STF"].exists = true;

  OP_TAB["STI"].opcode = "D4";
  OP_TAB["STI"].format = 3;
  OP_TAB["STI"].exists = true;

  OP_TAB["STL"].opcode = "14";
  OP_TAB["STL"].format = 3;
  OP_TAB["STL"].exists = true;

  OP_TAB["STS"].opcode = "7C";
  OP_TAB["STS"].format = 3;
  OP_TAB["STS"].exists = true;

  OP_TAB["STSW"].opcode = "E8";
  OP_TAB["STSW"].format = 3;
  OP_TAB["STSW"].exists = true;

  OP_TAB["STT"].opcode = "84";
  OP_TAB["STT"].format = 3;
  OP_TAB["STT"].exists = true;

  OP_TAB["STX"].opcode = "10";
  OP_TAB["STX"].format = 3;
  OP_TAB["STX"].exists = true;

  OP_TAB["SUB"].opcode = "1C";
  OP_TAB["SUB"].format = 3;
  OP_TAB["SUB"].exists = true;

  OP_TAB["SUBF"].opcode = "5C";
  OP_TAB["SUBF"].format = 3;
  OP_TAB["SUBF"].exists = true;

  OP_TAB["SUBR"].opcode = "94";
  OP_TAB["SUBR"].format = 2;
  OP_TAB["SUBR"].exists = true;

  OP_TAB["SVC"].opcode = "B0";
  OP_TAB["SVC"].format = 2;
  OP_TAB["SVC"].exists = true;

  OP_TAB["TD"].opcode = "E0";
  OP_TAB["TD"].format = 3;
  OP_TAB["TD"].exists = true;

  OP_TAB["TIO"].opcode = "F8";
  OP_TAB["TIO"].format = 1;
  OP_TAB["TIO"].exists = true;

  OP_TAB["TIX"].opcode = "2C";
  OP_TAB["TIX"].format = 3;
  OP_TAB["TIX"].exists = true;

  OP_TAB["TIXR"].opcode = "B8";
  OP_TAB["TIXR"].format = 2;
  OP_TAB["TIXR"].exists = true;

  OP_TAB["WD"].opcode = "DC";
  OP_TAB["WD"].format = 3;
  OP_TAB["WD"].exists = true;
}
void regtab_init()
{
  REG_TAB["A"].number = '0';
  REG_TAB["A"].exists = true;

  REG_TAB["X"].number = '1';
  REG_TAB["X"].exists = true;

  REG_TAB["L"].number = '2';
  REG_TAB["L"].exists = true;

  REG_TAB["B"].number = '3';
  REG_TAB["B"].exists = true;

  REG_TAB["S"].number = '4';
  REG_TAB["S"].exists = true;

  REG_TAB["T"].number = '5';
  REG_TAB["T"].exists = true;

  REG_TAB["F"].number = '6';
  REG_TAB["F"].exists = true;

  REG_TAB["PC"].number = '8';
  REG_TAB["PC"].exists = true;

  REG_TAB["SW"].number = '9';
  REG_TAB["SW"].exists = true;
}
//preprocess the input file
void preprocess(ifstream& in){
    set<char> oprtr; oprtr.insert('+');oprtr.insert('-');oprtr.insert('*');oprtr.insert('@');
    oprtr.insert('/');oprtr.insert('#');oprtr.insert(',');oprtr.insert('\'');oprtr.insert('=');
ofstream fout("preprocess.txt");
    string s,str,temp;
while(getline(in,s)){
    stringstream sfg(s);
    sfg>>temp;
if(temp[0]!='.'){
    int len=s.length();
    for(int i=0;i<len-1;i++){
        if(s[i+1]=='+' || s[i+1]=='-' || s[i+1]=='*' || s[i+1]=='/'){
            len++;
             s.insert(i+1,1,' ');i++;
        }
    }
    bool fk=false;
    len=s.length();
    for(int i=0;i<len-1;i++){
    if(s[i]==',' && s[i+1]!=' '){
        len++;
        s.insert(i+1,1,' ');i++;
    }
    }
    str=s;
    for(int i=0;i<s.length();i++){
        fk=(s[i]=='\'')?!fk:fk;
        if(!fk){
        str[i]=(char)toupper(s[i]);
        }
    }
    stringstream ss(str);    
    while(ss>>s){
        if(oprtr.find(s[0])==oprtr.end()){
            if(oprtr.find(s[s.length()-1])==oprtr.end()){
            fout<<s<<" ";
            }else{
                fout<<s.substr(0,s.length()-1)<<" "<<s[s.length()-1];
            }
        }else{
            if(s.length()==1)
            fout<<s;
            else fout<<s<<" ";
        }
        
    }
    fout<<"\n";
    }
}
}

//Pass1
//assigning address to labels and identifying duplicate labels
void pass1(ifstream& in,ofstream& ferror, bool& error){
    string label,opcode,operand,tempstr,csect_name="DEFAULT";int curr=0;
    int operand_val=0,line=0;
    while(getline(in,tempstr)){
        curr=CSECT[csect_name].LOCCTR;
        label=opcode=operand="";
        bool bn4=false;
        line++;//in "preprocess.txt"
        stringstream ss1(tempstr);
        ss1>>label;
        if(label[0]=='+' && OP_TAB[label.substr(1,label.length()-1)].exists){
            bn4=true;label=label.substr(1,label.length()-1);
        }
        if(OP_TAB[label].exists || ASM_DIR[label]){
            opcode=label;
            label="";
            getline(ss1,operand);
        }else{
            ss1>>opcode;
            getline(ss1,operand);
        }
        if(opcode[0]=='+'){
            bn4=true;
            opcode=opcode.substr(1,opcode.length()-1);
        }
        if(operand[0]==' '){
            int index646=0;
            while(operand[index646]==' ')index646++;
            operand=operand.substr(index646,operand.length()-index646);
        }
        if(opcode=="START"){
            ss1.clear(); ss1<<operand;
            ss1>>operand;
            CSECT[csect_name].start=convertHexToDec(operand);
            CSECT[csect_name].name =label;
            CSECT[csect_name].LOCCTR=convertHexToDec(operand);
        }else if(opcode=="CSECT"){

            for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
                if(ele.second.write){                   
                    CSECT[csect_name].LIT_TAB[ele.first].address=CSECT[csect_name].LOCCTR;                    
                    CSECT[csect_name].LOCCTR+=(ele.second.value.length()/2);
                    CSECT[csect_name].LIT_TAB[ele.first].write=false;
                }
            }  
            CSECT[csect_name].length=CSECT[csect_name].LOCCTR-CSECT[csect_name].start;          
                                
            csect_name=label;
            CSECT[csect_name].name=label;
        }else{// if the opcode is neither start nor csect
            if(label.length()!=0){// if the label exists process label
                if(CSECT[csect_name].SYM_TAB[label].exists){// if the label is already present in symbol table
                    error=true;
                    ferror<<"ERROR LINE:"<<line<<" duplicate symbol "<<label<<"\n";
                    //cout<<"ERROR LINE:"<<line<<" duplicate symbol "<<label<<"\n";
                }else{//inserting into the symbol table if not present
                    CSECT[csect_name].SYM_TAB[label].address=curr;
                    CSECT[csect_name].SYM_TAB[label].exists=true;
                    CSECT[csect_name].SYM_TAB[label].isRelative=true;                    
                }
            }            
            if(opcode=="END"){
                //handle LTORG here
                for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
                    if(ele.second.write){                   
                        CSECT[csect_name].LIT_TAB[ele.first].address=CSECT[csect_name].LOCCTR;                    
                        CSECT[csect_name].LOCCTR+=(ele.second.value.length()/2);
                        CSECT[csect_name].LIT_TAB[ele.first].write=false;
                    }
                }
            CSECT[csect_name].length=CSECT[csect_name].LOCCTR-CSECT[csect_name].start;          
                return;//successfully completed pass 1
            }else if(OP_TAB[opcode].exists){
                CSECT[csect_name].LOCCTR+=OP_TAB[opcode].format;
                if(bn4)CSECT[csect_name].LOCCTR++;
                if(operand.length()!=0){// checking for literals
                    string str640;
                    stringstream ss213(operand);
                    ss213>>str640;
                    if(str640[0]=='='){
                        if(str640[1]=='C'){
                            int index=3;
                            string val="";
                            int ch637;
                            while(index<str640.length()&& (str640[index]!='\'' && str640[index]!=' ' )){
                                ch637=(int)str640[index];
                                string te640=convertDecToHex(ch637);
                                changeString(te640,2);
                                val+=te640;
                                index++;                                
                            }
                            if(!CSECT[csect_name].LIT_TAB[val].exists)
                            {                                
                            CSECT[csect_name].LIT_TAB[val].value=val;
                            CSECT[csect_name].LIT_TAB[val].write=true;                        
                            CSECT[csect_name].LIT_TAB[val].exists=true;
                            }
                        }else if(str640[1]=='X'){
                            int index=3;
                            string val="";
                            while(index<str640.length() && (str640[index]!='\'' && str640[index]!=' ' )){
                                val+=str640[index];
                                index++;
                            }                            
                            if(!CSECT[csect_name].LIT_TAB[val].exists)
                            {
                            CSECT[csect_name].LIT_TAB[val].value=val;
                            CSECT[csect_name].LIT_TAB[val].exists=true;
                            CSECT[csect_name].LIT_TAB[val].write=true;                            
                            }

                        }else {
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" undefined literal type\n";
                            //cout<<"ERROR LINE:"<<line<<" undefined literal type\n";
                        }
                    }
                }
            }else if(opcode=="LTORG"){
                //handle LTORG here
                for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
                    if(ele.second.write){                   
                        CSECT[csect_name].LIT_TAB[ele.first].address=CSECT[csect_name].LOCCTR;                    
                        CSECT[csect_name].LOCCTR+=(ele.second.value.length()/2);
                        CSECT[csect_name].LIT_TAB[ele.first].write=false;
                    }
                }                
            }else if(opcode=="EQU"){
                if(label.length()!=0){
                    CSECT[csect_name].SYM_TAB[label].exists=true;
                    CSECT[csect_name].SYM_TAB[label].isRelative=false;
                    if(operand[0]=='*'){
                        CSECT[csect_name].SYM_TAB[label].address=curr;
                    }else{
                        int tval=0,ab=0;
                        eval_operand(operand,ab,error,ferror,tval,csect_name,line);
                        CSECT[csect_name].SYM_TAB[label].address=tval;
                    }
                }else{
                    error=true;
                    ferror<<"ERROR LINE:"<<line<<" Label Expeted for EQU directive \n";
                }
            }else if(opcode=="ORG"){
                if(operand.length()!=0){
                    int abs=0,temp12=0;
                    eval_operand(operand,abs,error,ferror,temp12,csect_name,line);
                    CSECT[csect_name].ORG_STACK.push(curr);
                    CSECT[csect_name].LOCCTR=temp12;                    
                } else {
                    if(!CSECT[csect_name].ORG_STACK.empty()){
                    int temp12=CSECT[csect_name].ORG_STACK.top();
                    CSECT[csect_name].ORG_STACK.pop();
                    CSECT[csect_name].LOCCTR=temp12;  
                    }else{
                        error=true;
                        ferror<<"ERROR: The ORG stack is empty\n";
                    }                  
                }
            }else if(opcode=="WORD"){
                CSECT[csect_name].LOCCTR+=3;
            }else if(opcode=="BYTE"){
                string s731=convertQuote(operand,line,error,ferror);
                CSECT[csect_name].LOCCTR+=(s731.length()/2);
            }else if(opcode=="RESW"){
                int temp733=0;int abs738=0;
                eval_operand(operand,abs738,error,ferror,temp733,csect_name,line);
                CSECT[csect_name].LOCCTR+=3*temp733;
                if(abs738!=0){
                error=true;
                ferror<<"ERROR: LINE"<<line<<" expression is not absolute value\n";            
                //cout<<"ERROR: LINE"<<line<<" expression is not absolute value\n";
                }                
            }else if(opcode=="RESB"){
                int temp736=0;int abs742=0;
                eval_operand(operand,abs742,error,ferror,temp736,csect_name,line);
                CSECT[csect_name].LOCCTR+=temp736;
                if(abs742!=0){
                error=true;
                ferror<<"ERROR: LINE"<<line<<" expression is not absolute value\n";            
                //cout<<"ERROR: LINE"<<line<<" expression is not absolute value\n";
                }
            }else if(opcode=="EXTDEF" ||opcode=="BASE" || opcode=="NOBASE"){
                //add to external table in pass2
            }else if(opcode=="EXTREF"){
                stringstream ss803(operand);
                string tem804;
                while(ss803>>tem804){
                    if(tem804[0]==',')tem804=tem804.substr(1,tem804.length()-1);
                    CSECT[csect_name].EXTREF_TAB[tem804].exists=true;
                    CSECT[csect_name].EXTDEF_TAB[tem804].name=tem804;
                }
            }else{
                error=true;
                ferror<<"ERROR LINE:"<<line<<" INVALID OPERATION CODE\n";
            }
        }
        if(label.length()>6){
            error=true;
            ferror<<"ERROR LINE:"<<line<<" used "<<label<<" with length more than 6\n";
        }
    }


}
//End of pass1

//Pass2
void pass2(ifstream& in,ofstream& ferror,bool& error,ofstream& ou ){
    ofstream LFile("Listing_File.txt");
    LFile<<"LINE"<<"  LOCCTR  INSTRUCTION           OBJECT CODE\n";
string H;
vector<string>T;string tempT="";
vector<string>D;string tempD="";
vector<string>R;string tempR="";
vector<string>M;string tempM="";
string E;
string label,opcode,operand,tempstr,csect_name="DEFAULT",obj_code;int curr=0;
int operand_val=0,line=0;int locctr=0;int LEN=0,ni=0,xbpe=0;
while(getline(in,tempstr)){
    obj_code="";
    ni=0;xbpe=0;
    curr=locctr;
    label=opcode=operand="";
    bool bn4=false;
    line++;//in "preprocess.txt"
    obj_code+=to_string(line)+"  ";
    obj_code+=convertDecToHex(locctr)+"  ";
    obj_code+=tempstr;
    stringstream ss1(tempstr);
    ss1>>label;
    if(label[0]=='+' && OP_TAB[label.substr(1,label.length()-1)].exists){
        bn4=true;label=label.substr(1,label.length()-1);
    }
    if(OP_TAB[label].exists || ASM_DIR[label]){
        opcode=label;
        label="";
        getline(ss1,operand);
    }else{
        ss1>>opcode;
        getline(ss1,operand);
    }
    if(opcode[0]=='+'){
        bn4=true;
        opcode=opcode.substr(1,opcode.length()-1);
    }
    if(operand[0]==' '){
        int index646=0;
        while(operand[index646]==' ')index646++;
        operand=operand.substr(index646,operand.length()-index646);
    }
    if(opcode=="START"){
        ss1.clear(); ss1<<operand;
        ss1>>operand;
        locctr=convertHexToDec(operand);
        H+="H";
        if(label.length()>6){error=true;ferror<<"ERROR LINE:"<<line<<" expecting 6 character program name\n";}
        string tem43=CSECT[csect_name].name;
        for(int i=1;i<=6-CSECT[csect_name].name.length();i++){
            tem43+=" ";
        }
        H=H+tem43;
        string t88=convertDecToHex(locctr);
        changeString(t88,6);
        H+=t88;
        LEN=0;        
    }else if(opcode=="CSECT"){ 
        //handle LTORG to listing file
        for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
            if(ele.second.write){      
                if(tempT.length()==0){
                    tempT="T";
                    string te88=convertDecToHex(curr);
                    changeString(te88,6);
                    tempT+=te88;
                }
                CSECT[csect_name].LIT_TAB[ele.first].address=locctr;
                LFile<<line<<"  "<<convertDecToHex(locctr)<<" * =X\'"<<ele.second.value<<"\'"<<"    "<<ele.second.value<<endl; 
                curr=locctr;              
                locctr+=(ele.second.value.length()/2);
                LEN+=ele.second.value.length()/2;
                CSECT[csect_name].LIT_TAB[ele.first].write=false;
                //text record entry
                if(tempT.length()+ele.second.value.length()<=67){
                    tempT+=ele.second.value;
                }else{
                    int va901=tempT.length()-7;
                    string h902=convertDecToHex(va901/2);
                    changeString(h902,2);
                    tempT.insert(7,1,h902[0]);
                    tempT.insert(8,1,h902[1]);
                    T.push_back(tempT);
                    tempT.clear();tempT="T";
                    string te88=convertDecToHex(curr);
                    changeString(te88,6);
                    tempT+=te88;
                    tempT+=ele.second.value;
                }
            }
        }        
        //dump the records here add first address to DEFAULT program
        string len87=convertDecToHex(LEN);
        changeString(len87,6);
        H=H+len87;
        E+="E";
        if(csect_name=="DEFAULT"){// converting integer address to hexa deciamal address
            string tem86="",temp836=convertDecToHex( CSECT[csect_name].start );
            for(int i=1;i<=(6-temp836.length());i++){
                tem86+="0";
            }
            E=E+tem86+temp836;
        }
        //dumping records of a control section
        ou<<H<<"\n";
        for(auto i:D)   ou<<i<<"\n";
        if(tempD.length()!=0)ou<<tempD<<"\n";        
        for(auto i:R)   ou<<i<<"\n";
        if(tempR.length()!=0)ou<<tempR<<"\n";
        for(auto i:T)   ou<<i<<"\n";
        if(tempT.length()!=0){int va901=tempT.length()-7;va901/=2;
                string h902=convertDecToHex(va901);
                changeString(h902,2);
                tempT.insert(7,1,h902[0]);
                tempT.insert(8,1,h902[1]);
                ou<<tempT<<"\n";
                }
        for(auto i:M)   ou<<i<<"\n";
        ou<<E<<"\n";
        H.clear();T.clear();D.clear();R.clear();M.clear();E.clear();tempD.clear();tempT.clear();tempM.clear();tempR.clear();
        locctr=0;     
        csect_name=label;
        if(operand.length()!=0)ferror<<"ERROR LINE:"<<line<<" expected no operand for CSECT \n";
        LEN=0;
        H+="H";
        if(label.length()>6){error=true;ferror<<"ERROR LINE:"<<line<<" expecting 6 character program name\n";}
        string tem43=CSECT[csect_name].name;
        for(int i=1;i<=6-CSECT[csect_name].name.length();i++){
            tem43+=" ";
        }
        H=H+tem43;
        H+="000000";
    }else{// if the opcode is neither start nor csect
        if(opcode=="END"){
            //handle LTORG to listing file
            for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
                if(ele.second.write){                   
                    if(tempT.length()==0){
                    tempT="T";
                    string te88=convertDecToHex(curr);
                    changeString(te88,6);
                    tempT+=te88;
                    }
                    CSECT[csect_name].LIT_TAB[ele.first].address=locctr;
                    LFile<<line<<"  "<<convertDecToHex(locctr)<<" * =X\'"<<ele.second.value<<"\'"<<"    "<<ele.second.value<<endl; 
                    curr=locctr;              
                    locctr+=(ele.second.value.length()/2);
                    LEN+=ele.second.value.length()/2;
                    CSECT[csect_name].LIT_TAB[ele.first].write=false;
                    //text record entry
                    if(tempT.length()+ele.second.value.length()<=67){
                        tempT+=ele.second.value;
                    }else{
                        int va901=tempT.length()-7;va901/=2;
                        string h902=convertDecToHex(va901);
                        changeString(h902,2);
                        tempT.insert(7,1,h902[0]);
                        tempT.insert(8,1,h902[1]);
                        T.push_back(tempT);
                        tempT.clear();tempT="T";
                        string te88=convertDecToHex(curr);
                        changeString(te88,6);
                        tempT+=te88;
                        tempT+=ele.second.value;
                    }
                }
            }
            string len87=convertDecToHex(LEN);
            changeString(len87,6);
            H=H+len87;
            E+="E";
            if(csect_name=="DEFAULT"){// converting integer address to hexa deciamal address
                string tem86="",temp836=convertDecToHex( CSECT[csect_name].start );
                for(int i=1;i<=(6-temp836.length());i++){
                    tem86+="0";
                }
                E=E+tem86+temp836;
            }
            ou<<H<<"\n";
            for(auto i:D)   ou<<i<<"\n";
            if(tempD.length()!=0)ou<<tempD<<"\n";
            for(auto i:R)   ou<<i<<"\n";
            if(tempR.length()!=0)ou<<tempR<<"\n";
            for(auto i:T)   ou<<i<<"\n";
            if(tempT.length()!=0){
                int va901=tempT.length()-7;va901/=2;
                string h902=convertDecToHex(va901);
                changeString(h902,2);
                tempT.insert(7,1,h902[0]);
                tempT.insert(8,1,h902[1]);
                ou<<tempT<<"\n";
                }
            for(auto i:M)   ou<<i<<"\n";
            ou<<E<<"\n";         
            LFile<<obj_code<<"\n";
            H.clear();T.clear();D.clear();R.clear();M.clear();E.clear();tempD.clear();tempT.clear();tempM.clear();tempR.clear();
            return;//successfully completed pass 2
        }else if(OP_TAB[opcode].exists){
            curr=locctr;string tObj="";
            locctr+=OP_TAB[opcode].format;
            LEN+=OP_TAB[opcode].format;
            if(bn4){locctr++;LEN++;}
            if(OP_TAB[opcode].format==1){//format 1
                tObj=OP_TAB[opcode].opcode;
            }else if(OP_TAB[opcode].format==2){//format 2
                tObj=OP_TAB[opcode].opcode;
                stringstream ss106(operand);
                string s106;
                while(ss106>>s106){
                    if(s106[0]==',')s106=s106.substr(1,s106.length()-1);
                    if(REG_TAB[s106].exists){
                        tObj+=(char)REG_TAB[s106].number;
                    }else{
                        error=true;
                        ferror<<"ERROR LINE:"<<line<<" The register \'"<<s106<<"\' is not valid\n";
                    }
                }
                int x=tObj.length();
                for(int i=1;i<=4-x;i++){
                    tObj+="0";
                }
            }else if(opcode=="RSUB"){
                tObj="4F0000";
                if(bn4)tObj+="00";
            }else if(OP_TAB[opcode].format==3){
                stringstream ss108(operand);
                string opr;
                ss108>>opr;
                int opc=convertHexToDec(OP_TAB[opcode].opcode),expr=0;
                if(bn4){//format 4
                    xbpe=1;//e=1;
                    if(opr[0]=='@'){
                        ni=2;//n=1
                        opr=opr.substr(1,opr.length()-1);
                        string tm="05";bool indexed=0;int rel=0,tempval=0;
                        string val10=eval_expression(operand,indexed,error,ferror,csect_name,line,curr,M,tm,rel,tempval);
                        changeString(val10,5);
                        opc+=ni;
                        string st1=convertDecToHex(opc);changeString(st1,2);
                        tObj+=st1;
                        st1=convertDecToHex(xbpe); changeString(st1,1);
                        tObj+=st1;
                        tObj+=val10;
                        ss108>>opr;
                        if(opr[1]=='X'){
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" cannot use indexed with indirect addressing mode\n";
                        }
                    }else if(operand[0]=='#'){
                        ni=1;//i=1
                        xbpe=1;//e=1
                        opr=opr.substr(1,opr.length()-1);
                        string tm="05";bool indexed=0;int rel=0,tempval;
                        string val10=eval_expression(operand,indexed,error,ferror,csect_name,line,curr,M,tm,rel,tempval);
                        changeString(val10,5);
                        opc+=ni;
                        string st1=convertDecToHex(opc);changeString(st1,2);
                        tObj+=st1;
                        st1=convertDecToHex(xbpe); changeString(st1,1);
                        tObj+=st1;
                        st1=convertDecToHex(tempval);changeString(st1,5);
                        tObj+=st1;
                        ss108>>opr;
                        if(opr[1]=='X'){
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" cannot use inedxed with immediate addressing mode\n";
                        }
                    }else{
                        tempM="M";
                        string t88=convertDecToHex(curr+1);
                        changeString(t88,6);
                        tempM+=t88;tempM+="05";
                        ni=3;//ni=3
                        xbpe=1;//e=1;
                        int val10=0;
                        if(CSECT[csect_name].SYM_TAB[opr].exists){
                            val10=CSECT[csect_name].SYM_TAB[opr].address;
                            tempM+='+'+CSECT[csect_name].name;
                            M.push_back(tempM);tempM.clear();
                        }else if(CSECT[csect_name].EXTREF_TAB[opr].exists){
                            val10=0;
                            tempM+='+'+opr;
                            M.push_back(tempM);tempM.clear();
                        }else{
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" Invalid operand \n";
                        }
                        ss108>>opr;
                        if(opr[1]=='X'){
                            xbpe+=8;
                        }
                        opc+=ni;
                        string st1=convertDecToHex(opc);changeString(st1,2);
                        tObj+=st1;
                        st1=convertDecToHex(xbpe);changeString(st1,1);
                        tObj+=st1;
                        st1=convertDecToHex(val10);changeString(st1,5);
                        tObj+=st1;                        
                    }
                }else{//format 3
                    string tm="03";
                    int val=0,opc;
                    opc=convertHexToDec(OP_TAB[opcode].opcode);
                    stringstream ss117(operand);
                    string opr;
                    ss117>>opr;
                    if(opr[0]=='@'){
                        ni=2;//indirect 
                        int tval=0;           
                        opr=opr.substr(1,opr.length()-1);
                        if(CSECT[csect_name].SYM_TAB[opr].exists){
                            val=CSECT[csect_name].SYM_TAB[opr].address;
                            val=val-locctr;
                            if(val>=2048 || val<-2048){
                                if(CSECT[csect_name].base.length()!=0){
                                    tval=CSECT[csect_name].SYM_TAB[CSECT[csect_name].base].address;
                                    val=val+locctr-tval;
                                    if(val<0 || val>4095){
                                        error=true;
                                        ferror<<"ERROR LINE:"<<line<<" disp out of bounds for BASE relative\n";
                                    }else{
                                        xbpe+=4;
                                    }
                                }else{
                                    error=true;
                                    ferror<<"ERROR LINE:"<<line<<" disp out of bounds for PC relative"<<"\n";
                                }                                
                            }else{
                                xbpe+=2;
                            }
                        }else if(CSECT[csect_name].EXTREF_TAB[opr].exists){
                            tempM="M";
                            string t88=convertDecToHex(curr+1);
                            changeString(t88,6);
                            tempM+=t88;
                            tempM+=tm;
                            tempM+='+';            
                            tempM+=opr;
                            if(opr.length()>6){
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" length of operand more than 6\n";
                            }
                            M.push_back(tempM);
                            tempM.clear();                            
                        }else{
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" operand "<<opr<<" not defined\n";
                        }    
                        ss117>>opr;
                                                            
                    }else if(opr[0]=='#'){
                        ni=1;//immediate
                        opr=opr.substr(1,opr.length()-1);
                        bool indexed=false;int rel=0,tempval=0;
                        string val12=eval_expression(operand,indexed,error,ferror,csect_name,line,curr,M,tm,rel,tempval);
                        if(rel==0){//absolute value
                            if(tempval>2047 || tempval <-2048 ) {
                                error=true;
                                ferror<<"ERROR LINE:"<<line<<" The value exceeds 3 half bytes\n";
                            }
                            val=tempval;                           
                        }else{
                            int tval=0;
                            val=tempval-locctr;
                            if(val>2047 || val<-2048){
                                if(CSECT[csect_name].base.length()!=0){
                                    tval=CSECT[csect_name].SYM_TAB[CSECT[csect_name].base].address;
                                    val=val+locctr-tval;
                                    if(val<0 || val>4095){
                                        error=true;
                                        ferror<<"ERROR LINE:"<<line<<" disp out of bounds for BASE relative\n";
                                    }else{
                                        xbpe+=4;
                                    }
                                }else{
                                    error=true;
                                    ferror<<"ERROR LINE:"<<line<<" disp out of bounds for PC relative"<<"\n";
                                }
                            }else{
                                xbpe+=2;//p=1
                            }
                        }
                        if(indexed){
                            error=true;
                            ferror<<"ERROR LINE:"<<line<<" indexed with immediate not allowed\n";
                        }
                    }else{
                        ni=3;//simple
                        if(opr[0]=='='){//liteerals
                            if(opr[1]=='C'){
                                int index=3;
                                string val12="";
                                int ch637;
                                while(index<opr.length()&& (opr[index]!='\'' && opr[index]!=' ' )){
                                    ch637=(int)opr[index];
                                    string te640=convertDecToHex(ch637);
                                    changeString(te640,2);
                                    val12+=te640;
                                    index++;                                
                                }
                                if(!CSECT[csect_name].LIT_TAB[val12].write)
                                {                                
                                CSECT[csect_name].LIT_TAB[val12].write=true; 
                                }
                                val=CSECT[csect_name].LIT_TAB[val12].address;
                            }else if(opr[1]=='X'){
                                int index=3;
                                string val12="";
                                while(index<opr.length() && (opr[index]!='\'' && opr[index]!=' ' )){
                                    val12+=opr[index];
                                    index++;
                                }                            
                                if(!CSECT[csect_name].LIT_TAB[val12].write)
                                {
                                CSECT[csect_name].LIT_TAB[val12].write=true;                            
                                }
                                val=CSECT[csect_name].LIT_TAB[val12].address;
                            }
                            int tval=0;
                            val=val-locctr;
                            if(val>2047 || val<-2048){
                                if(CSECT[csect_name].base.length()!=0){
                                    tval=CSECT[csect_name].SYM_TAB[CSECT[csect_name].base].address;
                                    val=val+locctr-tval;
                                    if(val<0 || val>4095){
                                        error=true;
                                        ferror<<"ERROR LINE:"<<line<<" disp out of bounds for BASE relative\n";
                                    }else{
                                        xbpe+=4;
                                    }
                                }else{
                                    error=true;
                                    ferror<<"ERROR LINE:"<<line<<" disp out of bounds for PC relative"<<"\n";
                                }
                            }else{
                                xbpe+=2;//1;
                            }
                            
                        }else{// normal 
                            bool indexed=false;int rel=0,tempval=0;
                            string val1=eval_expression(operand,indexed,error,ferror,csect_name,line,curr,M,tm,rel,tempval);                                                        
                            if(rel==0){
                                if(tempval>2047 || tempval<-2048 ){
                                    error=true;
                                    ferror<<"ERROR LINE:"<<line<<" disp value out of bounds\n";
                                }else{
                                    val=tempval;
                                }
                            }else{
                                    int tval=0;
                                    val=tempval-locctr;
                                    if(val>2047 || val<-2048){
                                        if(CSECT[csect_name].base.length()!=0){
                                            tval=CSECT[csect_name].SYM_TAB[CSECT[csect_name].base].address;
                                            val=val+locctr-tval;
                                            if(val<0 || val>4095){
                                                error=true;
                                                ferror<<"ERROR LINE:"<<line<<" disp out of bounds for BASE relative\n";
                                            }else{
                                                xbpe+=4;
                                            }
                                        }else{
                                            error=true;
                                            ferror<<"ERROR LINE:"<<line<<" disp out of bounds for PC relative"<<"\n";
                                        }
                                    }else{
                                        xbpe+=2;//p=1
                                    }
                            }                            
                            if(indexed)xbpe+=8;
                        }

                    }
                    opc+=ni;
                    string sti=convertDecToHex(opc);
                    changeString(sti,2);
                    tObj+=sti;
                    sti=convertDecToHex(xbpe);
                    changeString(sti,1);
                    tObj+=sti;
                    sti=convertDecToHex(val);
                    changeString(sti,3);
                    tObj+=sti;
                }
            }       
            //add tempT obj_code here
            obj_code+="  "+tObj;
            if(tempT.length()==0){
                tempT="T";
                string t88=convertDecToHex(curr);
                changeString(t88,6);
                tempT+=t88;
            }
            if(tempT.length()+tObj.length()<=67){
                tempT+=tObj;
            }else{
                int va901=tempT.length()-7;va901/=2;
                string h902=convertDecToHex(va901);
                changeString(h902,2);
                tempT.insert(7,1,h902[0]);
                tempT.insert(8,1,h902[1]);
                T.push_back(tempT);
                tempT.clear();tempT="T";
                string te88=convertDecToHex(curr);
                changeString(te88,6);
                tempT+=te88;
                tempT+=tObj;
            }
            
        }else if(opcode=="LTORG"){
            //handle LTORG here
            for(auto ele:CSECT[csect_name].LIT_TAB){ //HANDLING LTORG                
                if(ele.second.write){                   
                    if(tempT.length()==0){
                    tempT="T";
                    string te88=convertDecToHex(curr);
                    changeString(te88,6);
                    tempT+=te88;
                    }
                    CSECT[csect_name].LIT_TAB[ele.first].address=locctr;
                    LFile<<line<<"  "<<convertDecToHex(locctr)<<" * =X\'"<<ele.second.value<<"\'"<<"    "<<ele.second.value<<endl; 
                    curr=locctr;              
                    locctr+=(ele.second.value.length()/2);
                    LEN+=ele.second.value.length()/2;
                    CSECT[csect_name].LIT_TAB[ele.first].write=false;
                    //text record entry
                    if(tempT.length()+ele.second.value.length()<=67){
                        tempT+=ele.second.value;
                    }else{
                        int va901=tempT.length()-7;
                        string h902=convertDecToHex(va901/2);
                        changeString(h902,2);
                        tempT.insert(7,1,h902[0]);
                        tempT.insert(8,1,h902[1]);
                        T.push_back(tempT);
                        tempT.clear();tempT="T";
                        string te88=convertDecToHex(curr);
                        changeString(te88,6);
                        tempT+=te88;
                        tempT+=ele.second.value;
                    }
                }
            }                
        }else if(opcode=="EQU"){
            
            //do nothing
        }else if(opcode=="ORG"){
            if(operand.length()!=0){
                int abs=0,temp12=0;
                eval_operand(operand,abs,error,ferror,temp12,csect_name,line);
                CSECT[csect_name].ORG_STACK.push(curr);
                locctr=temp12;                    
            } else {
                if(!CSECT[csect_name].ORG_STACK.empty()){
                int temp12=CSECT[csect_name].ORG_STACK.top();
                CSECT[csect_name].ORG_STACK.pop();
                locctr=temp12;  
                }else{
                    error=true;
                    ferror<<"ERROR: The ORG stack is empty\n";
                }                  
            }
        }else if(opcode=="WORD"){
            //add word to text record 
            string tm="06";bool indexed=0;int rel=0,tempval;
            string val1157=eval_expression(operand,indexed,error,ferror,csect_name,line,locctr,M,tm,rel,tempval);
            changeString(val1157,6);
            obj_code+=" "+val1157;
            if(tempT.length()==0){
                 tempT="T";
                string te88=convertDecToHex(curr);
                changeString(te88,6);
                tempT+=te88;
            }
            if(tempT.length()+6<=67){
                tempT+=val1157;
            }else{
                int va901=tempT.length()-7;va901/=2;
                string h902=convertDecToHex(va901);
                changeString(h902,2);
                tempT.insert(7,1,h902[0]);
                tempT.insert(8,1,h902[1]);
                T.push_back(tempT);
                tempT.clear();tempT="T";
                string te88=convertDecToHex(curr);
                changeString(te88,6);
                tempT+=te88;
                tempT+=val1157;
            }
            locctr+=3;
            LEN+=3;
        }else if(opcode=="BYTE"){            
            string s731=convertQuote(operand,line,error,ferror);
            locctr+=(s731.length()/2);
            LEN+=(s731.length()/2);
            if(tempT.length()==0){
                tempT="T";
                string te88=convertDecToHex(curr);
                changeString(te88,6);
                tempT+=te88;
            }
            if(tempT.length()+s731.length()<=67){
                tempT+=s731;
            }else{
                int va901=tempT.length()-7;va901/=2;
                string h902=convertDecToHex(va901);
                changeString(h902,2);
                tempT.insert(7,1,h902[0]);
                tempT.insert(8,1,h902[1]);
                T.push_back(tempT);
                tempT.clear();tempT="T";
                string te88=convertDecToHex(curr);
                changeString(te88,6);
                tempT+=te88;
                tempT+=s731;
            }
            obj_code+=" "+s731;
        }else if(opcode=="RESW"){
            if(tempT.length()!=0){
                
            int va901=tempT.length()-7;va901/=2;
            string h902=convertDecToHex(va901);
            changeString(h902,2);
            tempT.insert(7,1,h902[0]);
            tempT.insert(8,1,h902[1]);
            T.push_back(tempT);
            tempT.clear();
            }
            //stop text record 
            int temp733=0;int abs738=0;
            eval_operand(operand,abs738,error,ferror,temp733,csect_name,line);
            locctr+=3*temp733;
            LEN+=3*temp733;
            if(abs738!=0){
            error=true;
            ferror<<"ERROR: LINE"<<line<<" expression is not absolute value\n";            
            //cout<<"ERROR: LINE"<<line<<" expression is not absolute value\n";
            }                
        }else if(opcode=="RESB"){
            if(tempT.length()!=0){
            int va901=tempT.length()-7;va901/=2;
            string h902=convertDecToHex(va901);
            changeString(h902,2);
            tempT.insert(7,1,h902[0]);
            tempT.insert(8,1,h902[1]);
            T.push_back(tempT);
            tempT.clear();
            }           
            //stop text record 
            int temp736=0;int abs742=0;
            eval_operand(operand,abs742,error,ferror,temp736,csect_name,line);
            cout<<temp736<<endl;
            locctr+=temp736;
            LEN+=temp736;
            if(abs742!=0){
            error=true;
            ferror<<"ERROR: LINE"<<line<<" expression is not absolute value\n";            
            //cout<<"ERROR: LINE"<<line<<" expression is not absolute value\n";
            }
        }else if(opcode=="EXTDEF" ){
            //add to D record
            string sd27;
            if(tempD.length()==0)tempD="D";
            stringstream ss102(operand);
            while(ss102>>sd27){
                if(sd27[0]==',')sd27=sd27.substr(1,sd27.length()-1);
                string strd103=sd27;
                if(CSECT[csect_name].SYM_TAB[sd27].exists){  
                string valu103=convertDecToHex( CSECT[csect_name].SYM_TAB[sd27].address);
                changeString(valu103,6);
                for(int i=1;i<=6-sd27.length();i++)strd103+=" ";
                strd103+=valu103;
                if(tempD.length()+12<=73){
                    tempD+=strd103;
                }else{
                    D.push_back(tempD);
                    tempD.clear();tempD="D";
                    tempD+=strd103;
                }
                }else{
                    error=true;
                    ferror<<"ERROR LINE:"<<line<<" Undefined symbol"<<sd27<<"\n";
                }

            }
        }else if(opcode=="EXTREF"){
            //add to R record 
            string sd27;
            if(tempR.length()==0)tempR="R";
            stringstream ss102(operand);
            while(ss102>>sd27){
                if(sd27[0]==',')sd27=sd27.substr(1,sd27.length()-1);
                string strr10=sd27;
                for(int i=1;i<=6-sd27.length();i++)strr10+=" ";
                if(tempR.length()+6<=73){
                    tempR+=strr10;                    
                }else{
                    R.push_back(tempR);
                    tempR.clear();tempR="R";
                    tempR+=strr10;
                }
            }
        }else if(opcode=="BASE"){
            stringstream ss102(operand);
            ss102>>operand;
            if(CSECT[csect_name].SYM_TAB[operand].exists)
            CSECT[csect_name].base=operand;
            else{
                error=true;
                ferror<<"ERROR LINE:"<<line<<" undefined symbol "<<operand<<endl;
            }
        }else if(opcode=="NOBASE"){
            //stop base usage
            CSECT[csect_name].base="";
        }else{
            error=true;
            ferror<<"ERROR LINE:"<<line<<" INVALID OPERATION CODE\n";
        }
    }
    if(label.length()>6){
        error=true;
        ferror<<"ERROR LINE:"<<line<<" used "<<label<<" with length more than 6\n";
    }
    cout<<obj_code<<"\n";
    LFile<<obj_code<<"\n";

}
}
//End of pass2

int main(){
    regtab_init();
    optab_init();
    asmdir_init();
    ifstream in("input.txt");
    ofstream ferror("errors.txt");
    ofstream ou("output.txt");
    if(!in){
        cout<<"ERROR: could not open input file"<<endl;
        exit(1);
    }
    if(!ou){
        cout<<"ERROR: could not open output file"<<endl;
        exit(1);
    }
    preprocess(in);
    in.close();
    cout<<".\n.Preprocessing\n.\n";
    in.open("preprocess.txt");
    bool error=false;
    cout<<".\nPASS1 Starting\n.\n";
    ferror<<"PASS1:\n";
    pass1(in,ferror,error);
    in.close();
    in.open("preprocess.txt");
    cout<<error<<" End of pass1 \n";
    
    cout<<".\nPASS2 Starting\n.\n";
    ferror<<"PASS2:\n";
    pass2(in,ferror,error,ou);
    cout<<error<<" End of pass2 \n";
    ofstream fout("TABLES.txt");    
    for(auto cs:CSECT){//displaying tables
        fout<<"---------------------------------------------------------------\n";    
        fout<<cs.second.name<<" START:"<<cs.second.start<<" LENGTH:"<<cs.second.length<<" LOCCTR:"<<cs.second.LOCCTR<<"\n";
        fout<<"SYMBOL TABLE\n";
        for(auto st:cs.second.SYM_TAB){
            if(st.second.exists)
            fout<<"NAME: "<<st.first<<" Address: "<<st.second.address<<" IsRelative: "<<st.second.isRelative<<"\n";
        }
        fout<<"\nLITERAL TABLE\n";
        for(auto lt:cs.second.LIT_TAB){
            fout<<"NAME:"<<lt.first<<"  VALUE: "<<lt.second.value<<"    ADDRESS: "<<lt.second.address<<"    WRITE: "<<lt.second.write<<"\n";
        }
    }
    fout.close();
    in.close();
    ferror.close();
    ou.close();
    return 0;
}