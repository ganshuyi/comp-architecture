#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
  
#define BIT_LEN 		1
#define BYTE_LEN 		BIT_LEN * 8
#define WORD_LEN 		BIT_LEN * 16
#define INSTRUCTION_LEN	BIT_LEN * 32
#define LINE_LEN 		1000
#define ARGC_LEN 		3
#define WHITESPACE		"|"


//assembly code structure
struct assembly_type {
	char label[7];
	char instruction[7];
	char field0[7];
	char field1[7];
	char field2[7];
	char comments[50];
	int address;
};



//global variables
int assembly_cnt = 0; //keep track of the number of assembly code lines
struct assembly_type assembly_line_arr[50]; //max 50 lines of assembly codes
int mc_arr[50]; //machine codes array

int opcode_len = 10;
char *std_opcodes[10] = {"add","nor","lw","sw","beq","jalr","halt","noop",".fill",""};

char *r_format = "add nor";
char *i_format = "lw sw beq";
char *j_format = "jalr";
char *o_format = "halt noop";


void terminate(char * err_msg) {
	printf ("Error: %s\n",err_msg);
	exit(1); //exit with error
} //terminate()


//Initialise a assembly_type record, fill fields with blank
void initArec(struct assembly_type arec) {
	strcpy(arec.label,"");
	strcpy(arec.instruction,"");
	strcpy(arec.field0,"");
	strcpy(arec.field1,"");
	strcpy(arec.field2,"");
	strcpy(arec.comments,"");
	arec.address = 0;
} //initArec()s


int is_int(char *input) {
    int num = ((sscanf(input, "%d", &num)) == 1);
    return num;
} //is_int()


int is_alpha(char *input) {
    if (*input >= 'a' && *input <= 'z') {return 1;} //lower case
    if (*input >= 'A' && *input <= 'Z') {return 1;} //upper case
    return 0;
} //is_alpha()


int chk_label(char *label_name) {
	int i, first_character = 0;
	if (strlen(label_name) == 0) {return 1;} //no label
	if (strlen(label_name) > 6) {return 0;} //maximum 6 characters
	
	if (is_int(&label_name[first_character])) {return 0;} //label starts with number
	
	for (i = 0; i < strlen(label_name); i++) {
		if (is_alpha(&label_name[i])) {continue;}
        if (is_int(&label_name[i])) {continue;}

        return 0; //not alphanumeric (alphabet/numeric)
    } //for
    
    return 1; //Valid
} //chk_label


int chk_label_dup(char *label_name) {
	int i;
	if (strlen(label_name) == 0) {return 0;} //no label
	
	for (i=0; i<assembly_cnt; i++) {
		if (!strcmp(assembly_line_arr[i].label,label_name)) {return 1;} //duplicated
	} //for
	
	return 0; //no duplicate
} //chk_label_dup()


int chk_opcode(char *opcode) {
	int i;
	
	for(i=0; i<opcode_len; i++) {
		if (!strcmp(std_opcodes[i],opcode)) {return 1;}
	} //for
	
	return 0;
} //chk_optcode()


int chk_fill_boundry(char *value) {
	long this_val, upper_limit, lower_limit;
	upper_limit = (2^31)-1; //32767
	lower_limit = -2^31; //-32768
	this_val = atol(value);
	
	if ((this_val > lower_limit) && (this_val < upper_limit)) {return 1;}
	
	return 0; //out of boundary
} //chk_fill_boundry()


int chk_addr_boundry(int value) {
	long upper_limit, lower_limit;
	upper_limit = (2^31)-1; //32767
	lower_limit = -2^31; //-32768
	
	if ((value > lower_limit) && (value < upper_limit)) {return 1;}
	
	return 0; //out of boundary
} //chk_addr_boundry()


