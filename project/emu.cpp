/************************
 Title : Emulator
 Author : Harsh Dahiya
 Roll number : 2201CS30
Declaration of Authorship
This cpp file, asm.cpp, is part of the miniproject of CS210 at the department of Computer Science and Engg, IIT Patna
Taught by : Dr.Jimson Mathew
*************************/

#include <iostream>
#include <algorithm>
#include <math.h>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <cstring>
#include <stack>
#include <climits>
#include <queue>
#include <iomanip>
#include <tuple>
#include <cassert>
#include <bitset>
#include <cstdio>
#include <numeric>
#include <fstream>
#include <sstream>
#include <cstdlib>

#pragma GCC target("popcnt")

#define ll long long int
#define mp make_pair
#define pb push_back
#define F first
#define S second
#define all(x) (x).begin(), (x).end()
#define endl "\n"
#define lp(n, v)               \
    for (ll i = 0; i < n; i++) \
    {                          \
        ll x;                  \
        cin >> x;              \
        v.emplace_back(x);     \
    }
#define vi vector<int>
#define pi pair<int, string>
#define itt int32_t

const ll N = 1e5 + 10;
const ll M = 1e9 + 7;
const ll inf = 1e9 + 10;
using namespace std;

map<int, string> opcodemapping;
int memory_tab[10000], reg1 = 0, reg2 = 0, program_counter = 0, stack_pointer = 9999;

void fill_opcodetable();
void instruction_set_architecture();
string hexadecimal_conversion(int n);
void memory_tab_dump(ofstream &tf, int program_counter);
void monitor_flow(ofstream &tf, int program_counter);

// Define functions for each opCode
void ldc(int opr)
{
    reg2 = reg1;
    reg1 = opr;
}

void adc(int opr)
{
    reg1 += opr;
}

void ldl(int opr)
{
    reg2 = reg1;
    reg1 = memory_tab[stack_pointer + opr];
}

void stl(int opr)
{
    memory_tab[stack_pointer + opr] = reg1;
    reg1 = reg2;
}

void ldnl(int opr)
{
    reg1 = memory_tab[reg1 + opr];
}

void stnl(int opr)
{
    memory_tab[reg1 + opr] = reg2;
}

void add()
{
    reg1 += reg2;
}

void sub()
{
    reg1 = reg2 - reg1;
}

void shl()
{
    reg1 = reg2 << reg1;
}

void shr()
{
    reg1 = reg2 >> reg1;
}

void adj(int opr)
{
    stack_pointer += opr;
}

void a2sp()
{
    stack_pointer = reg1;
    reg1 = reg2;
}

void sp2a()
{
    reg2 = reg1;
    reg1 = stack_pointer;
}

void call(int opr)
{
    reg2 = reg1;
    reg1 = program_counter;
    program_counter += opr;
}

void returnInstruction(int opr, bool &when_to_halt)
{
    if (program_counter == reg1 && reg1 == reg2)
        when_to_halt = true;
    program_counter = reg1;
    reg1 = reg2;
}

void brz(int opr)
{
    if (reg1 == 0)
        program_counter += opr;
}

void brlz(int opr)
{
    if (reg1 < 0)
        program_counter += opr;
}

void br(int opr)
{
    program_counter += opr;
}

void HALT(bool &when_to_halt)
{
    when_to_halt = true;
}

void print_out_message()
{
    cout << "Instructions for use:\n./emu [option] file.o\n";
    cout << "[option] can be one of the following:\n";
    cout << "\t-t : print trace of the code executed." << endl;
    cout << "\t-b : Print memory_tab dump before program execution." << endl;
    cout << "\t-a : Show memory_tab dump after program execution." << endl;
    cout << "\t-ISA\tdisplay ISA" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_out_message();
        return 0;
    }

    fill_opcodetable();

    string mode = argv[1], in_file = argv[2];

    string trf = in_file.substr(0, in_file.find('.')) + ".trace";

    ifstream inptr(in_file);
    ofstream outptr(trf);

    string ln;
    getline(inptr, ln);
    // File is empty
    if (!ln.length())
    {
        cout << "File is empty\n";
        exit(-1);
    }

    int l_no = 0, maxi_op = stoi("ff", nullptr, 16), ind = 0, maximum_operand = stoi("ffffff", nullptr, 16);

    while (ind < ln.length())
    {
        string hexadecimal_code_value = ln.substr(ind, 8);
        unsigned long value_int_hexadecimal = stoul(hexadecimal_code_value, nullptr, 16);
        int instruction = static_cast<int>(value_int_hexadecimal);

        string xyz = hexadecimal_code_value.substr(6, hexadecimal_code_value.length());
        int32_t opc = stoi(xyz, nullptr, 16);
        if (xyz[0] >= '8')
        {
            opc = -(maxi_op - opc + 1);
        }

        xyz = hexadecimal_code_value.substr(0, 6);
        int32_t opr = stoi(xyz, nullptr, 16);
        if (xyz[0] >= '8')
        {
            opr = -(maximum_operand - opr + 1);
        }

        if (opc < 0)
        {
            memory_tab[l_no] = opr;
        }
        else
        {
            memory_tab[l_no] = instruction;
        }
        l_no++;
        ind += 8;
    }

    if (mode[1] == 'b')
    {
        memory_tab_dump(outptr, l_no);
    }
    if (mode[1] == 't')
    {
        monitor_flow(outptr, program_counter);
    }
    if (mode[1] == 'a')
    {
        monitor_flow(outptr, program_counter);
        memory_tab_dump(outptr, l_no);
    }

    instruction_set_architecture();

    return 0;
}

