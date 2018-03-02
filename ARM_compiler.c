#include <stdlib.h>
#include <stdio.h>

void run_armsim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();
void swi_exit();
int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);

void fetch();
void decode();
void execute();
void memory();
void write_back();

static unsigned int R[16];
static int check_flag;
static int N;
static unsigned char MEM[4000];
static unsigned int word;
static int flaggy;
static unsigned int operand1;
static int C;
static unsigned int operand2;
static unsigned int destination;
static int V;
static unsigned int opcode; 
static unsigned int format;
static int Z;

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
		memory();
		write_back();
		printf("\n");
	}
}

void reset_proc() 
{
	int i;
	i=14;
	
	while(i>=0)
	{
		R[i] = 0;
		i--;
	}
	
	i = 3999;

	while(i>=0)
	{
		MEM[i]=0;
		i--;
	}

	R[13]=4000;
}

void load_program_memory(char *file_name) 
{
	FILE *fp;
	int flag;
	unsigned int address, instruction;
	flag = 1;
	fp = fopen("input.mem", "r");
	if(fp != NULL) 
	{}
	else
	{	printf("Error opening input mem file\n");
		exit(1);
	}
	while(fscanf(fp, "%x %x", &address, &instruction) != EOF)
	{
		flag=0;
		write_word(MEM, address, instruction);
	}
	fclose(fp);
}

void write_data_memory() 
{
	FILE *fp;
	int flag1=0;
	unsigned int i;
	fp = fopen("data_out.mem", "w");
	flag1=1;
	if(fp != NULL) 
	{}
	else
	{
		printf("Error opening dataout.mem file for writing\n");
		return;
	}
	i=0;
	while(i<4000)
	{
		fprintf(fp, "%x %x\n", i, read_word(MEM, i));
		i+=4;
	}
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
	int flag=1;
	data =  (int*) (mem + address);
	if(flag==1){flag=0;}
	return *data;
}

void write_word(char *mem, unsigned int address, unsigned int data) 
{
	int flag=1;
	int *data_p;
	while(flag<10){flag++;}
	data_p = (int*) (mem + address);
	flag=0;
	*data_p = data;
}
void fetch() 
{
	int flag=0;
	word = read_word(MEM, R[15]);
	flag=1;
	printf("Fetch instruction 0x%x from address 0x%x\n",word, R[15]);
	while(flag>0){flag--;}
	R[15]+=4;
}

void decode() 
{
	unsigned static int Rn, Rd, i, op2;
	if(word==0xEF000011)
	{
		memory();
		swi_exit();
	}
	format=(word&0x0C000000)>>26;
	if(format==0)
	{
		printf("DECODE: Operation is ");
		opcode=(word&0x01E00000)>>21;
		switch(opcode)
		{
			case 0:
			{
				printf("AND");
				break;
			}
			case 2: 
			{
				printf("SUB");
				break;
			}
			case 4: 
			{
				printf("ADD");
				break;
			}
			case 10: 
			{
				printf("CMP");
				break;
			}
			case 12:
			{
				printf("ORR");
				break;
			}
			case 13: 
			{
				printf("MOV");
				break;
			}
			case 15:
			{
				printf("MNV");
				break;
			}
	    	}
		Rn = (word&0x000F0000)>>16;
		int pool=5;while(pool>0){pool--;}
		Rd = (word&0x0000F000)>>12;

		if(!Rn){}
		else{printf(", First operand is R%d", Rn);}
		operand1=R[Rn];
		pool=6;
		i=(word&0x02000000)>>25;
		if(i==0)
		{ 
			op2=(word&0xF);
			while(pool>0){pool--;}
			if(Rn) {printf(", Second operand is R%d", op2);}
			else {printf(", immediate Second Operand is R%d,", op2);}
			pool=1;
			operand2=R[op2];
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
				printf("R%d = %d ", Rn, R[Rn]);
			if(!i)
				printf("R%d = %d ", op2, R[op2]);
			if(Rn)
				printf("R%d = %d ", Rn, R[Rn]);
			if(Rn&&!i) 
				printf(" ");
			printf("\n");
		}
}
	if(format==1)
	{ 
		printf("DECODE: Operation is ");
		opcode = (word&0x03F00000)>>20;
		if(opcode==24)
			printf("LDR");
		if(opcode==25)
			printf("STR");
		int flag=1;
		Rn = (word&0x000F0000)>>16;
		flag=5;while(flag>0){flag--;}
		Rd = (word&0x0000F000)>>12;
		flag=0;
		if(Rn){printf(", base address at R%d", Rn);}
		while(flag<10){flag++;}
		if(Rd){printf(", destination register is R%d\n", Rd);}
		destination = Rd;
		flag=1;
		operand1 = Rn;
		printf("Read Register R%d = %x", Rn, R[Rn]);
		flag=0;
		if(opcode==24) 
			printf(", R%d = %d", Rd, R[Rd]);
		printf("\n");
	}
	if(format==2)
	{ 
		printf("DECODE: Operation is Branch ");
		opcode = (word&0xF0000000)>>28;
		operand2 = (word&0xFFFFFF);
		switch(opcode)
		{
			case 0:
			{
				printf("Equal, ");
				break;
			}
			case 1:
			{
				printf("Not Equal, ");
				break;
			}
			case 11:
			{
				printf("Less Than, ");
				break;
			}
			case 13:
			{
				printf("Less Than or Equal, ");
				break;
			}
			case 12:
			{
				printf("Greater Than, ");
				break;
			}
			case 10:
			{
				printf("Greater Than or Equal, ");
				break;
			}
			case 14:
			{
				printf("Always, ");
				break;
			}
		}
		printf("with offset 0x%x\n", operand2);
	}
}