void read_assembly_source(char * fname) {
	FILE *fptr; 
	char * line = NULL;
	char *tmp;
	size_t len = 0;
	ssize_t read;
	int i = 0;
	int j, k, ok, dup;
	char *tmp_line;
	char *token;
	char emsg[50];

	
	struct assembly_type arec;
    
	fptr = fopen (fname, "r"); //open in read mode 
	if (fptr == NULL) {
		printf("Error opening input file: %s\n", fname); 
		exit(1); 
	}
	
	while ((read = getline(&line, &len, fptr)) != -1) {
		i++;
		//printf("Retrieved line: %d, of length: %zu:\n", i, read);
		//printf("%s", line);
        
		if (strchr(line, '\n') == NULL) {terminate("Line without carriage return!");}
		
		if (read < 2) {continue;} //skip if line length is less than 2
		
		j = 0;
		tmp_line = strdup(line);
		
		//strip carriage return at the end of line
		for(k = 0; k < strlen(tmp_line); k++) {
			if(tmp_line[k] == '\n')  {tmp_line[k] = '-';}
		}
		
		initArec(arec); //initialise the record
		
		
		while ((token = strsep(&tmp_line, WHITESPACE)) != NULL) {
			if (*token == '\0') {token = "";} //empty
			//printf("%d) %s\n", j, token);
			
			if (j == 0) {strcpy(arec.label,token);} //strcopy(destination, src)
			if (j == 1) {strcpy(arec.instruction,token);} //strcopy(destination, src)
			if (j == 2) {strcpy(arec.field0,token);} //strcopy(destination, src)
			if (j == 3) {strcpy(arec.field1,token);} //strcopy(destination, src)
			if (j == 4) {strcpy(arec.field2,token);} //strcopy(destination, src)
			if (j == 5) {strcpy(arec.comments,token);} //strcopy(destination, src)
			j++;
		} //while
		
		//check if lable is valid
		ok = chk_label(arec.label);
		if (!ok) {
			strcpy(emsg,"Invalid label: ");
			strcat(emsg,arec.label);
			terminate(emsg);
		}
		
		//check if lable is duplicated
		dup = chk_label_dup(arec.label);
		if (dup) {
			strcpy(emsg,"Duplicated label: ");
			strcat(emsg,arec.label);
			terminate(emsg);
		}
		
		//check if opcode is valid
		ok = chk_opcode(arec.instruction);
		if (!ok) {
			strcpy(emsg,"Invalid opcode: ");
			strcat(emsg,arec.instruction);
			terminate(emsg);
		}
		
		//check fill overflow
		if (!strcmp(arec.instruction, ".fill") && (is_int(arec.field0))) { //check boundry
			ok = chk_fill_boundry(arec.field0);
			if (!ok) {
				strcpy(emsg,".fill value overflow: ");
				strcat(emsg,arec.field0);
				terminate(emsg);
			}
		}
		
		
		//printf ("Label: %s, ", arec.label);
		//printf ("Instruction: %s, ", arec.instruction);
		//printf ("Field0: %s, ", arec.field0);
		//printf ("Field1: %s, ", arec.field1);
		//printf ("Field2: %s, ", arec.field2);
		//printf ("Comments: %s\n", arec.comments);
		
		arec.address = assembly_cnt;
		
		assembly_line_arr[assembly_cnt] = arec;
		assembly_cnt++;
		
    } //while

	fclose(fptr);
	
} //read_assembly_source()


void prn_assembly_arr(char * fname) {
	int i;
	printf("%d Assemply Records from this file: %s\n", assembly_cnt, fname);
	for (i=0; i<assembly_cnt; i++) {
		printf ("%d)",i+1);
		printf ("Label: %s, ", assembly_line_arr[i].label);
		printf ("Instruction: %s, ", assembly_line_arr[i].instruction);
		printf ("Field0: %s, ", assembly_line_arr[i].field0);
		printf ("Field1: %s, ", assembly_line_arr[i].field1);
		printf ("Field2: %s, ", assembly_line_arr[i].field2);
		printf ("Comments: %s\n", assembly_line_arr[i].comments);
	} //for
} //prn_assembly_arr()