void fill_opcodetable()
{
    opcodemapping[0] = "ldc";
    opcodemapping[1] = "adc";
    opcodemapping[2] = "ldl";
    opcodemapping[3] = "stl";
    opcodemapping[4] = "ldnl";
    opcodemapping[5] = "stnl";
    opcodemapping[6] = "add";
    opcodemapping[7] = "sub";
    opcodemapping[8] = "shl";
    opcodemapping[9] = "shr";
    opcodemapping[10] = "adj";
    opcodemapping[11] = "a2sp";
    opcodemapping[12] = "sp2a";
    opcodemapping[13] = "call";
    opcodemapping[14] = "return";
    opcodemapping[15] = "brz";
    opcodemapping[16] = "brlz";
    opcodemapping[17] = "br";
    opcodemapping[18] = "when_to_halt";
    opcodemapping[-1] = "data";
    opcodemapping[-2] = "SET";
}

void instruction_set_architecture()
{
    cout << "The Instruction Set Architecture is (ISA) :\n";
    cout << "Opcode = 'not applicable'   Mnemonic =  data    operand =  value\n";
    cout << "Opcode = '0 '                 Mnemonic =  ldc     operand =  value\n";
    cout << "Opcode = '1 '                 Mnemonic =  adc     operand =  value\n";
    cout << "Opcode = '2 '                 Mnemonic =  ldl     operand =  value\n";
    cout << "Opcode = '3 '                 Mnemonic =  stl     operand =  value\n";
    cout << "Opcode = '4 '                 Mnemonic =  ldnl    operand =  value\n";
    cout << "Opcode = '5 '                 Mnemonic =  stnl    operand =  value\n";
    cout << "Opcode = '6 '                 Mnemonic =  add     operand = 'not present' \n";
    cout << "Opcode = '7 '                 Mnemonic =  sub     operand = 'not present' \n";
    cout << "Opcode = '8 '                 Mnemonic =  shl     operand = 'not present' \n";
    cout << "Opcode = '9 '                 Mnemonic =  shr     operand = 'not present' \n";
    cout << "Opcode = '10'                 Mnemonic =  adj     operand =  value\n";
    cout << "Opcode = '11'                 Mnemonic =  a2sp    operand = 'not present' \n";
    cout << "Opcode = '12'                 Mnemonic =  sp2a    operand = 'not present' \n";
    cout << "Opcode = '13'                 Mnemonic =  call    operand =  offset\n";
    cout << "Opcode = '14'                 Mnemonic =  return  operand = 'not present' \n";
    cout << "Opcode = '15'                 Mnemonic =  brz     operand =  offset\n";
    cout << "Opcode = '16'                 Mnemonic =  brlz    operand =  offset\n";
    cout << "Opcode = '17'                 Mnemonic =  br      operand =  offset\n";
    cout << "Opcode = '18'                 Mnemonic =  when_to_halt    operand = 'not present' \n";
    cout << "Opcode = 'not applicable'     Mnemonic =  SET     operand =  value\n";
}

// Routine to convert integers to hex and store in string
string hexadecimal_conversion(int n)
{
    int size = 8;
    ostringstream os;
    os << hex << setw(size) << setfill('0') << n;
    string str = os.str();
    if (size < str.length())
    {
        str = str.substr(str.length() - size, size);
    }
    return str;
}

