//
//  lab3.c
//
//
//  Created by Gabriel Saleh and Cameron Weiss 11/6/17.
//  Copyright © 2017 Gabe Saleh. All rights reserved.
//
//NOTE: We emailed Professor Krishna and told him our problems with quark. We have tested this code on both Cyguin and the Mac terminal and it works without segmentation fault.
// Once I upload the code to quark and run it there is a segmentation fault. This is due to the strcpy, strcat, and strcmp functions. Tried the functions given on moodle and still did not work.
//
// List the full names of ALL group members at the top of your code.
/*** Gabriel Saleh and Cameron Weiss ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <assert.h>
//feel free to add here any additional library names you may need


#define SINGLE 1
#define BATCH 0
#define REG_NUM 32
#define MAXSIZE 512

enum opcode {ADD,ADDI,SUB,MULT,BEQ,LW,SW};

struct inst
{
    enum opcode op;
    int rs;
    int rd;
    int rt;
    int Imm;

};

struct latch
{
    struct inst prev;
    struct inst next;
    long t1;
    int t2;
};

struct latch if_latch; //latch between IF and ID
struct latch id_latch; //latch between ID and EX
struct latch ex_latch; //latch between EX and MEM
struct latch mem_latch; //latch between MEM and WB



int counter = 0;
char *rawInstruction[5000];
char *cleanInstruction[5000];
struct inst instructions[5000];
int dataMemory[MAXSIZE];
long pgm_c = 0;//program counter
double ifUtil,idUtil,exUtil, memUtil, wbUtil = 0;



void IF(long program_count);
void ID();
void EX(long program_count, long mips_register[]);
void MEM(long mips_register[]);
void WB(long mips_register[], int cyclescount);

struct inst parser(char *input);
char *progScanner(FILE *inputfile, FILE *outputfile, int lineNum);
char *regNumberConverter(char *input);
long offset = 0;


//helper fucntions
int removeCharacter(char *string, char delimiter);
void printStruct(struct inst instruct);
void illegelRegister(char *reg);
void printRegs(long *mips_regs[]);

int main (int argc, char *argv[])
{
    int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
    int c,m,n;
    int i;//for loop counter
    long *mips_reg[REG_NUM];
    long sim_cycle=0;//simulation cycle counter
    long instruct_count = 0;
    //define your own counter for the usage of each pipeline stage here
    int testCounter = 0;
    
    
    
    FILE *input=NULL;
    FILE *output=NULL;
    
    
    if(argc==7)
    {
        if(strcmp("-s",argv[1])==0)
        {
            
            sim_mode=SINGLE;
        }
        else if(strcmp("-b",argv[1])==0)
        {
            sim_mode=BATCH;
        }
        else
        {
            printf("Wrong sim mode chosen\n");
            exit(0);
        }
        
        m=atoi(argv[2]);
        n=atoi(argv[3]);
        c=atoi(argv[4]);
        
        input=fopen(argv[5],"r");
        output=fopen(argv[6],"w");
        
        assert(m && n && c > 0);
        
    }
    
    else
    {
        printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
        printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
        exit(0);
    }
    
    

    

    
    if(input==NULL)
    {
        printf("Unable to open input or output file\n");
        exit(0);
    }
    if(output==NULL)
    {
        printf("Cannot create output file\n");
        exit(0);
    }
    
    
    //initialize registers and program counter
    for(i=0;i<REG_NUM;i++)
    {
            mips_reg[i]=0;
    }
    for(i=0;i<512;i++)
    {
        dataMemory[i]=0;
    }
    
    
    
    
    char testinput[100];
    char *progout;
    int ch;
    int lines = 0;
    
    while(!feof(input))
    {
        ch = fgetc(input);
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(input);
    fopen(argv[5], "r");
    
    lines++;
    int b;
    for(b =1; b<lines; b++)
    {
        progout = progScanner(input, output, b);
        memcpy(testinput, progout, 50);
        char *regoutput = malloc(sizeof(char)*500);
        
        regoutput = regNumberConverter(testinput);
        instructions[instruct_count] = parser(regoutput);
        if(instruct_count == 0)
        {
            if(instructions[instruct_count].op == SW)
            {
                sim_cycle =  4;
            }
            else if(instructions[instruct_count].op == BEQ)
            {
                sim_cycle =  2;
            }
            else
            {
                sim_cycle =  1;
            }


        }
        
        
        if(instructions[instruct_count].op == MULT)
        {
            sim_cycle += 4+m;
        }
        else if(instructions[instruct_count].op == ADD)
        {
            sim_cycle += 4+n;
        }
        else if(instructions[instruct_count].op == ADDI)
        {
            sim_cycle += 4+n;
        }
        else if(instructions[instruct_count].op == SUB)
        {
            sim_cycle += 4+n;
        }
        else if(instructions[instruct_count].op == LW)
        {
            sim_cycle += 4+c;
        }
        else if(instructions[instruct_count].op == SW)
        {
            sim_cycle += 1+c;
        }
        else if(instructions[instruct_count].op == BEQ)
        {
            sim_cycle += 3+n;
        }
        
        instruct_count++;
        
        
        int cyclecount = 0;
        int b = 0;
        for(i = 0; i < 5; i++)
        {
            
            WB(mips_reg, cyclecount);
            MEM(mips_reg);
            EX(pgm_c, mips_reg);
            ID();
            IF(pgm_c);
            cyclecount++;
            
        }
        pgm_c++;
        if(sim_mode==1){
        printf("cycle: %ld ",sim_cycle);
            for (i=1;i<REG_NUM;i++){
                printf("%d  ",(int)mips_reg[i]);
            }
        
        printf("%ld\n",pgm_c);
        printf("press ENTER to continue\n");
        while(getchar() != '\n');
        
        }
    }
    
    if(sim_mode==0){
        fprintf(output,"program name: %s\n",argv[5]);
        fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",ifUtil, idUtil, exUtil, memUtil, wbUtil);
        // add the (double) stage_counter/sim_cycle for each
        // stage following sequence IF ID EX MEM WB
        
        fprintf(output,"register values ");
        for (i=1;i<REG_NUM;i++){
            fprintf(output,"%d  ",mips_reg[i]);
        }
        fprintf(output,"%ld\n",pgm_c);
        
    }
    
    fclose(input);
    fclose(output);
}

void printRegs(long *mips_regs[])
{
    int i = 0;
    for(i=1;i<REG_NUM;i++)
    {
        printf("REG(%d): %ld  \n",i,mips_regs[i]);
    }
    
}

/* Reads as input a pointer to a string holding the next line from the assembly language program
 * using the fgets() library function to do so. Removes all duplicate spaces, parentheses, and
 * commas from it and a pointer to the resulting character string will be returned. Items separated
 * in this character string separated soley by a single space.
 * add $s0, $s1, $s2 --> add $s0 $s1 $s2
 */
