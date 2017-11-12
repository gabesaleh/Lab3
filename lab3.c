//
//  lab3.c
//
//
//  Created by Gabriel Saleh and Cameron Weiss 11/6/17.
//  Copyright © 2017 Gabe Saleh. All rights reserved.
//

// List the full names of ALL group members at the top of your code.
/*** Gabriel Saleh and Cameron Weiss ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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



void IF(int program_count);
void ID();
void EX(int programcount);
void MEM();
void WB(long *mips_register[]);

struct inst parser(char *input);
char *progScanner(FILE *inputfile, FILE *outputfile);
char *regNumberConverter(char *input);



//helper fucntions
void removeCharacter(char *string, char delimiter);
void printStruct(struct inst instruct);

main (int argc, char *argv[])
{
    int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
    int c,m,n;
    int i;//for loop counter
    long *mips_reg[REG_NUM];
    long pgm_c=0;//program counter
    long sim_cycle=0;//simulation cycle counter
    //define your own counter for the usage of each pipeline stage here
    
    int test_counter=0;
    FILE *input=NULL;
    FILE *output=NULL;
    
    input=fopen(argv[1],"r");
    output=fopen(argv[2],"w");
    
   // printf("The arguments are:");
    
   /* for(i=1;i<argc;i++)
    {
        printf("%s ",argv[i]);
    }
    
    printf("\n");
    
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
        
    }
    
    else
    {
        printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
        printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
        exit(0);
    }
    /*if(input==NULL)
    {
        printf("Unable to open input or output file\n");
        exit(0);
    }
    if(output==NULL)
    {
        printf("Cannot create output file\n");
        exit(0);
    }
     */
    //initialize registers and program counter
       /*for (i=0;i<REG_NUM;i++)
        {
            *mips_reg[i]=0;
        }
    
    
    
    struct inst test;
    test.opcode = ADD;
    test.rs = 1;
    test.rt = 2;
    test.rd = 5;
    test.Imm = 0;
    
    instructions[0] = test;
    
    IF(pgm_c);
    ID();
    EX(pgm_c);
    MEM();
    WB(mips_reg);
    
    for (i=1;i<REG_NUM;i++){
        printf("%ld  ",*mips_reg[i]);
    }*/
    
    //parser(regNumberConverter(progScanner(input, output)));
    
    //char testinput[] = "sw 1 2 3";
    
   /* char progOutput[] = "add $s1 $s2 $s3";
    //printf("%s", progOutput);
    
    char *regOutput;
    regOutput = regNumberConverter(progOutput);
    printf("%s", regOutput);
    
    //printStruct(parser(regNumberConverter()));*/
    
    char testinput[100];
    
    
    char *progoutpoint = progScanner(input, output);
    
    strcpy(testinput, progoutpoint);
    char *regoutput = regNumberConverter(testinput);
    printf("%s\n", regoutput);
    printStruct(parser(regoutput));
    
    
    
    //start your code from here
}

/* Reads as input a pointer to a string holding the next line from the assembly language program
 * using the fgets() library function to do so. Removes all duplicate spaces, parentheses, and
 * commas from it and a pointer to the resulting character string will be returned. Items separated
 * in this character string separated soley by a single space.
 * add $s0, $s1, $s2 --> add $s0 $s1 $s2
 */
char *progScanner(FILE *inputfile, FILE *outputfile)
{
    
    char string[100];
    char *errorString = "Can't open file";
    
    if(inputfile == NULL)
    {
        perror("Error opening file");
        return(errorString);
        
    }
    
    
    if(fgets(string,100,inputfile)!=NULL)
    {
        
        removeCharacter(string, '(');
        removeCharacter(string, ')');
        removeCharacter(string, ',');
        removeCharacter(string, ' ');
        
        //printf("%s", string);
        
        fprintf(outputfile, "%s", string);
        
    }
    

    
    fclose(inputfile);
    fclose(outputfile);
    //printf("%s", string);
    char *pointer = string;
    
    return pointer;
}

/*   removes specified character from a given string
 *   *** used in progScanner to remove '(' ',' and extra spaces ***
 *   returns fixed string
 */
