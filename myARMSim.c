#include <stdlib.h>
#include <stdio.h>

void run_armsim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();
void swi_exit();
void fetch();
void decode();
void execute();
void mem();
void write_back();
int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);

static unsigned int R[16];
static int N,C,V,Z;
static unsigned char MEM[4000];

static unsigned int word;
static unsigned int operand1;
static unsigned int operand2;
static unsigned int destination;
static unsigned int opcode; 
static unsigned int format;

void main(int argc, char** argv) 
{
	char* prog_mem_file; 
	argc=3;
	if(argc < 2) 
	{
		printf("Incorrect number of arguments. Please invoke the simulator \n\t./myARMSim <input mem file> \n");
		exit(1);
	}
	reset_proc();
	load_program_memory(argv[1]);
	run_armsim();
}

void run_armsim() 
{
	while(1) 
	{
		fetch();
		decode();
		execute();
		mem();
		write_back();
		printf("\n");
	}
}

void reset_proc() 
{
	int i;
	for(i=0;i<15;i++)
		R[i]=0;
	for(i=0; i<4000; i++)
		MEM[i]=0;
	R[13]=4000;
}

void load_program_memory(char *file_name) 
{
	FILE *fp;
	unsigned int address, instruction;
	fp = fopen("fib.mem", "r");
	if(fp == NULL) 
	{
		printf("Error opening input mem file\n");
		exit(1);
	}
	while(fscanf(fp, "%x %x", &address, &instruction) != EOF)
		write_word(MEM, address, instruction);
	fclose(fp);
}

void write_data_memory() 
{
	FILE *fp;
	unsigned int i;
	fp = fopen("data_out.mem", "w");
	if(fp == NULL) 
	{
		printf("Error opening dataout.mem file for writing\n");
		return;
	}
	for(i=0; i < 4000; i = i+4)
		fprintf(fp, "%x %x\n", i, read_word(MEM, i));
	fclose(fp);
}

void swi_exit() 
{
	write_data_memory();
	printf("EXIT:\n");
	exit(0);
}
int read_word(char *mem, unsigned int address) 
{
	int *data;
	data =  (int*) (mem + address);
	return *data;
}

void write_word(char *mem, unsigned int address, unsigned int data) 
{
	int *data_p;
	data_p = (int*) (mem + address);
	*data_p = data;
}
void fetch() 
{
	word = read_word(MEM, R[15]);
	printf("Fetch instruction 0x%x from address 0x%x\n",word, R[15]);
	R[15]+=4;
}

void decode() 
{
	if(word==0xEF000011)
	{
		mem();
		swi_exit();
	}
	format=(word&0x0C000000)>>26;
	unsigned static int Rn, Rd, i, op2;
	if(format==0)
	{
		printf("DECODE: Operation is ");
		opcode = (word&0x01E00000)>>21;
		if(opcode==0)
			printf("AND");
		if(opcode==2) 
			printf("SUB");
		if(opcode==4) 
			printf("ADD");
		if(opcode==10) 
			printf("CMP");
		if(opcode==12)
			printf("ORR");
		if(opcode==13) 
			printf("MOV");
		if(opcode==15)
			printf("MNV");
	    
		Rn = (word&0x000F0000)>>16;
		Rd = (word&0x0000F000)>>12;

		if(Rn)
			printf(", First operand is R%d", Rn);
		operand1 = R[Rn];
		i = (word&0x02000000)>>25;
		if(i==0)
		{ 
			op2 = (word&0xF);
			if(!Rn) 
				printf(", immediate Second Operand is R%d,", op2);
			else 
				printf(", Second operand is R%d", op2);
			operand2 = R[op2];
		}
		if(i==1)
		{ 
			op2 = (word&0xFF);
			if(!Rn) 
			{
				printf(", First Operand is R%d",Rn);
				printf(", immediate Second Operand is %d,", op2);
			}
			else 
				printf(", Second operand %d", op2);
			operand2 = op2;
		}
		if(Rd)
			printf("\nDestination register is R%d\n", Rd);
		destination = Rd;
			
		if(Rn||i)
		{
			printf("Read registers: ");
			if(i)
				printf("R%d = %d", Rn, R[Rn]);
			if(Rn)
				printf("R%d = %d", Rn, R[Rn]);
			if(Rn&&!i) 
				printf(", ");
			if(!i)
				printf("R%d = %d", op2, R[op2]);
			printf("\n");
		}
}
	if(format==1)
	{ 
		printf("DECODE: Operation is ");
		opcode = (word&0x03F00000)>>20;
		if(opcode==25)
			printf("LDR");
		if(opcode==24)
			printf("STR");
	
		Rn = (word&0x000F0000)>>16;
		Rd = (word&0x0000F000)>>12;
		if(Rn)
			printf(", base address at R%d", Rn);
		if(Rd)
			printf(", destination register is R%d\n", Rd);
		destination = Rd;
		operand1 = Rn;
		printf("Read Register R%d = %x", Rn, R[Rn]);
		if(opcode==24) 
			printf(", R%d = %d", Rd, R[Rd]);
		printf("\n");
}
	if(format==2)
	{ 
		opcode = (word&0xF0000000)>>28;
		operand2 = (word&0xFFFFFF);
		printf("DECODE: Operation is Branch ");
		if(opcode==0)
			printf("Equal, ");
		if(opcode==1)
			printf("Not Equal, ");
		if(opcode==11)
			printf("Less Than, ");
		if(opcode==13)
			printf("Less Than or Equal, ");
		if(opcode==12)
			printf("Greater Than, ");
		if(opcode==10)
			printf("Greater Than or Equal, ");
		if(opcode==14)
			printf("Always, ");
		printf("with offset 0x%x\n", operand2);
	}
}