char *progScanner(FILE *inputfile, FILE *outputfile, int lineNum)
{
    
    char string[100];
    char *errorString = "Can't open file";
    int rightPCount;
    int leftPCount;
    
    
    if(inputfile == NULL)
    {
        perror("Error opening file");
        return(errorString);
        
    }

        fgets(string,100,inputfile);
    
        rightPCount = removeCharacter(string, '(');
        leftPCount = removeCharacter(string, ')');
        if(rightPCount != leftPCount)
        {
            printf("Mismatching parenthesis!\n");
            exit(1);
        }
        
        removeCharacter(string, ',');
        removeCharacter(string, ' ');
        
    
    
    char *pointer = string;
    
    return pointer;
}

/*   removes specified character from a given string
 *   *** used in progScanner to remove '(' ',' and extra spaces ***
 *   returns fixed string
 */
int removeCharacter(char *string, char delimiter)
{
    char *src, *destination;
    int counter = 0;
    
    for(src = destination = string; *src != '\0'; src++)
    {
        
        *destination = *src;
        
        if(delimiter == ' ')
        {
            if(*destination == delimiter && *(destination-1) == delimiter){}
            else
                destination++;
        }
        else if(delimiter == '(')
        {
            
            if(*destination == delimiter)
            {
                counter++;
                
                *destination = ' ';
                destination++;
            }
            else
                destination++;
        }
        
        else if(*destination != delimiter)
        {
                destination++;
        }
        else if(*destination == delimiter)
        {
            counter++;
        }
        
    }
    *destination='\0';
    return counter;
    
}

/* This function accepts as input the output of progScanner() and returns a pointer to a character
 * string in which all registernames are converted to numbers
 * $zero = $0 , $t0 = $8, parser needs to handle either representation
 * scan up until $, and read whatever is after that. If illegal register detected error reported
 */
