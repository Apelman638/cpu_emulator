#include <cstdint>
#include <iostream>
#include <fstream>
#include <map>
#include <bitset>
#include "gpu.h"

#define SUP_SHIFT 28
#define FLIP_SHIFT 27
#define OP_SHIFT 24
#define DEST_SHIFT 16
#define R1_SHIFT 8

using namespace std;

//logic componets in c++

int bit_adder(unsigned int a, unsigned int b) {
    int sum1 = a ^ b;
    int carry = (a & b) << 1;
    if(carry != 0) {
        return bit_adder(sum1, carry);
    }
    return sum1;
}

struct set_bitsize {
    struct small {
    unsigned int small : 3;
    }; struct large {
    unsigned int large : 20;
    }; struct byte {
    unsigned int byte : 4;
    }; struct bit_2 {
    unsigned int bit_2 : 2;
    };
};

int selector_2bit(set_bitsize::small input) {
    if ((input.small & (1<<2)) == (1<<2)) {
        return ((1<<1) & input.small);
    }
    if ((input.small & 1<<2) == 0) {
        return(1 & input.small);
    }
    return 0;
}

int16_t byte_selector(set_bitsize::byte input){ // decoder
    return 1<<input.byte; // if selection is 6, it outputs the 7th bit

}
//these selectors can be made much more optimized apparently

int bit_selector(int16_t selection, int16_t input) {
    return (selection) & (input); // i dont even need to make this
}

int switch_2bit(set_bitsize::bit_2 input){ 
    //outputs first bit in location given by 2nd bit (either 1st ouput bit or 2nd)
    switch (input.bit_2) {
        case 0b01: return 1;
        case 0b10: return 0;
        case 0b11: return 2;
        case 0b00: return 0;
        default: return 0;
    }
}

/*
create a set of register
make registers to hold the commands

new alu input:

0b0000 0000 00000000 00000000 00000000 
        op    des      reg1     reg2
last 4 bits determine where these input go, 0000 is alu1 0001 is alu2, still the rest are unused

i want to make it so that you can save instructions to a register
perhaps ill just have it automatically save them 
*/

int registers[256] = {0};
long registers_64[256]; // for very large input (strings)
uint32_t instruct_memory[0x1000] = {0};
uint32_t memory[0x1000] = {0};

int pc = 0; // program counter


int get_dest(uint32_t input) {
    return (input>>DEST_SHIFT & 0xff);
}

map<uint8_t, char> chars = {
    {0,' '},{1,'a'}, {2,'b'},{3,'c'},{4,'d'},{5,'e'},{6,'f'},{7,'g'},{8,'h'},
    {9,'i'},{10,'j'},{11,'k'},{12,'l'},{13,'m'},{14,'n'},{15,'o'},{16,'p'},
    {17,'q'},{18,'r'},{19,'s'},{20,'t'},{21,'u'},{22,'v'},{23,'w'},{24,'x'},
    {25,'y'},{26,'z'},
    {27,'A'},{28,'B'},{29,'C'},{30,'D'},{31,'E'},{32,'F'},
    {33,'G'},{34,'H'},{35,'I'},{36,'J'},{37,'K'},{38,'L'},{39,'M'},{40,'N'},
    {41,'O'},{42,'P'},{43,'Q'},{44,'R'},{45,'S'},{46,'T'},{47,'U'},{48,'V'},
    {49,'W'},{50,'X'},{51,'Y'},{52,'Z'},
    {53,'0'},{54,'1'},{55,'2'},{56,'3'},{57,'4'},{58,'5'},{59,'6'},{60,'7'},
    {61,'8'},{62,'9'},
    {63,'.'},{64,'\''},{65,'\"'},{66,','},{67,'/'},{68,';'},{69,':'},{70,'?'},
    {71,'<'},{72,'>'},{73,'!'},{74,'@'},{75,'#'},{76,'$'},{77,'%'},{78,'^'},
    {79,'&'},{80,'*'},{81,'('},{82,')'},{83,'-'},{84,'_'},{85,'+'},{86,'='},
    {87,'{'},{88,'}'},{89,'['},{90,']'},{91,'|'},{92,'\\'},{93,'`'},{94,'~'},
    {95,'\t'},{96,'\n'},{97,'\r'},{98,'\f'},{99,'\a'},{100,'\b'},{101,'\v'}
};

