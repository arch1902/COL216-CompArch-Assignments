// Use "make run" to compile and run on "text.txt"
#include <iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<vector>
#include<map>
#include<regex>
#include<sstream>
#include<fstream>
#include<cmath>
using namespace std;

fstream file1; 
ofstream out;
map<string,int> reg;
map<int,vector<string>> params;
map<string,int> label;
map<int,int> data_memory;
vector<vector<int>> DRAM(512,vector<int>(256,1));
map<string,int> statistics;
vector<string> operations = {"add","sub","mul","beq","bne","slt","lw","sw","addi"};
vector<string> registers = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$k0","$k1","$ra","$t0","$t1",
"$t2","$t3","$t4","$t5","$t6","$t7","$t8","$t9","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$gp",
"$fp","$sp"};
regex n("[-]?[0-9]+");  //for checking if a string is convertible to an Integer
regex l("([A-Z|a-z])[A-Z|a-z|0-9|_]*"); // for Label
int n_total;

int INSTRUCTION_MEMORY = pow(2,17); // Memory is word Addressable hence it has 2^19 Bytes
int DATA_MEMORY = pow(2,17);


int pc = 0; // program counter
vector<int> row_buffer; // row buffer
int num_rbf = 0; // number of row buffer updates
int curr = -1; // current row number in the buffer
vector<string> r,w; // currently being read/written
string msg1,msg2;

vector<string> instructions; //Vector containing all instructions 
string line;
int num=0;
size_t x;
string Instruction;
int finish , start;


// This code removes trailing and starting whitespace characters
const std::string WHITESPACE = " \n\r\t\f\v";
std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
std::string rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}
//Checks if the input string is present in the register or not
void validator(vector<string> V, string s,int l){
    for( auto j : V){
        if (j == s){
            return;
        }
    }
    cout<<"Invalid input "<<s<<" found on line "<<l<<endl;
    exit(-1);
}

void print(){
    out<<"zero = "<<reg["$zero"]<<endl;
    out<<"s0 = "<<reg["$s0"]<<endl;
    out<<"s1 = "<<reg["$s1"]<<endl;
    out<<"s2 = "<<reg["$s2"]<<endl;
    out<<"s3 = "<<reg["$s3"]<<endl;
    out<<"s4 = "<<reg["$s4"]<<endl;
    out<<"s5 = "<<reg["$s5"]<<endl;
    out<<"s6 = "<<reg["$s6"]<<endl;
    out<<"s7 = "<<reg["$s7"]<<endl;
    out<<"t0 = "<<reg["$t0"]<<endl;
    out<<"t1 = "<<reg["$t1"]<<endl;
    out<<"t2 = "<<reg["$t2"]<<endl;
    out<<"t3 = "<<reg["$t3"]<<endl;
    out<<"t4 = "<<reg["$t4"]<<endl;
    out<<"t5 = "<<reg["$t5"]<<endl;
    out<<"t6 = "<<reg["$t6"]<<endl;
    out<<"t7 = "<<reg["$t7"]<<endl;
    out<<"t8 = "<<reg["$t8"]<<endl;
    out<<"t9 = "<<reg["$t9"]<<endl;
    out<<"v0 = "<<reg["$v0"]<<endl;
    out<<"v1 = "<<reg["$v1"]<<endl;
    out<<"a0 = "<<reg["$a0"]<<endl;
    out<<"a1 = "<<reg["$a1"]<<endl;
    out<<"a2 = "<<reg["$a2"]<<endl;
    out<<"a3 = "<<reg["$a3"]<<endl;
    out<<"k0 = "<<reg["$k1"]<<endl;
    out<<"gp = "<<reg["$gp"]<<endl;
    out<<"sp = "<<reg["$sp"]<<endl;
    out<<"fp = "<<reg["$fp"]<<endl;
    out<<"ra = "<<reg["$ra"]<<endl;
    out<<"at = "<<reg["$at"]<<endl;
}