void save_mcode_to_file(char *fname) {
	FILE *fptr; 
	int i;
	
	if (!assembly_cnt) {return;} //nothing to save
	
	printf("Save Machine %d Codes to file: %s\n", assembly_cnt, fname);
	
	//open file for writing
	fptr = fopen(fname, "w");
	
	for (i=0; i<assembly_cnt; i++) {
		if (!mc_arr[i]) {continue;} //skip if empty
		fprintf(fptr, "%d\n",mc_arr[i]);
	} //for
	
    fclose(fptr);
} //save_mcode_to_file()


int valid_reg(char *reg, int rtype) {
	int rvalue = atoi(reg); //convert to integer

    if (!is_int(reg)) {return 0;} //reg is not integer
    if ((rvalue < 0) || (rvalue >= 8)) {return 0;} //reg is not between 0-7
    if ((rtype == 1) && (rvalue == 0)) {return 0;} //destination register

    return 1;
} //valid_reg()


int get_label_addr(char *this_label) {
	int addr = 0;
	int i;
	
	for (i=0; i<50; i++) {
		if (!strcmp(assembly_line_arr[i].label,this_label)) {addr = assembly_line_arr[i].address; break;}
	}

	return addr;
} //get_label_addr()




int convert_to_rformat(char *opcode, char *reg0, char *reg1, char *dest) {
	char emsg[50];
	int mcode = 0;

	//Check Register
	if (!valid_reg(reg0,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg0);
		terminate(emsg);
	} 
	else if (!valid_reg(reg1,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg1);
		terminate(emsg);
	}
	else if (!valid_reg(dest,1)) {
		strcpy(emsg,"Invalid destination register: ");
		strcat(emsg,dest);
		terminate(emsg);
	}
	
	//Format Opcode/Instruction
	if (!strcmp(opcode, "add")){mcode = (0 << 22);}
	else if (!strcmp(opcode, "nor")){mcode = (1 << 22);}
	else {return 0;}
	
	//Format address
	mcode |= (atoi(reg0) << 19);
	mcode |= (atoi(reg1) << 16);
	mcode |= (atoi(dest) << 0);
	
	return mcode;
} //convert_to_rformat()


int convert_to_iformat(char *opcode, char *reg0, char *reg1, char *offset, int pc) {
	char emsg[50];
	int mcode = 0;
	int addr = 0;
	long long upper_limit, lower_limit;
	upper_limit = (2^31)-1; //32767
	lower_limit = -2^31; //-32768

    
	//Check Register
	if (!valid_reg(reg0,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg0);
		terminate(emsg);
	} 
	else if (!valid_reg(reg1,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg1);
		terminate(emsg);
	}

	//Format opcode
	if (!strcmp(opcode, "lw")) {mcode |= (2 << 22);}
	else if (!strcmp(opcode, "sw")) {mcode |= (3 << 22);}
	else if (!strcmp(opcode, "beq")) {mcode |= (4 << 22);}
	else {
		strcpy(emsg,"Invalid Opcode: ");
		strcat(emsg,opcode);
		terminate(emsg);
    }

	// Format address
	mcode |= (atoi(reg0) << 19);
	mcode |= (atoi(reg1) << 16);

	if (is_int(offset)) {
		addr = atoi(offset);
		if (!chk_addr_boundry(addr)) { //not between -32768 to 32767
			strcpy(emsg,"Offset address is out of boundary: ");
			strcat(emsg,offset);
			terminate(emsg);
		}
	}
	else {addr = get_label_addr(offset);}

	if ((!strcmp(opcode, "beq")) && !is_int(offset)) {
		int tmp = addr;
		addr = tmp - pc - 1;
        if (!chk_addr_boundry(addr)) { //not between -32768 to 32767
			strcpy(emsg,"Offset address is out of boundary: ");
			strcat(emsg,offset);
			terminate(emsg);
		}
    }

	addr &= 0xFFFF;
	mcode |= addr;
	return mcode;
} //convert_to_iformat


