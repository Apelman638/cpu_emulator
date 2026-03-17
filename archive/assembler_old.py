import os
os.makedirs("bin", exist_ok=True)

"""
I am currently remaking this into a more optimized assembler and a more human friendly assembly language.
"""

print("File exists:", os.path.exists("test.asm"))

print("start")
with open("test.asm", "r") as f:
    assembly = f.readlines()
print("Contents read from file:", repr(assembly))

"""
for alu1:
0000
addition 000
subtraction 001
or 010
and 011
not 100
xor 101
pass 110

for alu2:
0001
    set = 000
    grt > 001
    less < 010
    comp == 011
    inc +1 100
    dinc -1 101
    rsh <<1 110
    lsh >>1 111
**no more flip**

for word machine:
0010
    put = 0000
    access = 0001
    0010
    clear64 0011
    clear 0100

for control flow:   
0011
    jmp = 0000
    if = 0001
    load = 0010
    str = 0011
    save = 0100
    access32 = 0101

graphic control:
    0b1 0000 c x1 y1 x2 y2
    case 1<<0 : build_screen(); break; 0000 
    case 1<<1 : draw(x1,y1,x2,y2,cc); break; 0001
    case 1<<2 : draw_rect(x1,y1,x2,y2,cc); break; 0010
    case 1<<3 : draw_line_horizontal(x1, x2, y1,cc); break; 0011
    case 1<<4 : draw_line_vertical(y1,y2,x1,cc); break; 0100
    case 1<<5 : move_obj_x(all_objects[y1], x1); break; 0101
    case 1<<6 : move_obj_y(all_objects[x1], y1); break; 0110
    case 1<<7 : update_screen(); break; 0111
    case 1<<8 : print_screen(); break; 1000
    case 1<<9 : save_screen(); break; 1001
    case 1<<10 : open_image(); break; 1010

add labels

0b 0000 0000 00000000 00000000 00000000
  supop  op    dest      r1       r2

maybe a get command that gets a value from a register and puts it into a variable
"""

no_r2 = ['dinc','inc','set','pass','not','lsh','rsh','if','load','str']
alu_ops = ["add","sub","or","and","not","xor","pass","set","grt","less","comp","inc","dinc","rsh","lsh"]
all_ops = ["add","sub","or","and","not","xor","pass","set","grt","less","comp","inc","dinc","rsh","lsh","put","access","varset","clear","clear64","jmp","if","load","str"]
only_dest = ["access","clear","clear64","jmp","access32"]

gpu_ops = ["bldsc", "draw", "rect", "lh", "lv", "movx", "movy", "upd", "print", "savsc", "opnimg"]
gpu_void = ["bldsc", "upd", "print", "savsc", "opnimg"]

