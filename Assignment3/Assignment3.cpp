#include <bits/stdc++.h>
#include <iostream>
using namespace std;

enum InstructionType
{
    add,
    sub,
    mul,
    slt,
    beq,
    bne,
    j,
    lw,
    sw,
    addi,
    illegal
};

struct Instruction
{
    InstructionType instr;
    int src1;
    int src2;
    int dest;
    int imvalue;
    string jumplabel;
};

class RegisterFile
{
public:
    int get(int index)
    {
        return reg[index];
    }
    void clear()
    {
        for (int i = 0; i < 31; i++)
        {
            reg[i] = 0;
        }
        reg[31] = 262143;
    }

    void post(int index, int data)
    {
        reg[index] = data;
    }

    void print(){
        cout<<"$zero : "<<setfill('0')<<setw(32)<<hex<<reg[0]<<" | ";
        for(int i=1;i<31;i++){
            cout<<"$r"<<i-1<<" : "<<setfill('0')<<setw(32)<<hex<<reg[i]<<" | ";
        }
        cout<<"$sp : "<<setfill('0')<<setw(32)<<hex<<reg[31]<<"\n";
    }

    RegisterFile(){
        clear();
    }

private:
    int reg[32] = {0};
};

class Memory
{
public:
    void clear()
    {
        memset(Mem, 0, sizeof(Mem));
    }

    int get(int index)
    {
        return Mem[index];
    }

    void post(int index, int data)
    {
        Mem[index] = data;
    }

private:
    int Mem[262144] = {0};
};

class Simulator
{
public:
    void loadinstructions(string fileName)
    {
        clear();
        string line;
        ifstream file(fileName);
        if (file.is_open())
        {
            int i = 1;
            while (getline(file, line))
            {
                Removespace(line);
                int comment = line.find('#');
                if (comment != string::npos)
                    line = line.substr(0, comment);
                if (line.empty())
                {
                    i++;
                    continue;
                }
                else
                {
                    bool cont = parse(line); // This line is without comment and without spaces.
                    if (!cont)
                    {
                        cout << i;
                        file.close();
                        abort();
                    }
                    i++;
                }
            }
            file.close();
        }
        else
        {
            cout << "File does not exist\nPlease enter correct file name\n";
            abort();
        }
        for (int i = 0; i < instructions.size(); i++)
        {
            Instruction instruction = instructions[i];
            if (instruction.instr >= 4 && instruction.instr <= 6)
            {
                if (instruction.dest == -1 && labels.find(instruction.jumplabel) != labels.end())
                {
                    instructions[i].dest = labels[instruction.jumplabel];
                }
                if (instructions[i].dest < 0 || instructions[i].dest > instructions.size())
                {
                    cout << " Illegal jump at instruction " << i + 1 << "\n";
                    abort();
                }
            }
        }
    }
    void run(){
        pc=0;
        clock=0;
        while(pc<instructions.size() && clock<200){
            num_times[pc+1] = (num_times.find(pc+1)==num_times.end())? 1 : num_times[pc+1]+1;
            bool done = execute(instructions[pc]);
            if(!done){
                cout<<pc<<"\n";
                abort();
            }
            clock++;
            registers.print();
        }
        for(auto x : num_times){
            cout<<"Instruction "<<x.first<<" is run "<<x.second<<" times\n";
        }
    }

    void clear(){
        instructions.clear();
        labels.clear();
        mem.clear();
        registers.clear();
        pc=0;
        clock=0;
    }
    Simulator(){
        clear();
    }

private:
    Memory mem;
    RegisterFile registers;
    int pc = 0;
    int clock = 0;
    vector<Instruction> instructions;
    unordered_map<string, int> labels;
    unordered_map<int,int> num_times;