int convert_to_jformat(char *reg0, char *reg1){
	char emsg[50];
	int mcode = 0;
	
	//Check Register
	if (!valid_reg(reg0,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg0);
		terminate(emsg);
	} 
	else if (!valid_reg(reg1,0)) {
		strcpy(emsg,"Invalid register: ");
		strcat(emsg,reg1);
		terminate(emsg);
	}
	
	//Format opcode
	mcode = (5 << 22);
	
	//Format register
	mcode |= (atoi(reg0) << 19);
	mcode |= (atoi(reg1) << 16);
	
	return mcode;
} //convert_to_jformat


int convert_to_oformat(char *opcode) {
    int mcode = 0;

    //Format opcode
    if (!strcmp(opcode, "halt")) {mcode = (6 << 22);}
    else if (!strcmp(opcode, "noop")) {mcode = (7 << 22);}
    else {return 0;}

    return mcode;
} //convert_to_oformat


void convert_to_mcode() {
	char emsg[50];
	int i;
	
	printf("Convert %d Assemby to Machine Codes\n", assembly_cnt);
	
	for (i=0; i<assembly_cnt; i++) {
		if (strstr(r_format, assembly_line_arr[i].instruction) != NULL) {
			mc_arr[i] = convert_to_rformat(assembly_line_arr[i].instruction, assembly_line_arr[i].field0, assembly_line_arr[i].field1, assembly_line_arr[i].field2);
			printf("Assembly: %s to Machine: %d\n",assembly_line_arr[i].instruction,mc_arr[i]);
		}
		else if (strstr(i_format, assembly_line_arr[i].instruction) != NULL) {
			mc_arr[i] = convert_to_iformat(assembly_line_arr[i].instruction, assembly_line_arr[i].field0, assembly_line_arr[i].field1, assembly_line_arr[i].field2, i);
			printf("Assembly: %s to Machine: %d\n",assembly_line_arr[i].instruction,mc_arr[i]);
		}
		else if (strstr(j_format, assembly_line_arr[i].instruction) != NULL) {
			mc_arr[i] = convert_to_jformat(assembly_line_arr[i].field0, assembly_line_arr[i].field1);
			printf("Assembly: %s to Machine: %d\n",assembly_line_arr[i].instruction,mc_arr[i]);
		}
		else if (strstr(o_format, assembly_line_arr[i].instruction) != NULL) {
			mc_arr[i] = convert_to_oformat(assembly_line_arr[i].instruction);
			printf("Assembly: %s to Machine: %d\n",assembly_line_arr[i].instruction,mc_arr[i]);
		}
		else if (strstr(".fill", assembly_line_arr[i].instruction) != NULL) {
			if (is_int(assembly_line_arr[i].field0)) {mc_arr[i] = atoi(assembly_line_arr[i].field0);}
			else {mc_arr[i] = get_label_addr(assembly_line_arr[i].field0);}
		}
		else {
			strcpy(emsg,"Invalid opcode: ");
			strcat(emsg,assembly_line_arr[i].instruction);
			terminate(emsg);
		}
		
	} //for
	
} //convert_to_mcode()


  
int main(int argc, char *argv[]) {
	 
	//incomplete argument
	if (argc != ARGC_LEN) {
		printf("Incomplete arguments for this command\n");
		printf("Usage: assemble Input-filename Output-filename\n");
		exit(1);
	} //if argc
	
	//parse 1: read labels and build address 
	read_assembly_source(argv[1]); 
	
	//parse 2: assembly to machine code conversion
	convert_to_mcode();
	
	//save to output file
	save_mcode_to_file(argv[2]);
	 
	printf("Conversion completed\n");
	exit(0);
} //main()