char access_char(uint8_t input){
    return chars[input]; 
}

string word_accesser(long input) {
    int i = 56;
    string word;
    while(i >= 0){
        word += access_char(input>>i  & 0xff);
        i-=8;
    }
    return word;
}

//updated alu that uses registers
void alu(uint32_t input) {
    /*
    can do:

    addition 000
    subtraction 001
    or 010
    and 011
    not 100
    xor 101
    nand 110
    pass 111
    */
    cout << "alu1: " << endl;
    set_bitsize::byte i;
    i.byte = input>>OP_SHIFT  & 0x7; // operation 
    int op = byte_selector(i); // 0b00001 is the flip operation, 0b00000111 is the operation
    int dest = get_dest(input); //0xff = 0b11111111. is a regular number that gets used like [dest]
    int num1 = registers[(input>>R1_SHIFT & 0xff)]; //takes the number out of the register and the register address ([num])
    int num2 = registers[(input & 0xff)]; //registers[addr: r2] 

    //if the flip bit is on it flips num1 and num2
    int first, sec;
    first = num1;
    sec = num2;
    if ((input>>FLIP_SHIFT) & 1) {
        first = num2;
        sec = num1;
    }

    switch (op) {
        case 1<<0 :
            cout << "into " << dest << " put" << " add " << num1 << " " << num2 << endl;
            registers[dest] = bit_adder(first, sec);//addition;
            break;
        case 1<<1 :
            cout << "into " << dest << " put" <<  " sub " << num1 << " " << num2 << endl;
            registers[dest] = bit_adder(first, -sec);//subtraction;
            break;
        case 1<<2 :
            cout << "into " << dest << " put" << " or " << num1 << " " << num2 << endl;
            registers[dest] = first | sec; //or;
            break;
        case 1<<3 :
            cout << "into " << dest << " put" <<  " and " << num1 << " " << num2 << endl;
            registers[dest] = first & sec;//and;
            break;
        case 1<<4 :
            cout << "into " << dest << " put" << " not " << num1 << " " << num2 << endl;
            registers[dest] = ~first; //not;
            break;
        case 1<<5 :
            cout << "into " << dest << " put" <<  " xor " << num1 << " " << num2 << endl;
            registers[dest]= first ^ sec; //xor
            break;;
        case 1<<6 :
            cout << "into " << dest << " put" << " pass " << num1 << " " << num2 << endl;
            registers[dest] = first; //pass;
            break;
        default:
            cout << "failed" << endl;
    }
}