void execute() 
{
	printf("EXECUTE: ");
	if(format==0)
	{
		if(opcode==0)
		{ 
			printf("AND %d and %d\n", operand1, operand2);
			operand1 = operand1&operand2;
		}
		if(opcode==4)
		{ 
			printf("ADD %d and %d\n", operand1, operand2);
			operand1 = operand1+operand2;
		}
		if(opcode==2)
		{ 
			printf("SUB %d and %d\n", operand1, operand2);
			operand1 = operand1-operand2;
		}
		if(opcode==10)
		{ 
			printf("CMP %d and %d\n", operand1, operand2);
			Z=0; N=0;
			if(operand1-operand2==0)
			{
				Z=1;
				printf("EXECUTE: Z updated to 1\n");
			}
			if(operand1-operand2<0)
			{
				N=1;
				printf("EXECUTE: N updated to 1\n");
			}
		}
		if(opcode==12)
		{ 
			printf("OR %d and %d\n", operand1, operand2);
			operand1 = operand1|operand2;
		}
		if(opcode==13)
		{
			printf("MOV %d in R%d\n",operand2,destination);
			operand1 = operand2;
		}
		if(opcode==15)
		{
			printf("MNV %d\n", operand2);
			operand1 = ~operand2;
		}
}
	if(format==1)
		printf("No execute operation\n");
	if(format==2)
	{
		operand1 = 0;
		if(opcode==0 && Z)
			operand1 = 1;
		if(opcode==1 && !Z)
			operand1 = 1;
		if(opcode==11 && N)
			operand1 = 1;
		if(opcode==13 && N||Z)
			operand1 = 1;
		if(opcode==12 && !N)
			operand1 = 1;
		if((opcode==10) && (!N&&Z))
			operand1 = 1;
		if(opcode==14)
			operand1 = 1;
		if(operand1==1)
		{
			unsigned int s = (operand2&0x800000)<<1, j;
			for(j=0;j<8;j++,s<<=1) 
				operand2+=s;
			operand2<<=2;
		
			R[15]+=(signed int)operand2;
			printf("Updating PC to 0x%x\n", R[15]);
		}
		else
			printf("No execute operation\n");
		
	}
}
//perform the memory operation
void mem() 
{
	printf("MEMORY: ");
	if(format==0)
		printf("No memory operation\n");
	if(format==1 && opcode==25)
	{
		unsigned int data = read_word(MEM, R[operand1]);
		printf("Read %d from address %x\n", data, R[operand1]);
		operand1 = data;
	}
	if(format==1 && opcode==24)
	{ 
		write_word(MEM, R[operand1], R[destination]);
		printf("Write %d to address %x\n", R[destination], R[operand1]);
	}
	}
	if(format==2)
		printf("No memory operation\n");
	
}
//writes the results back to register file
void write_back() 
{
	printf("WRITEBACK: ");
	if(format==0 && destination!=0)
	{
		R[destination] = operand1;
		printf("write %d to R%d\n", operand1, destination);
	}
	else
		printf("no writeback operation\n");
	if(format==1 && opcode==25)
	{
		R[destination] = operand1;
		printf("write %d to R%d\n", operand1, destination);
	}
	else
		printf("no writeback operation\n");
	if(format==2)
		printf("No writeback operation\n");
	
}