    bool execute(Instruction inst){
        pc++;
        switch(inst.instr){
            case InstructionType::add :  
                if(inst.dest==0){
                    cout<<"Error : zero register can not be modified at instruction ";
                    return false;
                }
                if(inst.src2==-1){
                    registers.post(inst.dest,registers.get(inst.src1)+inst.imvalue);
                    return true;
                }
                else{
                    registers.post(inst.dest,registers.get(inst.src1)+registers.get(inst.src2));
                    return true;
                }
                                         break;
            case InstructionType::sub :  
                if(inst.dest==0){
                    cout<<"Error : zero register can not be modified at instruction ";
                    return false;
                }
                if(inst.src2==-1){
                    registers.post(inst.dest,registers.get(inst.src1)-inst.imvalue);
                    return true;
                }
                else{
                    registers.post(inst.dest,registers.get(inst.src1)-registers.get(inst.src2));
                    return true;
                }                      
                                         break;
            case InstructionType::mul :   
                if(inst.dest==0){
                    cout<<"Error : zero register can not be modified at instruction ";
                    return false;
                }
                if(inst.src2==-1){
                    registers.post(inst.dest,registers.get(inst.src1)*inst.imvalue);
                    return true;
                }
                else{
                    registers.post(inst.dest,registers.get(inst.src1)*registers.get(inst.src2));
                    return true;
                }
                                         break;
            case InstructionType::slt :  
                                        
                                         break;
            case InstructionType::beq :   
                                        
                                         break;
            case InstructionType::bne :   
                                        
                                         break;
            case InstructionType::j   :   
                                        
                                         break;
            case InstructionType::lw  :   
                                        
                                         break;
            case InstructionType::sw  :   
                                        
                                         break;
            case InstructionType::addi:   
                                        
                                         break;
            default: return false;
                     break;
        }
        return true;
    }

    void Removespace(string &str)
    {
        int i = 0;
        while (i < str.length() && (str[i] == ' ' || str[i] == '\t'))
            i++;
        str = str.substr(i);
    }

    InstructionType findop(string op)
    {
        if (op == "add")
            return InstructionType(add);
        else if (op == "sub")
            return InstructionType(sub);
        else if (op == "mul")
            return InstructionType(mul);
        else if (op == "slt")
            return InstructionType(slt);
        else if (op == "beq")
            return InstructionType(beq);
        else if (op == "bne")
            return InstructionType(bne);
        else if (op == "j")
            return InstructionType(j);
        else if (op == "lw")
            return InstructionType(lw);
        else if (op == "sw")
            return InstructionType(sw);
        else if (op == "addi")
            return InstructionType(addi);
        else
            return InstructionType(illegal);
    }

    int isvalid(string name)
    {
        if (name == "$0" || name == "$zero")
            return 0;
        else if (name == "$1" || name == "$r0")
            return 1;
        else if (name == "$2" || name == "$r1")
            return 2;
        else if (name == "$3" || name == "$r2")
            return 3;
        else if (name == "$4" || name == "$r3")
            return 4;
        else if (name == "$5" || name == "$r4")
            return 5;
        else if (name == "$6" || name == "$r5")
            return 6;
        else if (name == "$7" || name == "$r6")
            return 7;
        else if (name == "$8" || name == "$r7")
            return 8;
        else if (name == "$9" || name == "$r8")
            return 9;
        else if (name == "$10" || name == "$r9")
            return 10;
        else if (name == "$11" || name == "$r10")
            return 11;
        else if (name == "$12" || name == "$r11")
            return 12;
        else if (name == "$13" || name == "$r12")
            return 13;
        else if (name == "$14" || name == "$r13")
            return 14;
        else if (name == "$15" || name == "$r14")
            return 15;
        else if (name == "$16" || name == "$r15")
            return 16;
        else if (name == "$17" || name == "$r16")
            return 17;
        else if (name == "$18" || name == "$r17")
            return 18;
        else if (name == "$19" || name == "$r18")
            return 19;
        else if (name == "$20" || name == "$r19")
            return 20;
        else if (name == "$21" || name == "$r20")
            return 21;
        else if (name == "$22" || name == "$r21")
            return 22;
        else if (name == "$23" || name == "$r22")
            return 23;
        else if (name == "$24" || name == "$r23")
            return 24;
        else if (name == "$25" || name == "$r24")
            return 25;
        else if (name == "$26" || name == "$r25")
            return 26;
        else if (name == "$27" || name == "$r26")
            return 27;
        else if (name == "$28" || name == "$r27")
            return 28;
        else if (name == "$29" || name == "$r28")
            return 29;
        else if (name == "$30" || name == "$r29")
            return 30;
        else if (name == "$31" || name == "$sp")
            return 31;
        else
            return -1;
    }
    bool checkNum(string num)
    {
        int j = num.length() - 1;
        while (j >= 0 && (num[j] == ' ' || num[j] == '\t'))
            j--;
        string str = num.substr(0, j + 1);
        for (int i = 0; i < str.length(); i++)
        {
            if (!isdigit(str[i]))
                return false;
        }

        return true;
    }