//Converts decimal to hexadecimal
string dectohex(int n){
    stringstream ss; 
    ss << hex << n; 
    string res = ss.str(); 
    return res;    
}
//prints the registers in hexadecimal format
void print_hex(){
    out<<"R0 [zero] = "<<dectohex(reg["$zero"])<<endl;
    out<<"R1  [r1]  = "<<dectohex(reg["$r1"])<<endl;
    out<<"R2  [r2]  = "<<dectohex(reg["$r2"])<<endl;
    out<<"R3  [r3]  = "<<dectohex(reg["$r3"])<<endl;
    out<<"R4  [r4]  = "<<dectohex(reg["$r4"])<<endl;
    out<<"R5  [r5]  = "<<dectohex(reg["$r5"])<<endl;
    out<<"R6  [r6]  = "<<dectohex(reg["$r6"])<<endl;
    out<<"R7  [r7]  = "<<dectohex(reg["$r7"])<<endl;
    out<<"R8  [r8]  = "<<dectohex(reg["$r8"])<<endl;
    out<<"R9  [r9]  = "<<dectohex(reg["$r9"])<<endl;
    out<<"R10 [r10] = "<<dectohex(reg["$r10"])<<endl;
    out<<"R11 [r11] = "<<dectohex(reg["$r11"])<<endl;
    out<<"R12 [r12] = "<<dectohex(reg["$r12"])<<endl;
    out<<"R13 [r13] = "<<dectohex(reg["$r13"])<<endl;
    out<<"R14 [r14] = "<<dectohex(reg["$r14"])<<endl;
    out<<"R15 [r15] = "<<dectohex(reg["$r15"])<<endl;
    out<<"R16 [r16] = "<<dectohex(reg["$r16"])<<endl;
    out<<"R17 [r17] = "<<dectohex(reg["$r17"])<<endl;
    out<<"R18 [r18] = "<<dectohex(reg["$r18"])<<endl;
    out<<"R19 [r19] = "<<dectohex(reg["$r19"])<<endl;
    out<<"R20 [r20] = "<<dectohex(reg["$r20"])<<endl;
    out<<"R21 [r21] = "<<dectohex(reg["$r21"])<<endl;
    out<<"R22 [r22] = "<<dectohex(reg["$r22"])<<endl;
    out<<"R23 [r23] = "<<dectohex(reg["$r23"])<<endl;
    out<<"R24 [r24] = "<<dectohex(reg["$r24"])<<endl;
    out<<"R25 [r25] = "<<dectohex(reg["$r25"])<<endl;
    out<<"R26 [r26] = "<<dectohex(reg["$r26"])<<endl;
    out<<"R27 [r27] = "<<dectohex(reg["$r27"])<<endl;
    out<<"R28 [r28] = "<<dectohex(reg["$r28"])<<endl;
    out<<"R29 [r29] = "<<dectohex(reg["$r29"])<<endl;
    out<<"R30 [r30] = "<<dectohex(reg["$r30"])<<endl;
    out<<"R31 [r31] = "<<dectohex(reg["$r31"])<<endl<<endl;
}
// Validator for the operands of instructions "add","sub","mul","addi","slt"
//Operator $reg1,reg2,reg3/Int
void validator_add(string s, int l, string instruction){
        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if (instruction=="addi" && count == 2){
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
            }else if (count == 2){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
                }
            }
            else validator(registers,substr,l+1);
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid number of operands for "<< instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
// Validator for instructions "bne" and "beq"
// beq $reg1,$reg2/Int,label
void validator_beq(string s, int l, string instruction){
        stringstream s_stream(s);   
        int count = 0;
        string substr;
        while(s_stream.good()) {
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1);}
            if(count ==1){
                if(substr[0] =='$'){validator(registers,substr,l+1);}
                else{
                    if(!regex_match(substr,n)){
                        cout<<"Invalid second operator "<<substr<<"for operator "<<instruction<<endl;
                        exit(-1);
                    }else{
                        if (abs(stoi(substr))>pow(2,16)){
                            cout<<"Error! I-type instruction cannot exceed 2^16 in MIPS on line "<<l+1<<endl;
                            exit(-1); 
                        }
                    }
                }
            }
            params[l].push_back(substr);
            count +=1;
        }
        if (count != 3){
            cout<<"Invalid operands for "<<instruction <<" operator on line "<<l+1<<endl;
            exit(-1);
        }
}
//Validator for Instructions "lw" and "sw"
//lw $reg1,off($reg2)
void validator_lw(string s, int l, string instruction){

        stringstream s_stream(s);
        int count = 0;
        string substr;
        while(s_stream.good()) {    
            getline(s_stream, substr, ',');
            substr = trim(substr);
            if(count==0){validator(registers,substr,l+1);params[l].push_back(substr);}
            if(count ==1){
                //off(reg)
                size_t q;
                q = substr.find_first_of('(');
                if(q==string::npos){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                string lh = substr.substr(0,q);
                // cout<<lh<<"\n";
                if(lh==""){
                    params[l].push_back("0");
                }
                else if (regex_match(lh,n)){
                    params[l].push_back(lh);
                }else{cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                //(register) 
                string rh = substr.substr(q);
                if (rh[0] != '(' || rh.back() != ')'){cout<<"Invalid Operand for "<<instruction<<" on line "<<l+1<<endl;exit(-1);}
                rh = trim(rh.substr(1,rh.size()-2));
                validator(registers,rh,l);
                params[l].push_back(rh);
            }
            count +=1;
        }
        if (count != 2){
            cout<<"Invalid operands for "<<instruction <<" operator on line "<<l+1;
            exit(-1);
        }
}

void add(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]+y;
            pc++;
            print();

}

void sub(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]-y;
            pc++;
            print();
}

