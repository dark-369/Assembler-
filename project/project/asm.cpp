/************************
 Title : Assembler
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
using namespace std;

const ll N = 1e5 + 10;
const ll M = 1e9 + 7;
const ll inf = 1e9 + 10;

map<string, pair<ll, ll>> opcodemapping;         // doing mapping
vector<pair<int, string>> error_list;            // storing all the errors
map<string, int> lable_store;                    // first is the name of the lable and the next is the pc value at the line where lable is present
vector<pair<string, string>> store_instructions; // storing the instructions
map<int, vector<string>> pc_and_instr;           // storing the instructions line by line
map<int, int> pc_line;                           // pc to line mapper

bool binary_check(string str);
bool octal_check(string str);
string octal_to_decimal(string str);
bool hexa_check(string s);
string hexa_to_decimal(string s);
bool decimal_check(string str);
bool is_valid_lable(string s);
bool isWhitespace(char c);
string ltrim(string s);
string rtrim(string s);
string trim(string s);
bool chspc(string s);
int operand_processor(string s, int line);
void fill_opcodetable();
void label_insert(int pc, string label, int line);
string hexadecimal_conversion(int num, int len);

void First_Pass(string in_file)
{
    ifstream passer;
    passer.open(in_file);
    string temp;
    pi checker;
    int program_counter = 0;
    int line = 0;
    while (getline(passer, temp))
    {
        temp = trim(temp);
        line++;
        // pc_and_instr[program_counter].pb(temp);
        // taking out the commented part;
        string s = "";
        for (int i = 0; i < temp.size(); i++)
        {
            if (temp[i] != ';')
            {
                s += temp[i];
            }
            else
            {
                break;
            }
        }
        temp = s;
        temp = trim(temp);

        string instruction = "";
        string operand = "";
        string labell = "";

        if (temp.find(':') != string::npos)
        {
            int colon = temp.find(':');
            labell = temp.substr(0, colon);
            string t1 = temp.substr(0, colon+1);
            labell = trim(labell);
            t1 = trim(t1);
            pc_and_instr[program_counter].pb(t1);
            if (labell.length())
            {
                label_insert(program_counter, labell, line);
                checker = mp(program_counter, labell);
            }
            temp = temp.substr(colon + 1, temp.length() - colon - 1);
            temp = trim(temp);
        }
        string temp1 = temp;
        trim(temp1);
        pc_and_instr[program_counter].pb(temp1);

        if (temp.find(' ') != string::npos)
        {   
            int space = temp.find(' ');
            instruction = temp.substr(0, space);
            instruction = trim(instruction);
            operand = temp.substr(space + 1, temp.length() - space - 1);
            operand = trim(operand);
        }
        else
        {
            instruction = trim(temp);
        }
        
            //    cout << "label " << labell << " " <<  "instruction " << instruction << " " << "operand " << operand << endl;

        // No instruction present so no need to increment the line
        if (!temp.length())
            continue;

        pc_line[program_counter] = line;

        if (!opcodemapping.count(instruction))
        {
            error_list.pb(mp(line, "This mnemonic is not valid " + instruction));
        }

        if (instruction == "SET")
        {
            if (checker.first != program_counter)
            {
                error_list.pb(mp(line, "Label doesn't exist: " + instruction));
            }
            else
            {
                lable_store[labell] = operand_processor(operand, line);
            }
        }

        if (!operand.length() && opcodemapping[instruction].first)
        {
            error_list.pb(mp(line, "Operand required here : " + instruction));
        }
        else if (operand.length() && !opcodemapping[instruction].first)
        {
            error_list.pb(mp(line, "Operand not required here: " + instruction + " " + operand));
        }

        store_instructions.pb(mp(instruction, operand));
        program_counter++;
    }
}

vector<pair<int, int>> Second_Pass()
{
    vector<pair<int, int>> opcode_operand_pair;
    int i = 0;
    while (i < store_instructions.size())
    {

        int l = pc_line[i];

        string instruction = store_instructions[i].first;
        string operand = store_instructions[i].second;

        int opcode = opcodemapping[instruction].second;

        int operand_int = 0;
        if (opcodemapping[instruction].first > 0)
            operand_int = operand_processor(operand, l);

        //  cout << i << "  " << operand << endl;
        if (instruction == "data")
        {   
            opcode = operand_int & (int)255;
            operand_int >>= 8;
        }

        opcode_operand_pair.pb(mp(opcode, operand_int));
        i++;
    }
    return opcode_operand_pair;
}

int main(int argc, char *argv[])
{

    // check to check whether only *.asm and *.cpp files are passed
    if (argc < 2)
    {
        cout << "Error, pass two files : \n *.asm file \n *.cpp file \n";
        return 0;
    }
    // calling the passing fucntion
    fill_opcodetable();

    string in_file = string(argv[1]);
    // calling the first pass by passing the in put file to it
    First_Pass(in_file);

    vector<pair<int, int>> opcode_operand_pair = Second_Pass();

    string extract_name = "";
    for (auto x : in_file)
    {
        if (x == '.')
            break;
        else
            extract_name.pb(x);
    }

    string object_file = extract_name + ".o";
    string listing_file = extract_name + ".lst";
    string log_file = extract_name + ".log";

    //    cout << error_list.size() << endl;

    ofstream lgptr(log_file), obptr(object_file, ios::binary | ios::out), ltptr(listing_file);

    if (error_list.size() > 0)
    {
        lgptr << "Compilation unsuccessful due to errors : \n";
        for (int i = 0; i < error_list.size(); i++)
        {
            lgptr << "line " << error_list[i].first << " : " << error_list[i].second << endl;
        }
    }
    else
    {
        lgptr << "Compilation is successful \n";
        int i = 0;
        // Those who take offsets, here have to find the distance
        set<int> offset_calc;
        offset_calc.insert(13);
        offset_calc.insert(15);
        offset_calc.insert(16);
        offset_calc.insert(17);
        while (i < opcode_operand_pair.size())
        {

            ltptr << hexadecimal_conversion(i, 8) << " ";

            for (int j = 0; j < pc_and_instr[i].size() - 1; j++)
            {
                string s = pc_and_instr[i][j];
                ltptr << "\t\t "
                      << " " << s << endl;
                ltptr << hexadecimal_conversion(i, 8) << " ";
            }

            int Opcode = opcode_operand_pair[i].first;
            int operand = opcode_operand_pair[i].second;

            if (offset_calc.count(Opcode))
            {
                operand -= i + 1;
            }

            ltptr << hexadecimal_conversion(operand, 6) << hexadecimal_conversion(Opcode, 2) << " " << pc_and_instr[i][pc_and_instr[i].size() - 1] << endl;
            obptr << hexadecimal_conversion(operand, 6) << hexadecimal_conversion(Opcode, 2);
            i++;
        }
    }
    lgptr.close();
    obptr.close();
    ltptr.close();

    return 0;
}

void fill_opcodetable()
{
    /* mapping of opcode table
    0 = Takes no operand, Exapmle : add instruction
    1 = Takes 1 operand and is value, Example: adc instruction
    2 = Takes 1 operand and is offset, Example: stnl instruction
       No opcode for data and SET
       opcode for data = -1
       opcode for set = -2
    */
    opcodemapping["ldc"] = {1, 0};
    opcodemapping["adc"] = {1, 1};
    opcodemapping["ldl"] = {2, 2};
    opcodemapping["stl"] = {2, 3};
    opcodemapping["ldnl"] = {2, 4};
    opcodemapping["stnl"] = {2, 5};
    opcodemapping["add"] = {0, 6};
    opcodemapping["sub"] = {0, 7};
    opcodemapping["shl"] = {0, 8};
    opcodemapping["shr"] = {0, 9};
    opcodemapping["adj"] = {1, 10};
    opcodemapping["a2sp"] = {0, 11};
    opcodemapping["sp2a"] = {0, 12};
    opcodemapping["call"] = {2, 13};
    opcodemapping["return"] = {0, 14};
    opcodemapping["brz"] = {2, 15};
    opcodemapping["brlz"] = {2, 16};
    opcodemapping["data"] = {1, -1};
    opcodemapping["br"] = {2, 17};
    opcodemapping["HALT"] = {0, 18};
    opcodemapping["SET"] = {1, -2};
}

