#include <bits/stdc++.h>
#include <iostream>
using namespace std;

int ROW_ACCESS_DELAY;
int COL_ACCESS_DELAY;

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
//Structure used to pass instrution in queue 
struct Mem_instr
{
    Instruction inst;
    int row;
    int inst_num;
    int num_to_write;
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
        for (int i = 0; i < 32; i++)
        {
            reg[i] = 0;
        }
        reg[29] = 1048572;
    }

    void post(int index, int data)
    {
        reg[index] = data;
    }

    void print(bool hext)
    {
        if (hext)
        {
            cout << "\t";
            for (int i = 1; i < 31; i++)
            {
                cout << getname(i) << hex << "0x" << reg[i] << " | ";
                if (i == 7 || i == 15 || i == 23 || i == 31)
                    cout << "\n\t";
            }
        }
        else
        {
            cout << "\t";
            for (int i = 0; i < 32; i++)
            {
                cout << getname(i) << " : " << reg[i] << " | ";
                if (i == 7 || i == 15 || i == 23 || i == 31)
                    cout << "\n\t";
            }
        }
    }

    string getname(int index)
    {
        return names[index];
    }

    RegisterFile()
    {
        clear();
    }

private:
    int reg[32] = {0};
    string names[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$s8", "$ra"};
};

class Memory
{
public:
    void clear()
    {
        Mem = {{0}};
        rowbuffer = {0};
        row = -1;
        bufferupdates = 0;
        process_end = 0;
    }

    int get_current_row()
    {
        return row;
    }

    int get_process_end()
    {
        return process_end;
    }

    void set_process_end(int end)
    {
        process_end = end;
    }

    int get_without_buffer(int index)
    {
        int row_required = index / 256;
        if (row == row_required)
        {
            return rowbuffer[index % 256];
        }
        return Mem[index / 256][index % 256];
    }

    pair<int, int> get(int index)
    {
        int row_req = index / 256;
        int col_req = index % 256;
        if (row_req == row)
        {
            return make_pair(rowbuffer[col_req], COL_ACCESS_DELAY);
        }
        else
        {
            bufferupdates++;
            if (row != -1)
            {
                Mem[row] = rowbuffer;
                rowbuffer = Mem[row_req];
                row = row_req;
                return make_pair(rowbuffer[col_req], 2 * ROW_ACCESS_DELAY + COL_ACCESS_DELAY);
            }
            else
            {
                rowbuffer = Mem[row_req];
                row = row_req;
                return make_pair(rowbuffer[col_req], ROW_ACCESS_DELAY + COL_ACCESS_DELAY);
            }
        }
    }

    int post(int index, int data)
    {
        int row_req = index / 256;
        int col_req = index % 256;
        bufferupdates++;
        if (row_req == row)
        {
            rowbuffer[col_req] = data;
            return COL_ACCESS_DELAY;
        }
        else
        {
            bufferupdates++;
            if (row != -1)
            {
                Mem[row] = rowbuffer;
                rowbuffer = Mem[row_req];
                rowbuffer[col_req] = data;
                row = row_req;
                return (2 * ROW_ACCESS_DELAY + COL_ACCESS_DELAY);
            }
            else
            {
                rowbuffer = Mem[row_req];
                rowbuffer[col_req] = data;
                row = row_req;
                return (ROW_ACCESS_DELAY + COL_ACCESS_DELAY);
            }
        }
    }

    int getbuffer()
    {
        return bufferupdates;
    }

private:
    array<array<int, 256>, 1024> Mem = {{0}};
    array<int, 256> rowbuffer = {0};
    int row = -1;
    int bufferupdates = 0;
    int process_end = 0;
};