void mul(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            reg[params[pc][1]]=reg[params[pc][2]]*y;
            pc++;
            print();
}

void beq(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            if (label.find(params[pc][3]) == label.end()){
                cout << "Invalid Label on line "<<pc+1<<endl;
                exit(-1);
            }
            int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
            if (reg[params[pc][1]] == y){
                pc = label[params[pc][3]];
                return;   
            }
            pc++;
}

void bne(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            if (label.find(params[pc][3]) == label.end()){
                cout << "Invalid Label on line "<<pc+1<<endl;
                exit(-1);
            }
            int y = (params[pc][2][0]=='$') ? reg[params[pc][2]] : stoi(params[pc][2]);
            if (reg[params[pc][1]] != y){
                pc = label[params[pc][3]];
                return;
            }
            pc++; 
}

void slt(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int y = (params[pc][3][0]=='$') ? reg[params[pc][3]] : stoi(params[pc][3]);
            if(reg[params[pc][2]]< y ){
                reg[params[pc][1]] = 1;
            }else {
                reg[params[pc][1]] = 0;
            }
            pc++; 
            print();
}

void j(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            if (label.find(params[pc][1]) == label.end()){
                cout << "Invalid Label on line "<<pc+1;
                exit(-1);
            }
            pc = label[params[pc][1]];
}

void addi(){
            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            reg[params[pc][1]]=reg[params[pc][2]]+stoi(params[pc][3]);
            pc++; 
            print();      
}


// check if the instruction is safe
bool check1(){
    if(count(r.begin(),r.end(),params[pc][1])) return false ;
    if(count(w.begin(),w.end(),params[pc][1])) return false ;
    if(count(w.begin(),w.end(),params[pc][2])) return false ;
    if(count(w.begin(),w.end(),params[pc][3])) return false ;
    return true;

}
// check if the instruction is safe
bool check2(){
    if(count(w.begin(),w.end(),params[pc][1])) return false ;
    if(count(w.begin(),w.end(),params[pc][2])) return false ;
    return true;   

}