bool octal_check(string str)
{
    // Check if the string is empty
    if (str.empty() || str.size() < 2)
    {
        return false;
    }
    // Check if the string starts with '0'
    if (str[0] != '0')
    {
        return false;
    }

    // Check if all characters are digits and are within the range '0' to '7'
    for (size_t i = 1; i < str.length(); ++i)
    {
        if (!isdigit(str[i]) || str[i] < '0' || str[i] > '7')
        {
            return false;
        }
    }

    // If all conditions pass, the string is octal
    return true;
}

bool hexa_check(string str)
{
    // Check if the string is empty
    if (str.empty())
    {
        return false;
    }

    // Check if the string starts with "0x" or "0X"
    if (str.length() > 2 && str[0] == '0' && str[1] == 'x')
    {
        // Starting from index 2, check if all characters are valid hexadecimal digits
        for (size_t i = 2; i < str.length(); ++i)
        {
            if (!isxdigit(str[i]))
            {
                return false;
            }
        }
        return true; // If all characters are valid hexadecimal digits
    }

    return false;
}

bool binary_check(string str)
{
    // Check if the string is empty or starts with "0b"
    if (str.empty() || (str.size() > 2 && str.substr(0, 2) != "0b") || (str.size() <= 2))
        return false;

    // Check each character in the string after the prefix
    for (size_t i = 2; i < str.size(); ++i)
    {
        if (str[i] != '0' || str[i] != '1') // If the character is not '0' or '1', it's not a valid binary digit
            return false;
    }
    return true;
}