    bool valName(string name)
    {
        for (int i = 0; i < name.length(); i++)
        {
            if (!(isalpha(name[i]) || name[i] == 95))
                return false;
        }
        return true;
    }

    bool parse(string line)
    {
        // Parse the string into instruction....
        int l = line.find(':');
        if (l != string::npos)
        {
            if (l == 0)
            {
                cout << "Syntax Error : empty label name at line ";
                return false;
            }
            int j = l - 1;
            while (j >= 0 && line[j] == ' ' || line[j] == '\t')
            {
                j--;
            }
            string label = line.substr(0, j + 1);
            // Could check validity of label
            if (labels.find(label) != labels.end())
            {
                if (labels[label] != -1)
                {
                    cout << "Syntax Error : Same label defined again at line  ";
                    return false;
                }
            }
            if (!valName(label))
            {
                cout << "invalid label name at line";
                return false;
            }
            labels[label] = instructions.size(); //variable rakh lena chahiye mere khayal se size ke liye
            int len = (line.length()) - 1;
            if (l == len)
            {
                return true;
            }
        }

        string ins = line.substr(l + 1);
        Removespace(ins);

        if (ins.find(':') != -1)
        {
            cout << "Syntax Error at line ";
            return false;
        }

        int i = 0;
        while (i < 4 && ins[i] != ' ' && ins[i] != '\t' && ins[i] != '\0')
            i++;

        string op = ins.substr(0, i);
        if (ins.length() <= i) //ye kam ka hai
        {
            cout << "Syntax Error at line ";
            return false;
        }
        string operands = ins.substr(i);
        Removespace(operands);
        InstructionType oper = findop(op);
        if (oper == 10)
        {
            cout << "Syntax Error : Invalid instruction at line ";
            return false;
        }

        if (oper < 4)
        { // For add, sub, mul, slt                                //changed something here
            int i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            int j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            string reg = operands.substr(0, j + 1);
            int reg1 = isvalid(reg);
            if (reg1 == -1)
            {
                cout << " Syntax Error:invalid register at line ";
                return false;
            }
            operands = operands.substr(i + 1);
            Removespace(operands);
            i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(i + 1);
            Removespace(operands);
            i = operands.length(); //yaha se kuch changes kiye hai do , hi hote hai
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            int reg3 = isvalid(reg);
            if (reg3 == -1)
            {
                //if (reg.length()==1)
                //{
                //Instruction instr;
                //instr.instr = oper;
                //instr.dest = reg1;
                //instr.src1 = reg2;
                //instr.src2 = reg3;
                //instr.imvalue=reg.at(0);
                //instructions.push_back(instr);
                //return true;
                //}
                if (checkNum(reg))
                { //ye function likhna padega
                    Instruction instr;
                    instr.instr = oper;
                    instr.dest = reg1;
                    instr.src1 = reg2;
                    instr.src2 = -1;
                    instr.imvalue = stoi(reg);
                    instructions.push_back(instr);
                    return true;
                }
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 1);
            Removespace(operands);
            if (!operands.empty())
            {
                cout << " Syntax Error at line ";
                return false;
            }
            Instruction instr;
            instr.instr = oper;
            instr.dest = reg1;
            instr.src1 = reg2;
            instr.src2 = reg3;
            instructions.push_back(instr);
            return true;
        }
        else if (oper < 6) // for beq, bne
        {
            int i = operands.find(',');
            int j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            string reg = operands.substr(0, j + 1);
            int reg1 = isvalid(reg);
            if (reg1 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            i = operands.find(',');
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            int x = operands.length() - 1;
            while (x >= 0 && operands[x] == ' ' || operands[x] == '\t')
                x--;
            operands = operands.substr(0, x + 1);
            if (operands.empty())
            {
                cout << "Syntax Error at line ";
            }
            Instruction inst;
            inst.instr = oper;
            inst.src1 = reg1;
            inst.src2 = reg2;
            int jump;
            char *p;

            if (checkNum(operands))
            {
                jump = stoi(operands);
                if (jump < 0)
                {
                    cout << " Invalid jump address at line ";
                    return false;
                }
                inst.dest = jump;
            }
            else
            {
                if (labels.find(operands) == labels.end())
                    labels[operands] = -1;
                jump = labels[operands];
                inst.dest = jump;
                inst.jumplabel = operands;
            }
            instructions.push_back(inst);
            return true;
        }
        else if (oper < 7) // for j
        {
            int x = operands.length() - 1;
            while (x >= 0 && operands[x] == ' ' || operands[x] == '\t')
                x--;
            operands = operands.substr(0, x + 1);
            if (operands.empty())
            {
                cout << "Syntax Error at line ";
            }
            Instruction inst;
            inst.instr = oper;
            int jump;
            char *p;
            if (checkNum(operands))
            {
                jump = stoi(operands);
                if (jump < 0)
                {
                    cout << " Invalid jump address at line ";
                    return false;
                }
                inst.dest = jump;
            }
            else
            {
                if (labels.find(operands) == labels.end())
                    labels[operands] = -1;
                jump = labels[operands];
                inst.dest = jump;
                inst.jumplabel = operands;
            }
            instructions.push_back(inst);
            return true;
        }
        else if (oper < 9) //for lw, sw
        {
            int i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            int j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            string reg = operands.substr(0, j + 1);
            int reg1 = isvalid(reg);
            if (reg1 == -1)
            {
                cout << " Syntax Error:invalid register at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            i = operands.find('(');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            int offset = 0;
            if (j >= 0)
            {
                string reg = operands.substr(0, j + 1);
                if (checkNum(reg))
                { //ye function likhna padega
                    offset = stoi(reg);
                }
                else
                {
                    cout << " Syntax Error at line ";
                    return false;
                }
            }
            string operands = operands.substr(i + 1);
            i = operands.find(')');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            if (!operands.empty())
            {
                cout << " Syntax Error at line ";
                return false;
            }
            Instruction instr;
            instr.instr = oper;
            instr.imvalue = offset;
            instr.dest = reg1;
            instr.src1 = reg2;
            instr.src2 = -1;
            instructions.push_back(instr);
            return true;
        }
        else
        {
            int i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            int j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            string reg = operands.substr(0, j + 1);
            int reg1 = isvalid(reg);
            if (reg1 == -1)
            {
                cout << " Syntax Error:invalid register at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line";
                return false;
            }
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            i = operands.length(); //yaha se kuch changes kiye hai do , hi hote hai
            j = i - 1;
            while (j >= 0 && operands[j] == ' ' || operands[j] == '\t')
            {
                j--;
            }
            reg = operands.substr(0, j + 1);
            if (checkNum(reg))
            { //ye function likhna padega
                Instruction instr;
                instr.instr = oper;
                instr.dest = reg1;
                instr.src1 = reg2;
                instr.src2 = -1;
                instr.imvalue = stoi(reg);
                instructions.push_back(instr);
                return true;
            }
            else
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            if (!operands.empty())
            {
                cout << " Syntax Error at line ";
                return false;
            }
            Instruction instr;
            instr.instr = oper;
            instr.dest = reg1;
            instr.src1 = reg2;
            instr.src2 = -1;
            instructions.push_back(instr);
            return true;
        }
    }

    // Baaki operations ke liye upar jaise codes likh de ifelse krke | Neeche wala part delete kr skte hai...wo galat hai
    // Ek function bana jo kisi bhi string ko check kre ki valid characters hai ya nhi usme label ke

    //void refresh(){
    //mem.clear();
    //registers.clear();
    //pc=0;
    //}
};

int main(int argc, char *argv[])
{
    string fileName = (argc > 1) ? argv[1] : "code.txt";
    Simulator sim;
    sim.loadinstructions(fileName); 
    sim.run();
    return 0;
}