// executes next few instructions when a lw/sw request is being processed 
void non_blocking(){
    while(n_total<=finish && pc<num){
        if(instructions[pc]==""){pc++;continue;}
        Instruction=params[pc][0];
        if(Instruction=="add" or Instruction=="sub" or Instruction=="mul" or Instruction=="addi" or Instruction=="slt"){
            bool ok = check1();
            if(!ok) return;
        }

        if(Instruction=="beq" or Instruction=="bne"){
            bool ok = check2();
            if(!ok) return;
        }  

        n_total++;
        statistics[Instruction]++;

        if(Instruction=="add") {
            add();

        } else if (Instruction=="sub"){
            sub();

        } else if (Instruction=="mul"){
            mul();

        } else if (Instruction=="beq"){
            beq();

        } else if (Instruction=="bne"){
            bne();   

        } else if (Instruction=="slt"){
            slt();

        } else if (Instruction=="j"){
            j();

        } else if (Instruction=="addi"){
            addi();

        } else if (Instruction=="lw"){
            return;

        } else if (Instruction=="sw"){
            return;

        } else if( label.find(params[pc][0]) != label.end() ){
             pc ++;
             n_total --;

        }else{
            continue;
        }
        if(reg["$zero"] != 0){
            cout<<"The value in Zero Registor is not mutable. Command on line "<<pc<<" tries to change its value"<<endl;
            exit(-1);
        }


    }

}

// Main program starts 