void removeCharacter(char *string, char delimiter)
{
    char *src, *destination;
    //int spaceCount = 0;
    for(src = destination = string; *src != '\0'; src++)
    {
        
        *destination = *src;
        
        if(delimiter == ' ')
        {
            if(*destination == delimiter && *(destination-1) == delimiter){}
            else
                destination++;
        }
        
        else if(*destination != delimiter)
        {
                destination++;
        }
    }
    *destination='\0';
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
    /*, "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp" "ra"};*/
    char* deconstruct[5];
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
    for(j= 0; j<31; j++)//traverse all possible registers
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
    
    parserInst.op = 0;
    parserInst.rt = 0;
    parserInst.rd = 0;
    parserInst.rs = 0;
    parserInst.Imm = 0;
    
    char *token = strtok(input, " ");
    
    int count = 0;
    while(token != NULL)
    {
        instruction[count] = token;
        //printf("%s \n",instruction[count]);
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
            
            
        }
        else if(i == 1)
        {
            if(strcmp(instruction[0], "addi") == 0 || strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                parserInst.rt = atoi(instruction[i]);
            }
            
            else if(strcmp(instruction[0], "mult") == 0 || strcmp(instruction[0], "beq") == 0)
            {
                parserInst.rs = atoi(instruction[i]);
            }
            else
            {
            parserInst.rd = atoi(instruction[i]);
            }
        }

        else if(i == 2)
        {
            if(strcmp(instruction[0], "mult") == 0 || strcmp(instruction[0], "beq") == 0)
            {
                parserInst.rt = atoi(instruction[i]);
            }
            
            else if(strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                parserInst.Imm = atoi(instruction[i]);
            }
            else
            {
            parserInst.rs = atoi(instruction[i]);
            }
        }
        
        else if(i == 3)
        {
            if(strcmp(instruction[0], "addi") == 0 || strcmp(instruction[0], "beq") == 0)
            {
                parserInst.Imm = atoi(instruction[i]);
            }
            
            else if(strcmp(instruction[0], "lw") == 0 || strcmp(instruction[0], "sw") == 0)
            {
                parserInst.rs = atoi(instruction[i]);
            }
            else{
            parserInst.rt = atoi(instruction[i]);
            }
        }
        
    }
    
    
    return parserInst;
    

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




void IF(int program_count)
{
    struct inst curr;
    
    curr=instructions[program_count];
    
    if_latch.prev = curr;

    if_latch.next = if_latch.prev;
    
    

    
}

void ID()
{
    struct inst curr;
    
    id_latch.next = id_latch.prev;
    
    curr = if_latch.next;
    
    id_latch.prev = if_latch.next;
    
    ex_latch.prev = if_latch.next;
}

void EX(int programcount)
{
    struct inst curr;
    
    ex_latch.next = ex_latch.prev;
    
    curr = id_latch.next;
    
    ex_latch.prev = id_latch.next;
    
    mem_latch.prev = ex_latch.next;
    
    if(curr.op == 0)//add
    {
        mem_latch.t1 = curr.rs + curr.rt;
        
    }
    
    if(curr.op == 1)//addi
    {
        mem_latch.t1 = curr.rs + curr.Imm;
    }
    
    if(curr.op == 2)//sub
    {
        mem_latch.t1 = curr.rs - curr.rt;
    }
    
    if(curr.op == 3)//mult
    {
        mem_latch.t1 = curr.rs*curr.rt;
    }
    
    if(curr.op == 4)//beq
    {
        if(curr.rs == curr.rt)
            programcount = programcount + 4 + (curr.Imm<<2);
        ex_latch.t2 = programcount;
    }
    
    if(curr.op == 5)//LW
    {
        ex_latch.t1 = curr.rs + curr.Imm;
    }
    
    if(curr.op == 6)//SW
    {
        ex_latch.t1 = curr.rs + curr.Imm;
    }
}

void MEM()
{
    struct inst curr;
    
    mem_latch.next = mem_latch.prev;
    
    curr = ex_latch.next;
    
    mem_latch.prev = ex_latch.next;
    
    if(curr.op == LW)
    {
        mem_latch.t1 = dataMemory[ex_latch.t1];
    }
    else if(curr.op == SW)
    {
        dataMemory[ex_latch.t1] = curr.rt;
    }
    
}

void WB(long *mips_register[])
{
    struct inst curr;
    
    curr = mem_latch.next;
    
    if ((curr.op != SW) && (curr.op != BEQ))
    {
        mips_register[curr.rd] = &mem_latch.t1;
    }
    
    
}
