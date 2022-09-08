#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
  
#define NUMMEMORY		65536 /* maximum number of words in memory */
#define NUMREGS			8 /* number of machine registers */
#define MAXLINELENGTH	1000

typedef struct stateStruct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int numMemory;
} stateType;


void terminate(char * err_msg) {
	printf ("Error: %s\n",err_msg);
	exit(1); //exit with error
} //terminate()


int valid_reg(int reg, int rtype) {
    if ((reg < 0) || (reg >= 8)) {return 0;} //reg is not between 0-7
    if ((rtype == 1) && (reg == 0)) {return 0;} //destination register

    return 1;
} //valid_reg()


int is_int(char *input) {
    int num = ((sscanf(input, "%d", &num)) == 1);
    return num;
} //is_int()


int chk_addr_boundry(int value) {
	long upper_limit, lower_limit;
	upper_limit = (2^31)-1; //32767
	lower_limit = -2^31; //-32768
	
	if ((value > lower_limit) && (value < upper_limit)) {return 1;}
	
	return 0; //out of boundary
} //chk_addr_boundry()


void printState(stateType *statePtr) {
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", statePtr->pc);
	printf("\tmemory:\n");
	
	for (i=0; i<statePtr->numMemory; i++) {
		printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
	printf("\tregisters:\n");
	
	for (i=0; i<NUMREGS; i++) {
		printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
	
	printf("end state\n");
} //printState()


void execute_code_rtype(stateType *statePtr, int opcode, int arg0, int arg1, int dest) {
	
	//Check Register
	if (!valid_reg(arg0,0)) {
		terminate("Invalid register: arg0");
	} 
	else if (!valid_reg(arg1,0)) {
		terminate("Invalid register: arg1");
	}
	else if (!valid_reg(dest,1)) {
		terminate("Invalid destination register");
	}
	
	if (opcode == 0) {statePtr->reg[dest] = statePtr->reg[arg0] + statePtr->reg[arg1];}
	else if (opcode == 1) {statePtr->reg[dest] = ~(statePtr->reg[arg0] | statePtr->reg[arg1]);}

} //execute_code_rtype()


void execute_code_itype(stateType *statePtr, int opcode, int arg0, int arg1, int offset) {
	if (offset & (1 << 15)) {offset -= (1 << 16);} //convert 16 to 32 bits
	
	//Check Register
	if (!valid_reg(arg0,0)) {
		terminate("Invalid register: arg0");
	} 
	else if (!valid_reg(arg1,0)) {
		terminate("Invalid register: arg1");
	}
	
	if (!chk_addr_boundry(offset)) { //not between -32768 to 32767
		terminate("Offset out of range");
	}
	
	if (opcode == 2) {statePtr->reg[arg1] = statePtr->mem[statePtr->reg[arg0] + offset];}
	else if (opcode == 3) {statePtr->mem[statePtr->reg[arg0] + offset] = statePtr->reg[arg1];}
	else if ((opcode == 4)&&(statePtr->reg[arg0] == statePtr->reg[arg1])) {statePtr->pc += offset;}
	
} //execute_code_itype


void execute_code_jtype(stateType *statePtr, int opcode, int arg0, int arg1) {
	//Check Register
	if (!valid_reg(arg0,0)) {
		terminate("Invalid register: arg0");
	} 
	else if (!valid_reg(arg1,0)) {
		terminate("Invalid register: arg1");
	}
	
	statePtr->reg[arg1] = statePtr->pc;
    statePtr->pc = statePtr->reg[arg0];
} //execute_code_jtype



int main(int argc, char *argv[]) {
	char line[MAXLINELENGTH];
	stateType state;
	int cnt = 0;
	int not_end = 1;
	FILE *filePtr;
	char emsg[50];
	
	if (argc != 2) {
		printf("error: usage: %s <machine-code file>\n", argv[0]);
		exit(1);
	}
	
	filePtr = fopen(argv[1], "r");
	if (filePtr == NULL) {
		printf("error: can't open file %s\n", argv[1]); 
		exit(1);
	}
	
	/* read in the entire machine-code file into memory */
	for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
			printf("error in reading address %d\n", state.numMemory);
			exit(1);
		} //if
		
		printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
	} //for
	
	fclose(filePtr);
	
	// Print initial state
	printf("Initial state of machine:\n");
	printState(&state);
	
	while (not_end) {
		int opcode, arg0, arg1, arg2, mval;
        
		//read machine-code from memory
		mval = state.mem[state.pc]; 
		opcode = (mval >> 22) & 0b111; //opcode: bit 25 to 22
		arg0 = (mval >> 19) & 0b111; //arg0: bit 21 to 19
		arg1 = (mval >> 16) & 0b111; //arg1: but 18 to 16
		arg2 = (mval & 0xFFFF); //arg2: bit 15 to 0
		
		//increment counters
		state.pc++;
		cnt++;
		
		printf("Loop (%d) pc: %d, Mval: %d, Opcode: %d, arg0: %d, arg1: %d, arg2: %d\n",cnt,state.pc,mval,opcode,arg0,arg1,arg2);
		if (cnt > 1000) { //max loop
			printf ("Max loop 1000, force break\n");
			break;
		} 
		//continue;

		//range check
		if (state.pc < 0 || state.pc >= NUMMEMORY) {
			terminate("PC memory out of range");
		}

         
		//execute code
		if ((opcode >= 0)&&(opcode <= 1)) {execute_code_rtype(&state, opcode, arg0, arg1, arg2);} //1:add, 2:nor
		else if ((opcode >= 2)&&(opcode <= 4)) {execute_code_itype(&state, opcode, arg0, arg1, arg2);} //2:lw, 3:sw, 4: beg
		else if (opcode == 5) {execute_code_jtype(&state, opcode, arg0, arg1);} //5:jalr
		else if (opcode == 6) {
			printf ("Execution halted\n");
			not_end = 0; break;
		} //6:halt
		else if (opcode == 7) {continue;} //7:noop
		else {
			terminate("Invalid opcode");
			not_end = 0;
		}

		printState(&state);
	} //while
	
	
	
	
	printf ("Total instructions: %d\n",cnt);
    printf("Final state of machine:\n");
    printState(&state);
	return(0);
} //main