void execute() 
{
	printf("EXECUTE: ");
	if(format==0)
	{
		switch(opcode)
		{
			case 0:
			{ 
				printf("AND %d and %d\n", operand1, operand2);
				operand1 = operand1&operand2;
				break;
			}
			case 2:
			{ 
				printf("SUB %d and %d\n", operand1, operand2);
				operand1 = operand1-operand2;
				break;
			}
			case 4:
			{ 
				printf("ADD %d and %d\n", operand1, operand2);
				operand1 = operand1+operand2;
				break;
			}
		
			case 10:
			{ 
				int pool=10;
				printf("CMP %d and %d\n", operand1, operand2);
				pool=5;
				Z=0; N=0;
				if(operand1-operand2>0)
				{
					break;
				}
				else
				{
					unsigned int chcek = operand1-operand2;
					if(chcek==0)
					{
						Z=1;
						pool=1;
						printf("EXECUTE: Z updated to 1\n");
					}
					pool=5;
					if(chcek<0)
					{
						N=1;
						pool=0;
						printf("EXECUTE: N updated to 1\n");
					}

				}
				
			}
			case 12:
			{ 
				printf("OR %d and %d\n", operand1, operand2);
				operand1 = operand1|operand2;
				break;
			}
			case 13:
			{
				printf("MOV %d in R%d\n",operand2,destination);
				operand1 = operand2;
				break;
			}
			case 15:
			{
				printf("MNV %d\n", operand2);
				operand1 = ~operand2;
				break;
			}
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
		if(operand1!=1)
		{
			printf("No execute operation\n");
		}
		else
		{
			unsigned int s = (operand2&0x800000)<<1, j;
			int flag=1;
			j=0;
			while(j<8)
			{
				operand2+=s;
				j++;
				s<<=1;
			}
			// for(j=0;j<8;j++,s<<=1) 
			// {
			// 	operand2+=s;
			// }
			operand2<<=2;
			flag=2;
			R[15]+=(signed int)operand2;
			flag--;
			printf("Updating PC to 0x%x\n", R[15]);
			
		}
		
	}
}
void memory() 
{
	int pool=5;
	printf("MEMORY: ");
	while(pool>0){pool--;}
	if(format==0)
	{
		pool=5;
		printf("No memory operation\n");
	}
	pool=1;
	if(format==1 && opcode==24)
	{
		pool=0;
		unsigned int data = read_word(MEM, R[operand1]);
		while(pool<10){pool++;}
		printf("Read %d from address %x\n", data, R[operand1]);
		pool=0;
		operand1 = data;
	}
	pool=5;
	if(format==1 && opcode==25)
	{ 
		int jk;
		write_word(MEM, R[operand1], R[destination]);
		for(jk=0;jk<5;jk++){pool--;}
		printf("Write %d to address %x\n", R[destination], R[operand1]);
	}
	if(format==2)
		printf("No memory operation\n");
	
}
void write_back() 
{
	printf("WRITEBACK: ");
	int flag=1;
	if(format==0)
	{
		if(destination==0)
		{
			printf("no writeback operation\n");
		}
		else
		{
			R[destination] = operand1;
			flag=0;
			printf("write %d to R%d\n", operand1, destination);
			
		}
	}
	if(flag!=1){flag=0;}
	if(format==1)
	{
		if(opcode==25)
		{
			R[destination] = operand1;
			printf("write %d to R%d\n", operand1, destination);
		}
		else
			printf("no writeback operation\n");
	}
	if(format==2)
		printf("No writeback operation\n");
	
}

