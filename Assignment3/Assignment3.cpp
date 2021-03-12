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
    addi
};

struct Instruction
{
    InstructionType instr;
    int src1;
    int src2;
    int dest;
    string jumplabel;
};

class RegisterFile
{

    int get(int index)
    {
        return reg[index];
    }
    void clear()
    {
        for (int i = 0; i < 32; i++)
        {
            reg[i] = 0;
        }
    }

    void post(int index, int data)
    {
        reg[index] = data;
    }

private:
    int reg[32] = {0};
};

class Memory
{

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
        string line;
        ifstream file(fileName);
        if (file.is_open())
        {
            int i=1;
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
                    if (!cont){
                        cout<<i;
                        abort();
                    }
                    i++;
                }
            }
        }
        else
        {
            cout << "File does not exist\nPlease enter correct file name\n";
        }
    }

private:
    Memory mem;
    RegisterFile registers;
    int pc = 0;
    vector<Instruction> instructions;
    unordered_map<string, int> labels;

    void Removespace(string &str)
    {
        int i = 0;
        while (i < str.length() && (str[j] == ' ' || str[j] == '\t'))
            j++;
        str = str.substr(j);
    }

    InstructionType findop(string op)
    {
        switch (op)
        {
        case "add":
            return 0;
            break;
        case "sub":
            return 1;
            break;
        case "mul":
            return 2;
            break;
        case "slt":
            return 3;
            break;
        case "beq":
            return 4;
            break;
        case "bne":
            return 5;
            break;
        case "j":
            return 6;
            break;
        case "lw":
            return 7;
            break;
        case "sw":
            return 8;
            break;
        case "addi":
            return 9;
            break;
        default:
            return 10;
            break;
        }
    }

    int isvalid(string reg)
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
        else if (name == "$5" || name == "$r4") return 5];
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

    bool parse(string line)
    {
        // Parse the string into instruction....
        int l = line.find(':');
        if (l != string::npos)
        {
            if (l = 0)
            {
                cout << "Syntax Error : empty label name at line ";
                return false;
            }
            j = l - 1;
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
            labels[label] = instructions.size();
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
        if (ins.length() <= i)
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
        { // For add, sub, mul
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
            string reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
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
            string reg = operands.substr(0, j + 1);
            int reg3 = isvalid(reg);
            if (reg3 == -1)
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
            instr.src2 = reg3;
            instructions.push_back(instr);
            return true;
        }
        else if (oper < 6)
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
            string reg = operands.substr(0, j + 1);
            int reg2 = isvalid(reg);
            if (reg2 == -1)
            {
                cout << " Syntax Error at line ";
                return false;
            }
            operands = operands.substr(j + 2);
            Removespace(operands);
            int x = operands.length()-1;
            while(x>=0 && operands[x]==' ' || operands[x] == '\t')
                x--;
            operands = operands.substr(0,x+1);
            if(operands.empty()){
                cout<< "Syntax Error at line ";
            }
            Instruction inst;
            inst.instr = oper;
            inst.src1 = reg1;
            inst.src2 = reg2;
            int jump;
            char *p;
            if (strtol(operands.c_str(), &p) == 0)
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
            return true;
        }

        // Baaki operations ke liye upar jaise codes likh de ifelse krke | Neeche wala part delete kr skte hai...wo galat hai
        int i = line.find(",");
        if (i == string::npos)
        {
            string ins = line.substr(l + 1, len);
            if (ins.at(0) != 'j')
            {
                cout << "Syntax Error : Invalid instruction at line ";
                return false;
            }
            if (ins.length() < 2)
            {
                cout << "Syntax Error : No label specified for jump instruction at line ";
                return false;
            }
            string dest = ins.substr(1);
            Instruction ist;
            ist.instr = j;
            int jump;
            char *p;
            if (strtol(dest.c_str(), &p) == 0)
            {
                jump = stoi(dest);
                if (jump < 0)
                {
                    cout << " Invalid jump address at line ";
                    return false;
                }
                ist.dest = jump;
            }
            else
            {
                if (labels.find(dest) == labels.end())
                    labels[dest] = -1;
                jump = labels[dest];
                ist.dest = jump;
                ist.jumplabel = dest;
            }
            instructions.push_back(ist);
            return true;
        }
        else
        {
        }
        return true;
    }

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
    return 0;
}