char *regNumberConverter(char *input)
{
    char** registers;
    registers = malloc(40*sizeof(char*));
    registers[0] = "zero";
    registers[1] = "at";
    registers[2] = "v0";
    registers[3] = "v1";
    registers[4] = "a0";
    registers[5] = "a1";
    registers[6] = "a2";
    registers[7] = "a3";
    registers[8] = "t0";
    registers[9] = "t1";
    registers[10] = "t2";
    registers[11] = "t3";
    registers[12] = "t4";
    registers[13] = "t5";
    registers[14] = "t6";
    registers[15] = "t7";
    registers[16] = "s0";
    registers[17] = "s1";
    registers[18] = "s2";
    registers[19] = "s3";
    registers[20] = "s4";
    registers[21] = "s5";
    registers[22] = "s6";
    registers[23] = "s7";
    registers[24] = "t8";
    registers[25] = "t9";
    registers[26] = "k0";
    registers[27] = "k1";
    registers[28] = "gp";
    registers[29] = "sp";
    registers[30] = "fp";
    registers[31] = "ra";
   
    char **deconstruct;
    deconstruct = malloc(sizeof(char*) * 50);
    char* output1;
    char* output2;
    char* output3;
    char* output4;
    char* output5;
    char* outfinal;
    int i,j,g;
    
    outfinal = malloc(sizeof(char) * 40);
    
    
    //remove all $ from input
    
    removeCharacter(input, '$');
    
    char *token = strtok(input, " ");
    int z = 0;
    
    while(token!=NULL)
    {
        deconstruct[z] = token;
        token = strtok(NULL, " ");
        z++;//length of deconstruct
    }
    
    i=1;
    for(j= 0; j<32; j++)//traverse all possible registers
    {
        for(g=0; g<z; g++)
        {
            if(!strncmp(deconstruct[g], registers[j], 2))
            {
                sprintf(deconstruct[g], "%d", j);
            }
            else if(!strncmp(deconstruct[g], "zero", 4))
            {
                sprintf(deconstruct[g], "%d", j);
            }
        }
    }
    
    
    
    output1 = deconstruct[0];
    output2 = deconstruct[1];
    output3 = deconstruct[2];
    
    
    
    memcpy(outfinal,output1,40);
    
    strcat(outfinal, " ");
    strcat(outfinal, output2);
    strcat(outfinal, " ");
    strcat(outfinal, output3);
    
    if(z>3)
    {
        output4 = deconstruct[3];
        strcat(outfinal, " ");
        strcat(outfinal, output4);
    }
    if(z>4)
    {
        output5 = deconstruct[4];
        strcat(outfinal, " ");
        strcat(outfinal, output5);
    }
    
    return outfinal;
    

}

/* This function uses the output of regNumberConverter(). The instruction is returned as an inst
 * struct with fields for each of the fields of MIPS instructions including: opcode, rs, rt, rd,
 * Imm. Of course not all of the fields will be present for all instructions. Integers only.
 * suggestion: use enumeration type to describe opcodes, e.g. enum inst{ADD, ADDI, SUM....etc.}
 * You can assume that the assembly language instructions will all be in lower case.
 * Any illegal opcode, immediate field that contains too large of number, missing $, should lead
 * to an error.
 * parser() should place the parsed instruction (in struct form) into the linear array that is used
 * to represent the Instruction Memory
 */

struct inst parser(char *input)
{
    struct inst parserInst;
    char* instruction[5];
    int i;
    char *ptr;
    
    
    char *token = strtok(input, " ");
    int count = 0;
    while(token != NULL)
    {
        instruction[count] = token;
        token = strtok(NULL, " ");
        count++;
    }
    
    
    for(i = 0; i < count; i++)
    {
        if(i == 0)
        {
            if(!strcmp(instruction[i], "add"))
                parserInst.op = ADD;
            else if(!strcmp(instruction[i], "addi"))
                parserInst.op = ADDI;
            else if(!strcmp(instruction[i], "sub"))
                parserInst.op = SUB;
            else if(!strcmp(instruction[i], "mult"))
                parserInst.op = MULT;
            else if(!strcmp(instruction[i], "beq"))
                parserInst.op = BEQ;
            else if(!strcmp(instruction[i], "sw"))
                parserInst.op = SW;
            else if(!strcmp(instruction[i], "lw"))
                parserInst.op = LW;
            else
            {
                printf("Illegal instruction!\n");
                exit(1);
            }
            
            
        }
        else if(i == 1)
        {
            /*strtol(instruction[i], &ptr, 10);
            if(*ptr != '\0' || ptr == instruction[i])
            {
                printf("Illegal register!\n");
                exit(1);
            }*/
            
            if(strcmp(instruction[0], "addi") == 0 || strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rt = atoi(instruction[i]);
            }
            
            else if(strcmp(instruction[0], "mult") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rd = atoi(instruction[i]);
            }
            else if(strcmp(instruction[0], "beq") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rs = atoi(instruction[i]);
            }
            else
            {
                illegelRegister(instruction[i]);
                parserInst.rd = atoi(instruction[i]);
            }
        }

        else if(i == 2)
        {
            /*strtol(instruction[i], &ptr, 10);
            if(*ptr != '\0' || ptr == instruction[i])
            {
                printf("Illegal register!\n");
                exit(1);
            }*/
            
            if(strcmp(instruction[0], "mult") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rs = atoi(instruction[i]);
            }
            else if(strcmp(instruction[0], "beq") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rt = atoi(instruction[i]);
            }
            else if(strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                parserInst.Imm = atoi(instruction[i]);
            }
            else
            {
                illegelRegister(instruction[i]);
            parserInst.rs = atoi(instruction[i]);
            }
        }
        
        else if(i == 3)
        {
            /*strtol(instruction[i], &ptr, 10);
            if(*ptr != '\0' || ptr == instruction[i])
            {
                printf("Illegal register!\n");
                exit(1);
            }
            */
            if(strcmp(instruction[0], "addi") == 0 || strcmp(instruction[0], "beq") == 0)
            {
                parserInst.Imm = atoi(instruction[i]);
            }
            
            else if(strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                illegelRegister(instruction[i]);
                parserInst.rs = atoi(instruction[i]);
            }
            else
            {
                illegelRegister(instruction[i]);
                parserInst.rt = atoi(instruction[i]);
            }
        }
        
    }
    
    
    return parserInst;
    

}