void memory_tab_dump(ofstream &tf, int program_counter)
{
    cout << "\n\t\t   Dumping from memory_tab   \t\t\n";
    tf << "\n\t\t   Dumping from memory_tab   \t\t\n";
    for (int i = 0; i < program_counter; i++)
    {
        if (i % 4)
        {
            cout << hexadecimal_conversion(memory_tab[i]) << " ";
            tf << hexadecimal_conversion(memory_tab[i]) << " ";
        }
        else
        {
            cout << "\n"
                 << hexadecimal_conversion(i) << "\t" << hexadecimal_conversion(memory_tab[i]) << " ";
            tf << "\n"
               << hexadecimal_conversion(i) << "\t" << hexadecimal_conversion(memory_tab[i]) << " ";
        }
    }
    cout << endl;
}

void monitor_flow(ofstream &tf, int program_counter)
{
    cout << "\n\t\t---Tracing instructions---\t\t\n\n";
    tf << "\n\t\t---Tracing instructions---\t\t\n\n";
    set<int> offect_opcode;
    offect_opcode.insert(13);
    offect_opcode.insert(15);
    offect_opcode.insert(16);
    offect_opcode.insert(17);

    int line_number = 0;

    bool when_to_halt = false;

    for (;;)
    {
        itt instr = memory_tab[program_counter];

        unsigned long hex_value = stoul("ffffffff", nullptr, 16);
        itt all_max = static_cast<itt>(hex_value);

        int maxi_op = stoi("ff", nullptr, 16);

        itt tempor_opc = instr & 0xff;
        string abhikeliye = hexadecimal_conversion(tempor_opc);
        abhikeliye = abhikeliye.substr(6, 2);

        itt op_code = stoi(abhikeliye, nullptr, 16);
        if (abhikeliye[0] >= '8')
        {
            op_code = -(maxi_op - op_code + 1);
        }

        itt opr = instr & 0xffffff00;
        if (opr & (1 << 31))
        {
            opr = -(all_max - opr + 1);
        }
        opr >>= 8;

        if (opcodemapping.count(op_code))
        {
            cout << "PC: " << hexadecimal_conversion(program_counter) << "\tSP: " << hexadecimal_conversion(stack_pointer) << "\tA: "
                 << hexadecimal_conversion(reg1) << "\tB: " << hexadecimal_conversion(reg2) << "\t" << opcodemapping[op_code]
                 << " " << opr << endl
                 << endl;

            tf << "PC: " << hexadecimal_conversion(program_counter) << "\tSP: " << hexadecimal_conversion(stack_pointer) << "\tA: "
               << hexadecimal_conversion(reg1) << "\tB: " << hexadecimal_conversion(reg2) << "\t" << opcodemapping[op_code]
               << " " << opr << endl
               << endl;
        }
        else
        {
            when_to_halt = true;
        }

        if (op_code == 0)
        {
            ldc(opr);
        }
        else if (op_code == 1)
        {
            adc(opr);
        }
        else if (op_code == 2)
        {
            ldl(opr);
        }
        else if (op_code == 3)
        {
            stl(opr);
        }
        else if (op_code == 4)
        {
            ldnl(opr);
        }
        else if (op_code == 5)
        {
            stnl(opr);
        }
        else if (op_code == 6)
        {
            add();
        }
        else if (op_code == 7)
        {
            sub();
        }
        else if (op_code == 8)
        {
            shl();
        }
        else if (op_code == 9)
        {
            shr();
        }
        else if (op_code == 10)
        {
            adj(opr);
        }
        else if (op_code == 11)
        {
            a2sp();
        }
        else if (op_code == 12)
        {
            sp2a();
        }
        else if (op_code == 13)
        {
            call(opr);
        }
        else if (op_code == 14)
        {
            returnInstruction(opr, when_to_halt);
        }
        else if (op_code == 15)
        {
            brz(opr);
        }
        else if (op_code == 16)
        {
            brlz(opr);
        }
        else if (op_code == 17)
        {
            br(opr);
        }
        else if (op_code == 18)
        {
            HALT(when_to_halt);
        }

        if (stack_pointer >= 10000)
        {
            cout << "SP exceeding the memory_tab_tab at PC: " << program_counter << endl;
            when_to_halt = true;
        }
        if (offect_opcode.count(op_code) && opr == -1)
        {
            cout << "Infinite loop detected" << endl;
            when_to_halt = true;
        }
        if (when_to_halt)
            break;
        program_counter++;
        line_number++;
    }
    cout << line_number << " number of instructions executed!" << endl;
}