int main(int argc, char *argv[]) {
    file1.open(argv[1]);
    if(!file1.is_open()){cout<<"File not found"<<endl;exit(-1);}
    string input_file = argv[1];
    int ROW_ACCESS_DELAY = atoi(argv[2]);
    int COL_ACCESS_DELAY = atoi(argv[3]);
    out.open(input_file.substr(0,input_file.size())+"_output.txt");

    while(getline(file1,line)){
        line = trim(line);
        instructions.push_back(line);
        if(line == ""){
            num++;
            continue;
        }
        x = line.find_first_of('$');
        Instruction = line;
        if(x==string::npos){
            // The line does not contain $ symbol hence it can either be a label or a jump instruction
            if (Instruction.substr(0,2) == "j "){
                params[num].push_back("j");
                string oprd = trim(Instruction.substr(2));
                params[num].push_back(oprd);
            }else if(Instruction.back() == ':'){
                string b = trim(Instruction.substr(0,Instruction.size()-1));
                if (regex_match(b,l)){
                    if(label.find(b) != label.end()){
                        cout<<"Same Label can't be defined twice"<<endl;
                        cout<<"Label "<<b<<" is defined on lines "<<label[b]+1<<" and "<<num+1<<endl;
                        exit(-1);
                    }
                     label[b] = num;
                     params[num].push_back(b);
                 }else{
                     cout<< "Invaid Label format on line"<<num+1;
                     exit(-1);
                 }
             }else{
                cout<< "Invalid Input "<<Instruction<<" on line "<<num+1;
                exit(-1);
            }
        }else{
            // The line contains $ symbol
            Instruction = trim(line.substr(0,x));           
            validator(operations,Instruction,num+1);           
            params[num].push_back(Instruction);
            string operands = trim(line.substr(x));            
            if (Instruction == "add"||Instruction =="sub"||Instruction =="mul"||Instruction =="slt"||Instruction =="addi"){  //$t1, $t2, $t3                
                validator_add(operands,num,Instruction);
            }else if (Instruction=="beq"||Instruction =="bne"){
                validator_beq(operands,num,Instruction);
            }else if (Instruction == "lw"||Instruction =="sw"){
                validator_lw(operands,num,Instruction);
            }else{
                continue;
            }
        }
        num++;
        // Throws an error if number of commands is more than Instruction storage limit.
        if (num>INSTRUCTION_MEMORY){
            cout<<"Error! Instruction memory limit exceeded"<<endl;
            exit(-1);
        }
        // instruction memory nums X 4 bytes
    }
    file1.close();
// -----------------------------------------------PARSING ENDS ---------------------------------------------------------
    // for (auto const& j : params){
    //      int c = j.first;
    //      vector<string> v = j.second;
    //      cout<<c<<"\n";
    //      for(auto j: v){
    //          cout<<string(j)<<" ";
    //      }
    //      cout<<"\n";
    // }
    // for(auto const& j : label){
    //     cout<<j.first<<"->"<<j.second<<"   ";
    // }
    // cout<<endl;


    // Program Counter that iterates over the instruction set and points to the instruction being executed
    while(pc<num){
        if(instructions[pc]==""){pc++;continue;}
        n_total++;
        
        Instruction=params[pc][0];
        statistics[Instruction] ++;
        if(Instruction=="add") {
            add();

        } else if (Instruction=="sub"){
            sub();

        } else if (Instruction=="mul"){
            mul();

        } else if (Instruction=="beq"){
            beq();

        } else if (Instruction=="bne"){
            bne();   

        } else if (Instruction=="slt"){
            slt();

        } else if (Instruction=="j"){
            j();

        } else if (Instruction=="lw"){      // READ

            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int offset = stoi(params[pc][2]);
            if((offset+reg[params[pc][3]])%4 != 0 || (offset+reg[params[pc][3]])<0 ||(offset+reg[params[pc][3]])>DATA_MEMORY*4){
                cout<<"Program tried to access invalid memory location. "<<endl;
                exit(-1);
            }
            out<<"DRAM request issued : READ"<<endl<<endl;
            start = n_total;
            
            int row = (offset+reg[params[pc][3]])/1024;
            if(curr==-1){
                finish =start + ROW_ACCESS_DELAY + COL_ACCESS_DELAY;
                curr = row;
                row_buffer=DRAM[row];        // row activated in row buffer
                num_rbf++;
                msg1 = "ROW "+to_string(row)+" activated";
                msg2 = "Data at the column offset from the row buffer loaded to Register "+params[pc][1];
            } else if (curr == row){
                finish =start + COL_ACCESS_DELAY;
                msg1 = "ROW "+to_string(row)+" already present in ROW BUFFER";
                msg2 = "Data at the column offset from the row buffer loaded to Register "+params[pc][1];
            } else {
                msg1 = "ROW "+to_string(curr)+" copied back to DRAM and ROW "+to_string(row)+" activated";
                msg2 = "Data at the column offset from the row buffer loaded to Register "+params[pc][1];
                finish =start + ROW_ACCESS_DELAY*2 + COL_ACCESS_DELAY;
                DRAM[curr] = row_buffer;     // current row buffer copied back to DRAM
                row_buffer = DRAM[row];      // required row copied to row buffer
                curr = row; 
                num_rbf++;   
                
            }
            int pc_temp = pc;

            pc++;
            w.push_back(params[pc_temp][1]);        //registers that will be written in this instruction
            r.push_back(params[pc_temp][3]);        //registers that will be read in this instruction

            non_blocking();

            r.clear();
            w.clear();
            n_total=finish;

            out<<"Cycle "<<start+1<<"-"<<finish<<" --> "<<instructions[pc_temp]<<endl;
            out<<msg1<<endl;
            out<<msg2<<endl;
            reg[params[pc_temp][1]] = data_memory[(offset+reg[params[pc_temp][3]])];        //Data loaded from column offset to the required register
            
            print();  

        } else if (Instruction=="sw"){       // WRITE

            out<<"Cycle "<<n_total<<" --> "<<instructions[pc]<<endl;
            int offset = stoi(params[pc][2]);
            if((offset+reg[params[pc][3]])%4 != 0 || (offset+reg[params[pc][3]])<0  || (offset+reg[params[pc][3]])>DATA_MEMORY*4){
                cout<<"Program tried to access invalid memory location."<<endl;
                exit(-1);
            }
            out<<"DRAM request issued : WRITE"<<endl<<endl;
            
            start = n_total;
            
            int row = (offset+reg[params[pc][3]])/1024;
            
            if(curr==-1){
                finish =start + ROW_ACCESS_DELAY + COL_ACCESS_DELAY;
                curr = row;
                row_buffer=DRAM[row];
                num_rbf+=2;
                msg1 = "ROW "+to_string(row)+" activated";
                msg2 = "Data from Register "+params[pc][1]+" stored at the column offset in row buffer";
            } else if (curr == row){
                finish =start + COL_ACCESS_DELAY;
                num_rbf++;
                msg1 = "ROW "+to_string(row)+" already present in ROW BUFFER";
                msg2 = "Data from Register "+params[pc][1]+" stored at the column offset in row buffer";
            } else {
                msg1 = "ROW "+to_string(curr)+" copied back to DRAM and ROW "+to_string(row)+" activated";
                msg2 = "Data from Register "+params[pc][1]+" stored at the column offset in row buffer";
                finish =start + ROW_ACCESS_DELAY*2 + COL_ACCESS_DELAY;
                DRAM[curr] = row_buffer;
                row_buffer = DRAM[row];
                curr = row;
                num_rbf+=2;              
            }
            int pc_temp = pc;
            
            pc++;
            r.push_back(params[pc_temp][1]);        //registers that will be read in this instruction
            w.push_back(params[pc_temp][3]);        //registers that will be written in this instruction

            non_blocking();

            r.clear();
            w.clear();
            n_total=finish;

            out<<"Cycle "<<start+1<<"-"<<n_total<<" --> "<<instructions[pc_temp]<<endl;
            out<<msg1<<endl;
            out<<msg2<<endl;
            data_memory[(offset+reg[params[pc_temp][3]])] = reg[params[pc_temp][1]];
            row_buffer[((offset+reg[params[pc_temp][3]])%1024)/4] = reg[params[pc_temp][1]];        // row buffer update
            print();

        } else if (Instruction=="addi"){
            addi();  

        } else if( label.find(params[pc][0]) != label.end() ){
             pc ++;
             n_total --;

        }else{
            continue;
        }
        if(reg["$zero"] != 0){
            cout<<"The value in Zero Registor is not mutable. Command on line "<<pc<<" tries to change its value"<<endl;
            exit(-1);
        }
        
    }
    if(curr!=-1) DRAM[curr] = row_buffer;        //row buffer copied back to DRAM after the last lw/sw instruction
    // Prints the relevant statistics of the MIPS code. 
    cout<<"Total Number of clock cycles = "<<n_total<<endl;
    out<<"Total Number of clock cycles = "<<n_total<<endl;

    cout<<"Total Number of Row-buffer Updates = "<<num_rbf<<endl;
    out<<"Total Number of Row-buffer Updates = "<<num_rbf<<endl;

    //cout<<"Instruction Memory Used: "<<num*4 <<" Bytes"<<endl;
    out<<"Instruction Memory Used: "<<num*4 <<" Bytes"<<endl;

    //cout<<"Data Memory Used: "<<data_memory.size()*4 <<" Bytes"<<endl;
    out<<"Data Memory Used: "<<data_memory.size()*4 <<" Bytes"<<endl;

    //cout<<"Number of times each instruction was executed :"<<endl;
    out<<"Number of times each instruction was executed :"<<endl; 

    for (auto j: operations){
        //cout<< j<< " ->"<<statistics[j]<<endl;
        out<< j << " ->"<<statistics[j]<<endl;
    }
    //cout<< "j"<< " -> "<<statistics["j"]<<endl;
    out<< "j" << " -> "<<statistics["j"]<<endl;


    //cout<<"-----------------------------------------"<<endl;
    //cout<< "Data Memory at the end of Execution:" << endl;

    out<<"-----------------------------------------"<<endl;
    out<< "Data Memory at the end of Execution:" << endl;

    for(auto v : data_memory){
        //cout<<v.first<<"-"<<v.first+3<<": "<<v.second<<endl;
        out<<v.first<<"-"<<v.first+3<<": "<<v.second<<endl;
    }

    out.close();
	return 0;
}
//------------------------------------Execution Ends---------------------------------------------------------------------