void illegelRegister(char *reg)
{
    if(atoi(reg) > 31)
    {
        printf("Illegal register!\n");
        exit(1);
    }
}

void printStruct(struct inst instruct)
{
    char *opcodeString;
    
    switch (instruct.op)
    {
      case ADD:
            opcodeString = "add";
            break;
      case ADDI:
            opcodeString = "addi";
            break;
      case SUB:
            opcodeString = "sub";
            break;
      case MULT:
            opcodeString = "mult";
            break;
      case BEQ:
            opcodeString = "beq";
            break;
      case LW:
            opcodeString = "lw";
            break;
      case SW:
            opcodeString = "sw";
            break;
      default:
            break;
            
    }
    
    printf("Opcode: %s, rs: %d, rt: %d, rd: %d, Imm: %d \n", opcodeString, instruct.rs, instruct.rt, instruct.rd, instruct.Imm);
}

void printMemLatch(struct latch memory_latch)
{
    printf("t1: %ld\n", memory_latch.t1);
}



void IF(long program_count)
{
    
    struct inst curr;
    
    curr=instructions[pgm_c];
    
    if_latch.next = curr;
    ifUtil++;
    
    
    
}

void ID()
{
    struct inst curr;
    
    curr = if_latch.next;
    
    id_latch.next = curr;
    
    id_latch.prev = if_latch.next;
    
    ex_latch.prev = if_latch.next;
    idUtil++;
    
}

void EX(long program_count, long mips_register[])
{
    struct inst curr;
    
    ex_latch.next = ex_latch.prev;
    
    curr = id_latch.next;
    
    ex_latch.prev = id_latch.next;
    
    mem_latch.prev = ex_latch.next;
    
    exUtil++;
    
    
    if(curr.op == ADD)
    {
        mem_latch.t1 = mips_register[curr.rs] + mips_register[curr.rt];
        
    }
    
    if(curr.op == ADDI)//addi
    {
        mem_latch.t1 = mips_register[curr.rs] + curr.Imm;
    }
    
    if(curr.op == SUB)//sub
    {
        mem_latch.t1 = mips_register[curr.rs] - mips_register[curr.rt];
    }
    
    if(curr.op == MULT)//mult
    {
        mem_latch.t1 = mips_register[curr.rs]*mips_register[curr.rt];
    }
    
    if(curr.op == BEQ)//beq
    {
        
    }
    
    if(curr.op == LW)//LW
    {
        ex_latch.t1 = mips_register[curr.rs] + curr.Imm;
    }
    
    if(curr.op == SW)//SW
    {
        ex_latch.t1 = mips_register[curr.rs] + curr.Imm;
    }
    
}

void MEM(long mips_register[])
{
    struct inst curr;
    
    memUtil++;
    
    mem_latch.next = mem_latch.prev;
    
    curr = ex_latch.next;
    
    mem_latch.prev = ex_latch.next;
    
    if(curr.op == LW)
    {
        mem_latch.t1 = dataMemory[ex_latch.t1];
    }
    else if(curr.op == SW)
    {
         dataMemory[ex_latch.t1] = mips_register[curr.rt];
    }
    
    
    
}

void WB(long mips_register[], int cyclescount)
{
    struct inst curr;
    
    curr = mem_latch.next;
    
    wbUtil++;
    
   
    if(cyclescount>3)
    {
        if ((curr.op != SW) && (curr.op != BEQ) && (curr.op != ADDI) && (curr.op != LW))
        {
            mips_register[curr.rd] = mem_latch.t1;
        }
        else if (curr.op == LW)
        {
            mips_register[curr.rt] = mem_latch.t1;
        }
        else if(curr.op == ADDI)
        {
        
            mips_register[curr.rt] = mem_latch.t1;
        }
    }
    
}

/*

int my_strcmp (const char * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

char *my_strcat(char *dest, const char *src)
{
    size_t i,j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];
    dest[i+j] = '\0';
    return dest;
}
*/