void alu2(uint32_t input) {
    cout << "alu2: " << endl;
    /*
    = set 000
    > grt 001
    < less 010
    == comp 011
    +1 inc 100
    -1 dinc 101
    <<1 rsh 110
    >>1 lsh 111 
    0b 0001 0 000 00000000 00000000 00000000
      alu2 flip op   dest     r1      r2

    0001 0 000 00000001 00000101 00000000
    alu2 n set    d1        5       x
    sets r1 to be 5
    */
    set_bitsize::byte i;
    i.byte = input>>OP_SHIFT  & 0x7; // operation 
    int op = byte_selector(i); // 0b00001 is the flip operation, 0b00000111 is the operation
    int dest = get_dest(input); //0xff = 0b11111111. is a regular number that gets used like [dest]
    int num1 = registers[(input>>R1_SHIFT & 0xff)]; //takes the number out of the register and the register address ([num])
    int num2 = registers[(input & 0xff)]; 

    //if the flip bit is on it flips num1 and num2
    int first, sec;
    first = num1;
    sec = num2;
    if ((input>>FLIP_SHIFT) & 1) {
        first = num2;
        sec = num1;
    }

    switch (op) {
        case 1<<0: 
            cout << "into " << dest << " put" <<  " set " << (input>>R1_SHIFT & 0xff) << endl;
            registers[dest] = (input>>R1_SHIFT & 0xff);
            break;
        case 1<<1: 
            cout << "into " << dest << " put" <<  " grt " << first << " " << sec << endl;
            registers[dest] = (first>sec);
            break;
        case 1<<2: 
            cout << "into " << dest << " put" << " less" << first << " " << sec << endl;
            registers[dest] = (first<sec);
            break;
        case 1<<3: 
            cout << "into " << dest << " put" << " comp" << first << " " << sec << endl;
            registers[dest] = (first==sec);
            break;
        case 1<<4: 
            cout << "into " << dest << " put" <<  " inc " << first << endl;
            first++;
            registers[dest] = first;
            break;
        case 1<<5: 
            cout << "into " << dest << " put" <<  " dinc " << first << endl;   
            first--;
            registers[dest] = first;
            break;
        case 1<<6:
            cout << "into " << dest << " put" << " rsh " << first << endl;
            registers[dest] = first>>1;
            break;
        case 1<<7:
            cout << "into " << dest << " put" << " lsh " << first << endl;
            registers[dest] = first<<1;
            break;
    }
}

void word_machine(uint32_t input) {
    /*
    the goal of this is to make it deal with chars and large values storted in 64 bit registers
    so an assembly command can 'put' values (8) in a register that represents chars

    0b 0010 0000 00000000 00000000 00000000
        wm   op    dest      v2       v1
        v means value 

    operations: 
    put: op 0000
    access: op 0001
    not implemented: op 0010
    clear64: op 0011
    clear: op 0100

    */
    cout << "word machine: " << endl;
    set_bitsize::byte i;
    i.byte = input>>OP_SHIFT  & 0xf;
    int op = byte_selector(i); // 0b00001 is the flip operation, 0b00000111 is the operation
    int dest = get_dest(input); //0xff = 0b11111111. is a regular number that gets used like [dest]
    int num1 = input>>R1_SHIFT & 0xff; 
    int num2 = input & 0xff; 
    
    switch(op) {
        case 1<<0: 
            registers_64[dest] = (registers_64[dest]<<16) | (input & 0xffff);
            cout << "into " << dest << " put" << ((registers_64[dest]<<16) | (input & 0xffff)) << endl;
            break;
        case 1<<1:
            cout << "\033[31m" << word_accesser(registers_64[dest]) << "\033[0m" << endl;
            cout << "into " <<  dest << " access " << endl;
            break;
        case 1<<2:
            cout << "not supported yet" << endl;
            break;
        case 1<<3:
            registers_64[dest] = 0; 
            cout << "r_64: " << dest << " has been cleared." << endl;
            break;
        case 1<<4:
            cout <<  "into " << dest << " cleared." << endl;
            registers[dest] = 0;
            break;
        default:
            cout << "failed" << endl;
            break;
    }
}

