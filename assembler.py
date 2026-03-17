import os
os.makedirs("bin", exist_ok=True)

# add comment support

print("File exists:", os.path.exists("test.asm"))

print("start")
with open("test.asm", "r") as f:
    assembly = f.read()
print("Contents read from file:\n", assembly)

var_list = {}
funcs_list = {}
assembly = assembly.split("\n")

print(assembly)
cleaned = []
for line in assembly:
    if ('#' in line):
        line = line.split('#')[0]

    line = line.strip()
    if line != '':
        cleaned.append(line)

assembly = cleaned

# works: 
# ['A = 10;', 'B = 11;', 'C = 29;', 'set 0 5;', 'set 1 5;', 'func test {', 'add 1 0 1;', 'dinc 1 1;', '};', 'rep: A test;', 'access32 1;']
print(assembly)

# pass in each line
def find_vars(line): # works
    tokens = line.strip().split(' ')
    if len(tokens) > 1:
        if tokens[1] == '=':
            var_list[tokens[0]] = tokens[2].replace(";",'') # assign to variable
    # this checks all the lines and sees if a variable assignment is here
    # if so, it gets assigned a key value pair.

adding_func = False
pt1 = [] # part 1
def find_funcs(line):
    global adding_func
    global rec_name # recent name
    tokens = []
    if "func" in line:
        tokens = line.split()
        rec_name = tokens[1]
        funcs_list[rec_name] = []
        adding_func = True
    elif '};' in line:
        adding_func = False 
        rec_name = ""
    elif adding_func:
        funcs_list[rec_name].append(line.replace(';','')) 
    elif not adding_func:
        pt1.append(line) # removes any lines apart of the function from the code
        # seems to stop after function declaration ends

final_assembly = []
def ready_assembly(line): # this replaces all the variable and function names with the proper line
    final_line = [] # at the end the whole line gets added back together
    tokens = line.split() # turns each part of the line into a token
    if "=" in tokens or "func" in tokens or "};" in tokens:
        return
    rep_num = 1
    if "rep:" in tokens:
        if tokens[1] in var_list:
            rep_num = int(var_list[tokens[1]]) 
        else:
            rep_num = int(tokens[1])
            print(rep_num)
    for token in tokens:
        token = token.replace(';','')
        if token in var_list: # if the token is a variable
            final_line.append(var_list[token]) # replaces it with the associated value
            continue  # not to append twice at the end
        elif token in funcs_list: # if token is a function name 
            # needs support for if rep is infront
            print("function found: ", token)
            fline_list = []
            for fline in funcs_list[token]: # since functions are multiple lines
                fline_list.append(fline)
            final_assembly.extend(fline_list * rep_num) 
            return

        for i in range(rep_num):
            final_line.append(token)

    final_assembly.append(" ".join(final_line))

for line in assembly:
    find_vars(line)
for line in assembly:
    find_funcs(line)

print(funcs_list)
for line in pt1:
    ready_assembly(line)

print(final_assembly)

no_r2 = {
    'dinc':"00010101",'inc':"00010100",'set':"00010000",'pass':"00000110",'not':"00000100",
    'lsh':"00010111",'rsh':"00010110",'if':"00110001",'load':"00110010",'str':"00110011"
    } # turn to dict
other_ops = {"add":"00000000","sub":"00000001","or":"00000010","and":"00000011","not":"00000100","xor":"00000101",
             "grt":"00010001","less":"00010010","comp":"00010011","put":"00100000"
             }
only_dest = {
    "access":"00100001", "clear":"00100100", "clear64":"00100011", "jmp":"00110000", "access32":"00110101","pass":"00000110"
    }
void_ops = {
    "bldsc":f"1{0:031b}", "upd":f"10111{0:027b}", "print":f"11000{0:027b}", "savsc":f"11001{0:027b}", 
    "opnimg":f"11010{0:027b}", "halt":"00111111000000000000000000000000", "save":"00110100000000000000000000000000"
    }

gpu_ops_else = {
    "draw":"10001", "rect":"10010", "lh":"10011", "lv":"10100", "movx":"10101", "movy":"10110",
    "setc":"11011",
    # draw has 5 params 3 for c 6 for x1 y1 x2 y2
    # rect has 5 params 3 for c 6 for x1 y1 x2 y2
    # lh has 4 params, 3 for c x1 x2 y
    # lv has 4 params, 3 for c y1 y2 x
    # movx 2 params, obj dx 6 each
    # movy 2 params, obj dy 6 each 
    # op + c + ...
    # setc c x1 y1
    }

def to_bin(comm) -> str: # this seems to work 
    line = ''
    if len(comm) < 1:
        return ''
    comms = comm.split()
    print(f"tokens\'{comms}\'")
    op = comms[0]
    opN = 0
    if comms[0] in void_ops:
        line += void_ops[comms[0]]
        return line + "\n"
    if op in only_dest:
        line += only_dest[op] + f"{int(comms[opN+1]):08b}" + f"{0:016b}"
    if op in no_r2:
        line += no_r2[op] + f"{int(comms[opN+1]):08b}" + f"{int(comms[opN+2]):08b}" + f"{0:08b}"
    if op in other_ops:
        line += other_ops[op] + f"{int(comms[opN+1]):08b}" + f"{int(comms[opN+2]):08b}" + f"{int(comms[opN+3]):08b}"
    if op in gpu_ops_else:
        match op:
            case "draw":
                line += gpu_ops_else["draw"] + f"{int(comms[opN+1]):03b}" + f"{int(comms[opN+2]):06b}" + f"{int(comms[opN+3]):06b}" + f"{int(comms[opN+4]):06b}" + f"{int(comms[opN+5]):06b}"
    
            case "rect":
                line += gpu_ops_else["rect"] + f"{int(comms[opN+1]):03b}" + f"{int(comms[opN+2]):06b}" + f"{int(comms[opN+3]):06b}" + f"{int(comms[opN+4]):06b}" + f"{int(comms[opN+5]):06b}"
              
            case "lh":
                line += gpu_ops_else["lh"] + f"{int(comms[opN+1]):03b}" + f"{int(comms[opN+2]):06b}" + f"{int(comms[opN+3]):06b}" + f"{int(comms[opN+4]):06b}" + f"{0:06b}"
    
            case "lv":
                line += gpu_ops_else["lv"] + f"{int(comms[opN+1]):03b}" + f"{int(comms[opN+2]):06b}" + f"{int(comms[opN+3]):06b}" + f"{int(comms[opN+4]):06b}" + f"{0:06b}"
    
            case "movx":
                line += gpu_ops_else["movx"] + f"{0:03b}" + f"{int(comms[opN+1]):06b}" + f"{int(comms[opN+2]):06b}" + f"{0:012b}"
            case "movy":
                line += gpu_ops_else["movy"] + f"{0:03b}" + f"{int(comms[opN+1]):06b}" + f"{int(comms[opN+2]):06b}" + f"{0:012b}"
            case "setc":
                line += gpu_ops_else["setc"] + f"{int(comms[opN+1]):03b}" f"{int(comms[opN+2]):06b}" + f"{int(comms[opN+3]):06b}" + f"{0:012b}"
   
    line = (line + "\n")

    return line

binary = ''
for line in final_assembly:
    binary += to_bin(line)

print(binary)

with open('bin/binary.bin', 'w') as b:
    b.write(binary)

print("done")