class Simulator
{
public:
    bool loadinstructions(string fileName)
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
                        cout << i << endl;
                        file.close();
                        return false;
                    }
                    i++;
                }
            }
            file.close();
        }
        else
        {
            cout << "File does not exist\nPlease enter correct file name\n";
            return false;
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
                if (instructions[i].dest < 0 || instructions[i].dest - 32 > instructions.size())
                {
                    cout << " Illegal jump at instruction " << i + 1 << "\n";
                    return false;
                }
            }
        }
        return true;
    }
    // COMPLETELY MODIFIED FROM MINOR
    void run()
    {
        pc = 0;
        clock = 0;
        cout << "Every Cycle description :\n\n";
        while (pc < instructions.size() || !(Mem_instructions.empty()))
        {
            ++clock;

            Run_Memory();
            // CPU FETCHES INSTRUCTION AND RUNS IT IF INDEPENDENT
            if (pc < instructions.size())
            {
                Instruction ins = instructions[pc];
                bool independent = is_independent(ins);
                if (independent)
                {
                    cout << "\tcycle " << clock << " : ";
                    pair<bool, int> done = execute(ins);
                    if (!done.first)
                    {
                        cout << pc << "\n";
                        return;
                    }
                    if (mem.get_process_end() == clock)
                        Mem_instructions.erase(current_mem);
                }
                else
                    wait_for_DRAM();
            }
            else
                wait_for_DRAM();


        }
        printstatistics();
    }

    void clear()
    {
        instructions.clear();
        labels.clear();
        mem.clear();
        registers.clear();
        pc = 0;
        clock = 0;
        hex = false;
        mem_modified.clear();
        Mem_instructions.clear();
    }
    Simulator()
    {
        clear();
    }