void control_flow(uint32_t input) {
    /*
    0b 0011 0000 00000000 00000000 00000000
        cf   op    dest      r1       r2

    0000 = jmp
    0001 = if
    if dest r1 (tests r1, skips to destination else doesnt)
    0010 = load, loads memory into a register
    0011 = str, stores register into memory
    0100 = save, saves the entire.bin file 
    add command for including other files
    0101 = access32, prints value in register
    
    1111 = halt, ends the program
    */
    cout << "control flow: " << endl;
    set_bitsize::byte i;
    i.byte = input>>OP_SHIFT & 0xf;
    int op = byte_selector(i); // 0b00001 is the flip operation, 0b00000111 is the operation
    int dest = get_dest(input); //0xff = 0b11111111. is a regular number that gets used like [dest]
    int num1 = registers[input>>R1_SHIFT & 0xff]; 
    int num2 = input & 0xff; 

    switch (op) {
        case 1<<0: pc = dest-1; cout << "jmp to " << dest << endl; break; //jmp
        case 1<<1: if(num1) {pc = dest-1;} cout << "if " << num1 << " is 1, go to line " << dest << endl; break; //if
        //executes something if the value in register is true
        // alu commands can set a register to hold a true (>0) or false (=0) value 
        case 1<<2 : registers[dest] = memory[input>>R1_SHIFT & 0xff]; cout << "memory address " << (input>>R1_SHIFT & 0xff) << " has been loaded into " << dest << endl; break; //load
        case 1<<3 : memory[dest] = num1; cout << "memory address " << dest << " has been loaded with " << num1 << endl;break; //store
        case 1<<4 : {
            cout << "saving..." << endl;
            string filename; 
            cout << "Name file: ";
            cin >> filename;
            if (filename == ".cancel") {
                cout << "canceled" << endl;
                break;
            }
            system("mkdir -p bin");
            filename = "./bin/" + filename + ".bin";
            cout << "File named: " << filename << endl;
            ofstream MyFile(filename);
            if (MyFile.is_open()) {
                int i = 0;
                while((instruct_memory[i+1])) {
                    MyFile << bitset<32>(instruct_memory[i]).to_string() << endl;
                    i++;
                }
            } else {
                cout << "file failed to open" << endl;
            }
            MyFile.close();
            break;
        }
        case 1<<5 : cout << "\033[31mvalue is: " << registers[dest] << "\033[0m" << endl; break;
        case 1<<7 : cout << "Program ended" << endl; exit(0); break;
    }
}

void graphics_control(uint32_t input) {
    /*add stuff
    0b 0 000 0000 00000000 00000000 00000000
      gc  ?   ?       ?        ?        ?

    void set_color(int x, int y, Color c);
    Color get_color(int x, int y);
    void build_screen();
    void draw_rect(int x1, int y1, int x2, int y2, Color c);
    void draw(int x1, int y1, int x2, int y2, Color c);
    void draw_line_horizontal(int x1, int x2, int y, Color c);
    void draw_line_vertical(int y1, int y2, int x, Color c);
    void clear_screen(Color c);
    void move_obj_x(Object &obj, int dx);
    void move_obj_y(Object &obj, int dy);
    void update_screen();
    void print_screen();
    void save_screen();
    void open_image();
    */ 
}

void cpu(uint32_t input) {
    set_bitsize::byte supOp; 
    supOp.byte = (input>>SUP_SHIFT & 0xF);
    uint32_t in32 = input;
    int r_supOp = byte_selector(supOp);
    switch (r_supOp) {
        case 1<<0 : alu(in32); break;
        case 1<<1 : alu2(in32); break;
        case 1<<2 : word_machine(in32); break;
        case 1<<3 : control_flow(in32); break;
    }   
}

void run_computer() {
    string file_to_run;
    cout << "Run file: "; // chose a .bin file to run
    cin >> file_to_run;
    if (file_to_run == ".cancel") {
        cout << "canceled" << endl;
        return;
    }
    system("mkdir -p bin");
    ifstream input("./bin/" + file_to_run + ".bin");
    if (!input) {
        cout << "Failed to open file" << endl;
        return;
    }
    string line;  
    while(getline(input, line)){ // this puts all the instructions into memory
        uint32_t instruction = stol(line, nullptr, 2);
        instruct_memory[pc++] = instruction; 
    }
    int program_len = pc;
    pc = 0;

    while(pc < program_len) {
        cout << "PC: " << pc << endl;
        uint32_t instruction = instruct_memory[pc];
        cpu(instruction);  // instruction may modify pc
        pc++;             
        cout << endl;
    }
}

int main() {
    run_computer();
    return 0;
}