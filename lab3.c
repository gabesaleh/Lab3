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

struct inst
{
    enum {ADD,ADDI,SUB,MULT,BEQ,LW,SW} opcode;
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
       for (i=0;i<REG_NUM;i++)
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
    }
    
    
    
    //start your code from here
}

/* Reads as input a pointer to a string holding the next line from the assembly language program
 * using the fgets() library function to do so. Removes all duplicate spaces, parentheses, and
 * commas from it and a pointer to the resulting character string will be returned. Items separated
 * in this character string separated soley by a single space.
 * add $s0, $s1, $s2 --> add $s0 $s1 $s2
 */
char *progScanner()
{
    return NULL;
}

/* This function accepts as input the output of progScanner() and returns a pointer to a character
 * string in which all registernames are converted to numbers
 * $zero = $0 , $t0 = $8, parser needs to handle either representation
 * scan up until $, and read whatever is after that. If illegal register detected error reported
 */
char *regNumberConverter(char *input)
{
    return NULL;
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
/*struct inst parser()
{
    return;
}*/



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
    
    if(curr.opcode == 0)//add
    {
        mem_latch.t1 = curr.rs + curr.rt;
        
    }
    
    if(curr.opcode == 1)//addi
    {
        mem_latch.t1 = curr.rs + curr.Imm;
    }
    
    if(curr.opcode == 2)//sub
    {
        mem_latch.t1 = curr.rs - curr.rt;
    }
    
    if(curr.opcode == 3)//mult
    {
        mem_latch.t1 = curr.rs*curr.rt;
    }
    
    if(curr.opcode == 4)//beq
    {
        if(curr.rs == curr.rt)
            programcount = programcount + 4 + (curr.Imm<<2);
        ex_latch.t2 = programcount;
    }
    
    if(curr.opcode == 5)//LW
    {
        ex_latch.t1 = curr.rs + curr.Imm;
    }
    
    if(curr.opcode == 6)//SW
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
    
    if(curr.opcode == LW)
    {
        mem_latch.t1 = dataMemory[ex_latch.t1];
    }
    else if(curr.opcode == SW)
    {
        dataMemory[ex_latch.t1] = curr.rt;
    }
    
}

void WB(long *mips_register[])
{
    struct inst curr;
    
    curr = mem_latch.next;
    
    if ((curr.opcode != SW) && (curr.opcode != BEQ))
    {
        mips_register[curr.rd] = &mem_latch.t1;
    }
    
    
}