#perhaps optimize this and turn it into a function.
def binary(contents):
    addFunc = False
    funcs = {}
    funcBi = ''
    variables = {}
    binary = ""
    for lines in contents:
        if ('#' in lines):
            lines = lines.split("#")[0].strip()
            if not lines:
                continue
        
        if 'include' in lines: # this is only read by the assembler
            tTokens = lines.strip().split()
            fileToInclude = tTokens[1]
            with open(fileToInclude+".bin", "r") as f:
                binary += f.read()
                break

        if 'halt' in lines:
            binary += "00111111000000000000000000000000\n"
            break
        line = ""
        if 'save' in lines:
            binary += "00110100000000000000000000000000"
            continue

        if len(lines.strip().split()) <= 0:
            binary += ""
            continue

        if '=' in lines:
            tTokens = lines.strip().split()
            variables[tTokens[0]] = tTokens[2]
            continue 

        if ('func' in lines):
            tTokens = lines.strip().split()
            funcs[tTokens[1]] = ""
            addFunc = True
            continue

        if ('endf' in lines):
            tTokens = lines.strip().split()
            addFunc = False
            funcs[tTokens[1]] = funcBi
            funcBi = ""
            continue

        cont = False
        # certain ops dont need a value in r2
        if any(o in lines for o in no_r2):
            r2 = '00000000'
            cont = True
        
        tokens = lines.strip().split()

        for token in tokens:
            if token in variables.keys():
                tokens[tokens.index(token)] = variables[token]
            continue    
        
        # assembly to gpu, maybe make it its own function
        # the way i made this is a better model for the whole function
        if any(o in lines for o in gpu_ops):
            line += "1"
            if tokens[0] == "rep:": # this repeats functions, so the function is 3rd after the rep num
                repNum = int(tokens[1])
                op = tokens[2]
                opI = 2
                if tokens[2] in funcs.keys(): # checks if its an already saved function
                    for i in range(repNum): 
                        binary+=funcs[tokens[2]]
                continue 
            else:
                repNum = 1
                op = tokens[0]
                opI = 0
            if op in gpu_void:
                match op:
                    case "bldsc" :
                        line += "0000"
                        c = f"{0:03b}"
                        x1 = f"{0:06b}"
                        y1 = f"{0:06b}"
                        x2 = f"{0:06b}"
                        y2 = f"{0:06b}"                 
                    case "upd":
                        line += "0111" 
                        c = f"{0:03b}"
                        x1 = f"{0:06b}"
                        y1 = f"{0:06b}"
                        x2 = f"{0:06b}"
                        y2 = f"{0:06b}"         
                    case "print":
                        line += "1000" 
                        c = f"{0:03b}"
                        x1 = f"{0:06b}"
                        y1 = f"{0:06b}"
                        x2 = f"{0:06b}"
                        y2 = f"{0:06b}"  
                    case "savsc":
                        line += "1001"
                        c = f"{0:03b}"
                        x1 = f"{0:06b}"
                        y1 = f"{0:06b}"
                        x2 = f"{0:06b}"
                        y2 = f"{0:06b}"  
                    case "opnimg":
                        line += "1010"
                        c = f"{0:03b}"
                        x1 = f"{0:06b}"
                        y1 = f"{0:06b}"
                        x2 = f"{0:06b}"
                        y2 = f"{0:06b}"  
            #  0b1 0000 c x1 y1 x2 y2
            match op:
                case "draw":
                    line += "0001"
                    c = f"{int(tokens[opI+1]):03b}"
                    x1 = f"{int(tokens[opI+2]):06b}"
                    y1 = f"{int(tokens[opI+3]):06b}"
                    x2 = f"{int(tokens[opI+4]):06b}"
                    y2 = f"{int(tokens[opI+5]):06b}"
                case "rect":
                    line += "0010"
                    c = f"{int(tokens[opI+1]):03b}"
                    x1 = f"{int(tokens[opI+2]):06b}"
                    y1 = f"{int(tokens[opI+3]):06b}"
                    x2 = f"{int(tokens[opI+4]):06b}"
                    y2 = f"{int(tokens[opI+5]):06b}"
                case "lh":
                    line += "0011"
                    c = f"{int(tokens[opI+1]):03b}"
                    x1 = f"{int(tokens[opI+2]):06b}"
                    y1 = f"{int(tokens[opI+3]):06b}" # actually x2 but for consistancy
                    x2 = f"{int(tokens[opI+2]):06b}" # actually y
                    y2 = f"{0:06b}" # nothing
                case "lv":
                    line += "0100"
                    c = f"{int(tokens[opI+1]):03b}"
                    x1 = f"{int(tokens[opI+2]):06b}" # y1
                    y1 = f"{int(tokens[opI+3]):06b}" # y2
                    x2 = f"{int(tokens[opI+4]):06b}" # x
                    y2 = f"{int(tokens[opI+5]):06b}" # nothing
                case "movx":
                    line += "0101"
                    c = f"{0:03b}" # nothing
                    x1 = f"{int(tokens[opI+1]):06b}" # obj 
                    y1 = f"{int(tokens[opI+2]):06b}" # dx
                    x2 = f"{0:06b}" #  nothing
                    y2 = f"{0:06b}" # nothing                   
                case "movy":
                    line += "0110"
                    c = f"{0:03b}" # nothing
                    x1 = f"{int(tokens[opI+1]):06b}" # obj 
                    y1 = f"{int(tokens[opI+2]):06b}" # dy
                    x2 = f"{0:06b}" #  nothing
                    y2 = f"{0:06b}" # nothing  

            line += c + x1 + y1 + x2 + y2 + "\n"             
            for i in range(repNum): 
                if addFunc:
                    funcBi+=line
                    break
                binary+=line 
            continue

        if len(tokens) <= 0:
            binary += ""

        if (tokens[0] == 'flip'):
            repNum = 1
            flip = '1'
            op = tokens[1]
            dest = f"{int(tokens[2]):08b}"
            r1 = f"{int(tokens[3]):08b}"
            if (cont == False): 
                r2 = f"{int(tokens[4]):08b}"
        elif tokens[0] == 'rep:':
            repNum = int(tokens[1])
            flip = '0'
            if tokens[2] in funcs.keys():
                for i in range(repNum): 
                    binary+=funcs[tokens[2]]
                continue 
            op = tokens[2]
            dest = f"{int(tokens[3]):08b}"
            r1 = f"{int(tokens[4]):08b}"
            if (cont == False):
                r2 = f"{int(tokens[5]):08b}"
        elif tokens[0] == "varset":
            variables[tokens[1]] = 0 #get value from register might not be possible with tokenizer
            # not supported yet
        elif tokens[0] in funcs.keys():
            for i in range(repNum): 
                binary+=funcs[tokens[0]]
        elif any(o in lines for o in only_dest):
            repNum = 1
            flip = '0'
            op = tokens[0]
            dest = f"{int(tokens[1]):08b}"
            r1 = f"{0:08b}"
            r2 = f"{0:08b}"
        elif any(p in lines for p in gpu_ops):
            repNum = 1
            flip = ''
            op = ''
            dest = ''
            r1 = ''
            r2 = ''
        else:
            repNum = 1
            flip = '0'
            op = tokens[0]
            dest = f"{int(tokens[1]):08b}"
            r1 = f"{int(tokens[2]):08b}"
            if (cont == False):
                r2 = f"{int(tokens[3]):08b}"

        match op:
            case "add":
                line += "0000" + flip + "000"
            case "sub":
                line += "0000" + flip + "001"
            case "or":
                line += "0000" + flip + "010"
            case "and":
                line += "0000" + flip + "011"
            case "not":
                line += "0000" + flip + "100"
            case "xor":
                line += "0000" + flip + "101"
            case "pass":
                line += "0000" + flip + "110"
            case "set":
                line += "0001" + flip + "000"
            case "grt":
                line += "0001" + flip + "001"
            case "less":
                line += "0001" + flip + "010"
            case "comp":
                line += "0001" + flip + "011"
            case "inc":
                line += "0001" + flip + "100"
            case "dinc":
                line += "0001" + flip + "101"
            case "rsh":
                line += "0001" + flip + "110"
            case "lsh":
                line += "0001" + flip + "111"
            case "put":
                line += "00100000"
            case "access":
                line += "00100001"
            case "varset":
                line += "00100010"
                # not supported yet
            case "clear64":
                line += "00100011"
            case "clear":
                line += "00100100"
            case "jmp":
                line += "00110000"
            case "if":
                line += "00110001"
            case "load":
                line += "00110010"
            case "str":
                line += "00110011"
            case "access32":
                line += "00110101"

        line += dest + r1 + r2 +  "\n"
        for i in range(repNum): 
            if addFunc:
                funcBi+=line
                break
            binary+=line

    return binary

code = binary(assembly)
print(code)

with open('bin/binary.bin', 'w') as b:
    b.write(code)

print("done")