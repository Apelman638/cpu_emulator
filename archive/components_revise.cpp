#include <iostream>
using namespace std;

struct byte {
    int byte : 4;
};

int16_t byte_selector(byte input){ // decoder
    return 1<<input.byte; // if selection is 6, it outputs the 7th bit

}

int bit_adder(unsigned int a, unsigned int b) {
    int sum1 = a ^ b;
    int carry = (a & b) << 1;
    if(carry != 0) {
        return bit_adder(sum1, carry);
    }
    return sum1;
}

int alu(byte input, int num1, int num2) {
    /*
    the input gets decoded. in the original alu it waas all in 1 input, and there were 14 and 14 bits reserved for num1 and num2
    */

    int first, sec;
    first = num1;
    sec = num2;
    if ((input.byte>>3) & 1) {
        first = num2;
        sec = num1;
    }

    int op = byte_selector((input.byte & 0b0111));

    //cout << (input>>31) << " " << op << " " << num1 << " " << num2 << endl;

    //make the first last bit in the input switch num1 and num2

    switch (op) {
        case 1<<0 :
            //cout << (input>>31) << " add " << num1 << " " << num2 << endl;
            return bit_adder(first, sec);//addition;
        case 1<<1 :
            //cout << (input>>31) << " sub " << num1 << " " << num2 << endl;
            return bit_adder(first, -sec);//subtraction;
        case 1<<2 :
            //cout << (input>>31) << " or " << num1 << " " << num2 << endl;
            return first | sec; //or;
        case 1<<3 :
            //cout << (input>>31) << " and " << num1 << " " << num2 << endl;
            return first & sec;//and;
        case 1<<4 :
            //cout << (input>>31) << " not " << num1 << " " << num2 << endl;
            return ~num1; //not;
        case 1<<5 :
            //cout << (input>>31) << " xor " << num1 << " " << num2 << endl;
            return first ^ sec; //xor;
        case 1<<6 :
            //cout << (input>>31) << " pass " << num1 << " " << num2 << endl;
            return first; //pass;
        default:
            cout << "failed" << endl;
    }
    return 0;
}


int alu(uint32_t input) {
    /*
    can do:

    addition
    subtraction
    or
    and
    not
    xor
    nand
    pass

    usually only need 5 functions but ill do 8 to be clean
    16 is ideal so i can do 4 control bits and 6 for each number

    do 3 selector bits
    29 for passing in numbers
    0b 000 000000000000 00000000000000000
    selection num1 num2

    i could also use the 4 bit selector i made

    ill do 32 and ignore extra bits i guess
    0b 0000 00000000000000 00000000000000
    */
    set_bitsize::byte i;
    i.byte = input>>28  & 0b0111; // first 4 bits
    int op = byte_selector(i); // 0b1 is the flip operation, 0b0111 is the operation


    int num1 = bit_selector(0b11111111111111, input>>14);
    int num2 = bit_selector(0b11111111111111, input); //11...111 & ...num2

    int first, sec;
    first = num1;
    sec = num2;
    if ((input>>31) & 1) {
        first = num2;
        sec = num1;
    }


    //cout << (input>>31) << " " << op << " " << num1 << " " << num2 << endl;

    //make the first last bit in the input switch num1 and num2

    switch (op) {
        case 1<<0 :
            //cout << (input>>31) << " add " << num1 << " " << num2 << endl;
            return bit_adder(first, sec);//addition;
        case 1<<1 :
            //cout << (input>>31) << " sub " << num1 << " " << num2 << endl;
            return bit_adder(first, -sec);//subtraction;
        case 1<<2 :
            //cout << (input>>31) << " or " << num1 << " " << num2 << endl;
            return first | sec; //or;
        case 1<<3 :
            //cout << (input>>31) << " and " << num1 << " " << num2 << endl;
            return first & sec;//and;
        case 1<<4 :
            //cout << (input>>31) << " not " << num1 << " " << num2 << endl;
            return ~num1; //not;
        case 1<<5 :
            //cout << (input>>31) << " xor " << num1 << " " << num2 << endl;
            return first ^ sec; //xor;
        case 1<<6 :
            //cout << (input>>31) << " pass " << num1 << " " << num2 << endl;
            return first; //pass;
        default:
            cout << "failed" << endl;
    }
    return 0;
}



