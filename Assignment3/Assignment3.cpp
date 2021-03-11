#include<bits/stdc++.h>
#include<iostream>
using namespace std;

enum InstructionType {add,sub,mul,beq,bne,slt,j,lw,sw,addi};

struct Instruction{
    InstructionType instr;
    int src1;
    int src2;
    int dest;
};

class RegisterFile{

    int get(int index){
        return reg[index];
    }
    void clear(){
        for(int i=0;i<32;i++){
            reg[i]=0;
        }
    }

    void post(int index, int data){
        reg[index] = data;
    }


    // int get(string name){       
    //     if (name == "$0" || name == "$zero") return reg[0];
    //     else if (name == "$1" || name == "$r0") return reg[1];
    //     else if (name == "$2" || name == "$r1") return reg[2];   for the parser
    //     else if (name == "$3" || name == "$r2") return reg[3];
    //     else if (name == "$4" || name == "$r3") return reg[4];
    //     else if (name == "$5" || name == "$r4") return reg[5];
    //     else if (name == "$6" || name == "$r5") return reg[6];
    //     else if (name == "$7" || name == "$r6") return reg[7];
    //     else if (name == "$8" || name == "$r7") return reg[8];
    //     else if (name == "$9" || name == "$r8") return reg[9];
    //     else if (name == "$10" || name == "$r9") return reg[10];
    //     else if (name == "$11" || name == "$r10") return reg[11];
    //     else if (name == "$12" || name == "$r11") return reg[12];
    //     else if (name == "$13" || name == "$r12") return reg[13];
    //     else if (name == "$14" || name == "$r13") return reg[14];
    //     else if (name == "$15" || name == "$r14") return reg[15];
    //     else if (name == "$16" || name == "$r15") return reg[16];
    //     else if (name == "$17" || name == "$r16") return reg[17];
    //     else if (name == "$18" || name == "$r17") return reg[18];
    //     else if (name == "$19" || name == "$r18") return reg[19];
    //     else if (name == "$20" || name == "$r19") return reg[20];
    //     else if (name == "$21" || name == "$r20") return reg[21];
    //     else if (name == "$22" || name == "$r21") return reg[22];
    //     else if (name == "$23" || name == "$r22") return reg[23];
    //     else if (name == "$24" || name == "$r23") return reg[24];
    //     else if (name == "$25" || name == "$r24") return reg[25];
    //     else if (name == "$26" || name == "$r25") return reg[26];
    //     else if (name == "$27" || name == "$r26") return reg[27];
    //     else if (name == "$28" || name == "$r27") return reg[28];
    //     else if (name == "$29" || name == "$r28") return reg[29];
    //     else if (name == "$30" || name == "$r29") return reg[30];
    //     else if (name == "$31" || name == "$sp") return reg[31];
    //     else if (name == "$pc") return pc; 
    //     else throw "Invalid Register address";
    // }

    private:
        int reg[32] = {0};
};

class Memory{

    void clear() {
        memset(Mem,0,sizeof(Mem));
    }

    int get(int index){
        return Mem[index];
    }

    void post(int index, int data){
        Mem[index] = data;
    }

    private:
        int Mem[262144] = {0};
};

class Simulator{
    public:
        void loadinstructions(string fileName){
            string line;
            ifstream file(fileName);
            if (file.is_open())
            {
                while (getline(file, line))
                {
                    string::iterator end_pos = remove(line.begin(), line.end(), ' ');
                    line.erase(end_pos, line.end());
                    end_pos = remove(line.begin(), line.end(), '\t');
                    line.erase(end_pos, line.end());
                    int comment = line.find('#');
                    if(comment!=string::npos)
                        line = line.substr(0,comment);
                    if (line.empty())
                    {
                        continue;
                    }
                    else
                    {
                        bool cont=parse(line);    // This line is without comment and without spaces.
                        if (!cont) return;
                    }
                }
            }
            else
            {
                cout << "File does not exist\nPlease enter correct file name\n";
            }
        }

    private :
        Memory mem;
        RegisterFile registers;
        int pc=0; 
        vector<Instruction> instructions;
        unordered_map<string,int> labels;

        bool parse(string line){
            // Parse the string into instruction....
            int l = line.find(':');
            if (l!=string::npos){
            if (l=0){
            cout<<"empty label name"<<endl;
            return false;
            }
            string label = line.substr(0,l-1);
            labels[label]=pc;
            }
            int len=(line.length())-1;
            if(l==len){return true;}
            int i=line.find(",");
            if(i==string::npos){
            string ins=line.substr(l+1,len);
            if(ins.at(0)!='j'){
            cout<<"invalid instruction"<<endl;
            return false;
            }
            if(l+1==len){cout<<"no label specified for jump instruction"<<endl;return false;}
            string det=line.substr(l+2,len);
            if (labels.find(det)==labels.end()){cout<<"incorrect jump address"<<endl;return false;}
            Instruction ist;
            ist.instr=j;
            ist.dest=labels.at(det);
            instructions.push_back(ist);
            pc++;
            return true;
            }
            else{}
            return true;
        }
        
        //void refresh(){
        //mem.clear();
        //registers.clear();
        //pc=0;
        //}

        
};


int main(int argc, char* argv[])
{   
    string fileName = (argc>1)?argv[1]:"code.txt";
    Simulator sim;
    sim.loadinstructions(fileName);    
    return 0;
}