private:
    Memory mem;
    RegisterFile registers;
    int pc = 0;
    int clock = 0;
    vector<Instruction> instructions;
    unordered_map<string, int> labels;
    unordered_map<int, int> num_times;
    bool hex = false;
    vector<int> mem_modified;
    // LIST OF MEMORY INSTRUCTIONS
    list<Mem_instr> Mem_instructions;
    list<Mem_instr>::iterator current_mem;

    // RUNNING MEMORY INSTRUCTIONS FROM LIST IF MEMORY IS IDLE
    void Run_Memory()
    {
        //Firstly check for any memory instruction in queue which can be executed
        if (clock > mem.get_process_end() && !(Mem_instructions.empty()))
        {
            pair<bool, int> done_mem;
            if (mem.get_current_row() == -1)
            {
                done_mem = execute_mem_instruction(Mem_instructions.begin());
            }
            else
            {
                bool toexecute = true;
                list<Mem_instr>::iterator it = Mem_instructions.begin();
                while (it != Mem_instructions.end())
                {
                    if ((*it).row == mem.get_current_row())
                    {
                        list<Mem_instr>::iterator it2 = Mem_instructions.begin();

                        while (it2 != it)
                        {
                            if (((*it2).inst.instr == InstructionType::lw) && ((*it2).inst.dest == (*it).inst.dest))
                            {
                                break;
                            }
                            it2++;
                        }
                        if (it2 == it)
                        {
                            toexecute = false;
                            done_mem = execute_mem_instruction(it);
                        }

                        break;
                    }
                    it++;
                }
                if (toexecute)
                    done_mem = execute_mem_instruction(Mem_instructions.begin());
            }
        }
    }
    // CPU WAITS FOR DRAM IF NEXT INSTRUCTION CANNOT BE EXECUTED WITHOUT DRAM COMPLETION
    void wait_for_DRAM()
    {
        if ((mem.get_process_end() - clock) > 0)
        {
            cout << "\tcycle " << (clock) << "-" << mem.get_process_end() << " : Waiting for DRAM to return\n";
        }
        else if (mem.get_process_end() == clock)
        {
            cout << "\tcycle " << (clock) << " : Waiting for DRAM to return\n";
        }
        clock = mem.get_process_end();
        Mem_instructions.erase(current_mem);
    }
    // TO CHECK DEPENDENCY OF INSTRUCTION -- TAKEN FROM MINOR
    bool is_independent(Instruction ins)
    {
        if (ins.instr == InstructionType::sw || ins.instr == InstructionType::lw)
        {
            return true;
        }
        else
        {
            list<Mem_instr>::iterator it = Mem_instructions.begin();
            while (it != Mem_instructions.end())
            {
                if ((*it).inst.instr == InstructionType::lw)
                {
                    int orig_register = (*it).inst.dest;
                    if (ins.dest == orig_register || ins.src1 == orig_register || ins.src2 == orig_register)
                    {
                        return false;
                    }
                }
                it++;
            }
        }
        return true;
    }
    // EXECUTE MEM INSTRUCTION
    pair<bool, int> execute_mem_instruction(list<Mem_instr>::iterator iter)
    {
        Instruction inst = (*iter).inst;
        int address = (*iter).inst_num;
        current_mem = iter;
        pair<bool, int> answer = make_pair(true, 1);
        int index1, index2;
        if (inst.instr == InstructionType::lw)
        {
            index1 = inst.imvalue + registers.get(inst.src1);
            pair<int, int> ans = mem.get(index1 / 4);
            registers.post(inst.dest, ans.first);
            answer.second = ans.second;
            printcycledata(answer, inst, address);
            mem.set_process_end(clock + answer.second - 1);
            return answer;
        }
        else if (inst.instr == InstructionType::sw)
        {
            index2 = inst.imvalue + registers.get(inst.src1);
            int time = mem.post(index2 / 4, (*iter).num_to_write);
            answer.second = time;
            printcycledata(answer, inst, address);
            mem.set_process_end(clock + answer.second - 1);
            return answer;
        }
        else
        {
            cout << " Invalid Instruction ";
            answer.first = false;
            return answer;
        }
    }
    // PRINT END STATISTICS
    void printstatistics()
    {
        cout << "\nStatistics :\n\n";
        cout << "\tNumber of clock cycles is " << clock << "\n";
        cout << "\tNumber of Buffer updates is " << mem.getbuffer() << "\n\n";
        cout << "Final register states : \n\n";
        registers.print(hex);
        cout<<"\n";
        if (!mem_modified.empty())
        {
            sort(mem_modified.begin(), mem_modified.end());
            vector<int>::iterator mem_m;
            mem_m = unique(mem_modified.begin(), mem_modified.end());
            mem_modified.resize(distance(mem_modified.begin(), mem_m));
            cout << "Memory content at the end :\n\n";
            for (int i = 0; i < mem_modified.size(); i++)
            {
                cout << "\t" << mem_modified[i] << "-" << mem_modified[i] + 3 << " : " << mem.get_without_buffer(mem_modified[i] / 4) << "\n";
            }
        }
        else
        {
            cout << "No Memory accesses \n";
        }
    }
    // PRINT CYCLE DATA FOR MEMORY INSTRUCTIONS WHEN THEY ARE EXECUTED BY MEMORY -- TAKEN FROM MINOR
    void printcycledata(pair<bool, int> done, Instruction ins, int address)
    {
        if (done.second > 1)
        {
            cout << "\tcycle " << (clock) << "-" << (clock + done.second - 1) << " : ";
            if (ins.instr == InstructionType::lw)
            {
                cout << "\tDRAM :\t" << registers.getname(ins.dest) << " = " << registers.get(ins.dest) << " ; ";
                mem_modified.push_back(ins.imvalue + registers.get(ins.src1));
            }
            else
            {
                int index2 = ins.imvalue + registers.get(ins.src1);
                cout << "\tDRAM :\tmemory address " << index2 << "-" << index2 + 3 << " = " << mem.get_without_buffer(index2 / 4) << " ; ";

                mem_modified.push_back(index2);
            }
        }
        else if (done.second == 1)
        {
            cout << "\tcycle " << (clock) << " : ";
            if (ins.instr == InstructionType::lw)
            {
                cout << "\tDRAM :\t" << registers.getname(ins.dest) << " = " << registers.get(ins.dest) << " ; ";
                mem_modified.push_back(ins.imvalue + registers.get(ins.src1));
            }
            else
            {
                int index2 = ins.imvalue + registers.get(ins.src1);
                cout << "\tDRAM :\tmemory address " << index2 << "-" << index2 + 3 << " = " << mem.get_without_buffer(index2 / 4) << " ; ";
                mem_modified.push_back(index2);
            }
        }
        if (done.second == COL_ACCESS_DELAY)
        {
            cout << "Buffer contains the required row"
                 << " ; Instruction address : " << address << "\n";
            if (done.second > 1)
            {
                cout << "\tcycle " << (clock) << "-" << (clock + done.second - 1) << " : \tDRAM :\tAccess column\n";
            }
            else if (done.second == 1)
            {
                cout << "\tcycle " << (clock) << " : \tDRAM :\tAccess column\n";
            }
        }
        else if (done.second == ROW_ACCESS_DELAY + COL_ACCESS_DELAY)
        {
            cout << "Buffer is Empty. Required row from memory is copied to buffer"
                 << " ; Instruction address : " << address << "\n";
            if (ROW_ACCESS_DELAY > 1)
                cout << "\tcycle " << (clock) << "-" << (clock + ROW_ACCESS_DELAY - 1) << " : \tDRAM :\tActivate required row(" + to_string(mem.get_current_row()) + ") to buffer\n";
            else
                cout << "\tcycle " << (clock) << " : \tDRAM :\tActivate required row to buffer\n";

            if (COL_ACCESS_DELAY > 1)
                cout << "\tcycle " << (clock + ROW_ACCESS_DELAY) << "-" << (clock + done.second - 1) << " : \tDRAM :\tAccess column\n";
            else
                cout << "\tcycle " << (clock + ROW_ACCESS_DELAY) << " : \tDRAM :\tAccess column\n";
        }
        else if (done.second == (2 * ROW_ACCESS_DELAY + COL_ACCESS_DELAY))
        {
            cout << "Buffer is copied back and required row is copied to buffer"
                 << " ; Instruction address : " << address << "\n";
            if (ROW_ACCESS_DELAY > 1)
            {
                cout << "\tcycle " << (clock) << "-" << (clock + ROW_ACCESS_DELAY - 1) << " : \tDRAM :\tCopy the buffer back\n";
                cout << "\tcycle " << (clock + ROW_ACCESS_DELAY) << "-" << (clock + 2 * ROW_ACCESS_DELAY - 1) << " : \tDRAM :\tActivate required row(" + to_string(mem.get_current_row()) + ") to buffer\n";
            }
            else
            {
                cout << "\tcycle " << (clock) << " : \tDRAM :\tCopy the buffer back\n";
                cout << "\tcycle " << (clock + ROW_ACCESS_DELAY) << " : \tDRAM :\tActivate required row(" + to_string(mem.get_current_row()) + ") to buffer\n";
            }
            if (COL_ACCESS_DELAY > 1)
                cout << "\tcycle " << (clock + 2 * ROW_ACCESS_DELAY) << "-" << (clock + done.second - 1) << " : \tDRAM :\tAccess column\n";
            else
                cout << "\tcycle " << (clock + 2 * ROW_ACCESS_DELAY) << " : \tDRAM :\tAccess column\n";
        }
    }
    //Same execution for instructions other than lw and sw
    pair<bool, int> execute(Instruction inst)
    {
        pc++;
        pair<bool, int> answer = make_pair(true, 1);
        int index1, index2;
        switch (inst.instr)
        {
        case InstructionType::add:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            if (inst.src2 == -1)
            {
                registers.post(inst.dest, registers.get(inst.src1) + inst.imvalue);
                cout << "\tadd :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            else
            {
                registers.post(inst.dest, registers.get(inst.src1) + registers.get(inst.src2));
                cout << "\tadd :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            break;
        case InstructionType::sub:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            if (inst.src2 == -1)
            {
                registers.post(inst.dest, registers.get(inst.src1) - inst.imvalue);
                cout << "\tsub :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            else
            {
                registers.post(inst.dest, registers.get(inst.src1) - registers.get(inst.src2));
                cout << "\tsub :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            break;
        case InstructionType::mul:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            if (inst.src2 == -1)
            {
                registers.post(inst.dest, registers.get(inst.src1) * inst.imvalue);
                cout << "\tmul :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            else
            {
                registers.post(inst.dest, registers.get(inst.src1) * registers.get(inst.src2));
                cout << "\tmul :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                return answer;
            }
            break;
        case InstructionType::slt:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            if (inst.src2 == -1)
            {
                if (registers.get(inst.src1) < inst.imvalue)
                {
                    registers.post(inst.dest, 1);
                    cout << "\tslt :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
                else
                {
                    registers.post(inst.dest, 0);
                    cout << "\tslt :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }
            else
            {
                if (registers.get(inst.src1) < registers.get(inst.src2))
                {
                    registers.post(inst.dest, 1);
                    cout << "\tslt :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
                else
                {
                    registers.post(inst.dest, 0);
                    cout << "\tslt :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }
            break;
        case InstructionType::beq:
            if (inst.src2 == -1)
            {
                if (registers.get(inst.src1) == inst.imvalue)
                {
                    cout << "\tbeq :\tJumped"
                         << " ; Instruction address : " << pc << "\n";
                    pc = inst.dest - 32;
                    return answer;
                }
                else
                {
                    cout << "\tbeq : \tNot Jumped"
                         << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }
            else
            {
                if (registers.get(inst.src1) == registers.get(inst.src2))
                {
                    cout << "\tbeq :\tJumped"
                         << " ; Instruction address : " << pc << "\n";
                    pc = inst.dest - 32;
                    return answer;
                }
                else
                {
                    cout << "\tbeq :\tNot Jumped"
                         << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }

            break;
        case InstructionType::bne:
            if (inst.src2 == -1)
            {
                if (registers.get(inst.src1) != inst.imvalue)
                {
                    cout << "\tbne :\tJumped"
                         << " ; Instruction address : " << pc << "\n";
                    pc = inst.dest - 32;
                    return answer;
                }
                else
                {
                    cout << "\tbne :\tNot Jumped"
                         << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }
            else
            {
                if (registers.get(inst.src1) != registers.get(inst.src2))
                {
                    cout << "\tbne :\tJumped"
                         << " ; Instruction address : " << pc << "\n";
                    pc = labels[inst.jumplabel] - 32;
                    return answer;
                }
                else
                {
                    cout << "\tbne :\tNot Jumped"
                         << " ; Instruction address : " << pc << "\n";
                    return answer;
                }
            }
            break;
        case InstructionType::j:
            cout << "\tj :\tJumped"
                 << " ; Instruction address : " << pc << "\n";
            pc = inst.dest - 32;
            return answer;
            break;
        case InstructionType::lw:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            index1 = inst.imvalue + registers.get(inst.src1);
            if (index1 >= 0 && index1 <= 1048572 && index1 % 4 == 0)
            {
                //pair<int, int> ans = mem.get(index1 / 4);
                //registers.post(inst.dest, ans.first);
                Mem_instructions.push_back({inst, index1 / 1024, pc, -1});
                cout << "\tlw :\tDRAM request issued"
                     << " ; Instruction address : " << pc << " \n";
                //answer.second = ans.second + 1;
                return answer;
            }
            else
            {
                cout << "Invalid address at instruction ";
                answer.first = false;
                return answer;
            }
            break;
        case InstructionType::sw:
            index2 = inst.imvalue + registers.get(inst.src1);
            if (index2 >= 0 && index2 <= 1048572 && index2 % 4 == 0)
            {
                //int time = mem.post(index2 / 4, registers.get(inst.dest));
                Mem_instructions.push_back({inst, index2 / 1024, pc, registers.get(inst.dest)});
                cout << "\tsw : \tDRAM request issued"
                     << " ; Instruction address : " << pc << " \n";
                //answer.second = time + 1;
                return answer;
            }
            else
            {
                cout << "Invalid address at instruction ";
                answer.first = false;
                return answer;
            }
            break;
        case InstructionType::addi:
            if (inst.dest == 0)
            {
                cout << "Error : zero register can not be modified at instruction ";
                answer.first = false;
                return answer;
            }
            registers.post(inst.dest, registers.get(inst.src1) + inst.imvalue);
            cout << "\taddi :\t" << registers.getname(inst.dest) << " = " << registers.get(inst.dest) << " ; Instruction address : " << pc << "\n";
            return answer;
            break;

        default:
            cout << " Invalid Instruction ";
            answer.first = false;
            return answer;
            break;
        }
        answer.first = false;
        return answer;
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
        else if (name == "$at" || name == "$r0" || name == "$1")
            return 1;
        else if (name == "$v0" || name == "$r1" || name == "$2")
            return 2;
        else if (name == "$v1" || name == "$r2" || name == "$3")
            return 3;
        else if (name == "$a0" || name == "$r3" || name == "$4")
            return 4;
        else if (name == "$a1" || name == "$r4" || name == "$5")
            return 5;
        else if (name == "$a2" || name == "$r5" || name == "$6")
            return 6;
        else if (name == "$a3" || name == "$r6" || name == "$7")
            return 7;
        else if (name == "$t0" || name == "$r7" || name == "$8")
            return 8;
        else if (name == "$t1" || name == "$r8" || name == "$9")
            return 9;
        else if (name == "$t2" || name == "$r9" || name == "$10")
            return 10;
        else if (name == "$t3" || name == "$r10" || name == "$11")
            return 11;
        else if (name == "$t4" || name == "$r11" || name == "$12")
            return 12;
        else if (name == "$t5" || name == "$r12" || name == "$13")
            return 13;
        else if (name == "$t6" || name == "$r13" || name == "$14")
            return 14;
        else if (name == "$t7" || name == "$r14" || name == "$15")
            return 15;
        else if (name == "$s0" || name == "$r15" || name == "$16")
            return 16;
        else if (name == "$s1" || name == "$r16" || name == "$17")
            return 17;
        else if (name == "$s2" || name == "$r17" || name == "$18")
            return 18;
        else if (name == "$s3" || name == "$r18" || name == "$19")
            return 19;
        else if (name == "$s4" || name == "$r19" || name == "$20")
            return 20;
        else if (name == "$s5" || name == "$r20" || name == "$21")
            return 21;
        else if (name == "$s6" || name == "$r21" || name == "$22")
            return 22;
        else if (name == "$s7" || name == "$r22" || name == "$23")
            return 23;
        else if (name == "$t8" || name == "$r23" || name == "$24")
            return 24;
        else if (name == "$t9" || name == "$r24" || name == "$25")
            return 25;
        else if (name == "$k0" || name == "$r25" || name == "$26")
            return 26;
        else if (name == "$k1" || name == "$r26" || name == "$27")
            return 27;
        else if (name == "$gp" || name == "$r27" || name == "$28")
            return 28;
        else if (name == "$sp" || name == "$r28" || name == "$29")
            return 29;
        else if (name == "$s8" || name == "$r29" || name == "$30")
            return 30;
        else if (name == "$ra" || name == "$r30" || name == "$31")
            return 31;
        else
            return -1;
    }
    bool checkNum(string num)
    {
        Removespace(num);
        int j = num.length() - 1;
        if (j == -1)
        {
            cout << "invalid/empty argument ";
            return false;
        }
        while (j >= 0 && (num[j] == ' ' || num[j] == '\t'))
            j--;
        string str = num.substr(0, j + 1);
        int l = str.length();
        for (int i = 0; i < l; i++)
        {
            if (i == 0 && l > 1)
            {
                if (isdigit(str[i]) || str[i] == '+' || str[i] == '-')
                    continue;
                else
                {
                    return false;
                }
            }
            else
            {
                if (!isdigit(str[i]))
                    return false;
            }
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
                cout << "invalid label name at line ";
                return false;
            }
            labels[label] = instructions.size() + 32; //variable rakh lena chahiye mere khayal se size ke liye
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
        if (ins.length() == 0)
        {
            return true;
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
        { // For add, sub, mul, slt                                //changed something here
            int i = operands.find(',');
            if (i == -1)
            {
                cout << "invalid instruction at line ";
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
                cout << "invalid instruction at line ";
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
            operands = operands.substr(i + 1);
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
                cout << "invalid instruction at line ";
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
            i = operands.find('(');
            if (i == -1)
            {
                int j = operands.length() - 1;
                while (j >= 0 && (operands[j] == ' ' || operands[j] == '\t'))
                {
                    j++;
                }
                string reg = operands.substr(0, j + 1);
                if (checkNum(reg))
                {
                    Instruction instr;
                    instr.instr = oper;
                    instr.imvalue = stoi(reg);
                    instr.dest = reg1;
                    instr.src1 = 0;
                    instr.src2 = -1;
                    instructions.push_back(instr);
                    return true;
                    ;
                }
                else
                {
                    cout << " Syntax Error at line ";
                    return false;
                }
                return true;
            }
            j = i - 1;
            while (j >= 0 && (operands[j] == ' ' || operands[j] == '\t'))
            {
                j--;
            }
            int offset = 0;
            if (j >= 0)
            {
                string reg = operands.substr(0, j + 1);
                if (checkNum(reg))
                {
                    offset = stoi(reg);
                }
                else
                {
                    cout << " Syntax Error at line ";
                    return false;
                }
            }
            operands = operands.substr(i + 1);
            i = operands.find(')');
            if (i == -1)
            {
                cout << "invalid instruction at line ";
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
                cout << "invalid instruction at line ";
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
                cout << "invalid instruction at line ";
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
            instr.src2 = -1;
            instructions.push_back(instr);
            return true;
        }
    }
};

int main(int argc, char *argv[])
{
    ROW_ACCESS_DELAY = (argc > 1) ? stoi(argv[1]) : 10;
    COL_ACCESS_DELAY = (argc > 2) ? stoi(argv[2]) : 2;
    if (ROW_ACCESS_DELAY <= 0 || COL_ACCESS_DELAY <= 0)
    {
        cout << "Invalid DRAM access delay(s)\n";
        return 0;
    }
    string fileName = (argc > 3) ? argv[3] : "demo.txt";

    Simulator sim;
    if (sim.loadinstructions(fileName))
    {
        sim.run();
    }
    return 0;
}