bool decimal_check(string str)
{
    // Check if the string is empty
    if (str.empty())
    {
        return false;
    }
    // Check if the rest of the string contains only digits
    int startind = 0;
    if (str[0] == '-' || str[0] == '+')
        startind++;
    for (int i = startind; i < (int)str.length(); ++i)
    {
        if (!isdigit(str[i]))
        {
            return false;
        }
    }
    return true;
}

string hexa_to_decimal(string s)
{
    // Removing the '0x' prefix and converting to decimal
    stringstream ss;
    ss << std::hex << s;
    unsigned int decimal_number;
    ss >> decimal_number;

    // Converting the decimal number to a string
    std::stringstream decimal_ss;
    decimal_ss << decimal_number;
    return decimal_ss.str();
}

string octal_to_decimal(string s)
{
    // Converting the octal string to decimal
    std::stringstream ss;
    ss << std::oct << s;
    unsigned int decimal_number;
    ss >> decimal_number;

    // Converting the decimal number to a string
    std::stringstream decimal_ss;
    decimal_ss << decimal_number;
    return decimal_ss.str();
}

bool is_valid_lable(string s)
{
    int n = s.size();
    if (!((s[0] <= 'Z' && s[0] >= 'A') || (s[0] <= 'z' && s[0] >= 'a')))
        return false;
    for (int i = 1; i < n; i++)
    {
        if (!((s[0] <= 'Z' && s[0] >= 'A') || (s[0] <= 'z' && s[0] >= 'a') || (s[0] <= '9' && s[0] >= '0')))
            return false;
    }
    return true;
}

bool isWhitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

// Function to remove leading whitespace
string ltrim(string s)
{
    auto it = find_if(s.begin(), s.end(), [](char c)
                      { return !isWhitespace(c); });
    return string(it, s.end());
}

// Function to remove trailing whitespace
string rtrim(string s)
{
    auto it = std::find_if(s.rbegin(), s.rend(), [](char c)
                           { return !isWhitespace(c); });
    return string(s.begin(), it.base());
}

// Function to remove leading and trailing whitespace
string trim(string s)
{
    return rtrim(ltrim(s));
}

bool chspc(string s)
{
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == ' ')
            return true;
    }
    return false;
}

void label_insert(int pc, string label, int line)
{
    if (!is_valid_lable(label))
    {
        error_list.pb(mp(line, "invalid label name: " + label));
        return;
    }
    if (lable_store.find(label) == lable_store.end())
    {
        lable_store[label] = pc;
    }
    else
    {
        error_list.pb(mp(line, "duplicate label name: " + label));
    }
}

int operand_processor(string s, int line)
{
    if (is_valid_lable(s))
    {
        if (!lable_store.count(s))
        {
            error_list.pb(mp(line, "No label found : " + s));
        }
        return lable_store[s];
    }

    const char *start_string = s.c_str();
    char *end_string;
    int ans;
    // cout << start_string << endl;

    if (hexa_check(s))
    {   
        // cout << "Yes1" << endl;
        ans = strtol(start_string, &end_string, 16);
        return ans;
    }
    else if (binary_check(s))
    {   
        // cout << "Yes2" << endl;
        ans = strtol(start_string + 2, &end_string, 2);
        return ans;
    }
    else if (octal_check(s))
    {   
        // cout << "Yes3" << endl;
        ans = strtol(start_string, &end_string, 8);
        return ans;
    }
    else if (decimal_check(s))
    {   
        // cout << "Yes4" << endl;
        ans = strtol(start_string, &end_string, 10);
        return ans;
    }
    else
    {
        error_list.pb(mp(line, "Invalid expression: " + s));
        return 0;
    }
}

string hexadecimal_conversion(int num, int len)
{
    ostringstream os;
    os << hex << setw(len) << setfill('0') << num;
    string str = os.str();
    if (len < str.length())
    {
        str = str.substr(str.length() - len, len);
    }
    return str;
}
