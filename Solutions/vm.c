/**

 * Juliana Oyola-Pabon;
 * Travis Wise
 * 
 * Filename:    vm.c
 * To Compile:  gcc <filename.c>
 * To Run:      <./a.out> <inputfile.txt>
 */

// Preprocessor Directives
#include <stdio.h>
#include <stdlib.h>

// Constants
#define MAX_PAS_LENGTH 500

// Instruction Struct
typedef struct instruction
{
    int OP;
    int L;
    int M;
} instruction;

// Function Declarations
void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int DP, int *pas, int GP);
int base(int);

// Globals
instruction IR = {0, 0, 0};
//PAS - rep Process Memory
int pas[MAX_PAS_LENGTH];
// Set all to 0
int GP = 0, IC = 0, DP = 0, FREE = 0, BP = 0, PC = 0, SP = 0;

// Main function
int main(int argc, char *argv[])
{
    // Read input file from command line and read file.
    FILE *inputF = fopen(argv[1], "r");
    if (inputF == NULL)
        return 0;

    // Read in file until end of file.
    while (!feof(inputF))
    {
        // Scan in for OP, L, M
        fscanf(inputF, "%d%d%d", &pas[IC], &pas[IC + 1], &pas[IC + 2]);
        IC += 3;
    }
    fclose(inputF);

    // Setup registers based on location of instruction counter
    GP = IC;
    DP = IC - 1;
    FREE = IC + 40;
    BP = IC;
    PC = 0;
    SP = MAX_PAS_LENGTH;
    int line = 0;
    // Print Column Headers to Console
    printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
    printf("%s\t\t\t%d\t%d\t%d\t%d\n", "Initial values:", PC, BP, SP, DP);


    while (1)
    {
        // Entering Fetch Cycle
        // Place instruction at location 0 to 2 in IR
        IR.OP = pas[PC];
        IR.L = pas[PC + 1];
        IR.M = pas[PC + 2];
        line = PC / 3;
        PC += 3;

        switch (IR.OP)
        {
        // LIT Pushes a constant OP.M onto the stack
        case 1:
            if (BP == GP)
            {
                DP = DP + 1;
                pas[DP] = IR.M;
            }
            else
            {
                SP = SP - 1;
                pas[SP] = IR.M;
            }
            // Print Initial Values for each Register to Console
            print_execution(line, "LIT", IR, PC, BP, SP, DP, pas, GP);
            break;

        // OPR Operation to be performed on the data at top of stack.
        case 2:
            switch (IR.M)
            {
            // RTN
            case 0:
                SP = BP + 1;
                BP = pas[SP - 2];
                PC = pas[SP - 3];

                print_execution(line, "RTN", IR, PC, BP, SP, DP, pas, GP);
                break;
            // NEG
            case 1:
                if (BP == GP)
                {
                    pas[DP] = -1 * pas[DP];
                }
                else
                {
                    pas[SP] = -1 * pas[SP];
                }
                print_execution(line, "NEG", IR, PC, BP, SP, DP, pas, GP);
                break;
            // ADD
            case 2:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] + pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] + pas[SP - 1];
                }
                print_execution(line, "ADD", IR, PC, BP, SP, DP, pas, GP);
                break;
                // SUB
            case 3:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] - pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] - pas[SP - 1];
                }
                print_execution(line, "SUB", IR, PC, BP, SP, DP, pas, GP);
                break;
            // MUL
            case 4:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] * pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] * pas[SP - 1];
                }
                print_execution(line, "MUL", IR, PC, BP, SP, DP, pas, GP);
                break;
            // DIV
            case 5:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] / pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] / pas[SP - 1];
                }
                print_execution(line, "DIV", IR, PC, BP, SP, DP, pas, GP);
                break;
            // ODD
            case 6:
                if (BP == GP)
                {
                    pas[DP] = pas[DP] % 2;
                }
                else
                {
                    pas[SP] = pas[SP] % 2;
                }
                print_execution(line, "ODD", IR, PC, BP, SP, DP, pas, GP);
                break;
            // MOD
            case 7:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] % pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] % pas[SP - 1];
                }
                print_execution(line, "MOD", IR, PC, BP, SP, DP, pas, GP);
                break;
            // EQL
            case 8:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] == pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] == pas[SP - 1];
                }
                print_execution(line, "EQL", IR, PC, BP, SP, DP, pas, GP);
                break;
            // NEQ
            case 9:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] != pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] != pas[SP - 1];
                }
                print_execution(line, "NEQ", IR, PC, BP, SP, DP, pas, GP);
                break;
            // LSS
            case 10:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] < pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] < pas[SP - 1];
                }
                print_execution(line, "LSS", IR, PC, BP, SP, DP, pas, GP);
                break;
            // LEQ
            case 11:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] <= pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] <= pas[SP - 1];
                }
                print_execution(line, "LEQ", IR, PC, BP, SP, DP, pas, GP);
                break;
            // GTR
            case 12:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] > pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] > pas[SP - 1];
                }
                print_execution(line, "GTR", IR, PC, BP, SP, DP, pas, GP);
                break;
            // GEQ
            case 13:
                if (BP == GP)
                {
                    DP = DP - 1;
                    pas[DP] = pas[DP] >= pas[DP + 1];
                }
                else
                {
                    SP = SP + 1;
                    pas[SP] = pas[SP] >= pas[SP - 1];
                }
                print_execution(line, "GEQ", IR, PC, BP, SP, DP, pas, GP);
                break;
            default:
                return 0;
            }
            break;
        // LOD Load value to top of stack from the stack location offset OP.M from OP.L levels down
        case 3:
            if (BP == GP)
            {
                DP = DP + 1;
                pas[DP] = pas[GP + IR.M];
            }
            else {
                if (base(IR.L) == GP)
                {
                    SP = SP - 1;
                    pas[SP] = pas[GP + IR.M];
                }
                else {
                    SP = SP - 1;
                    pas[SP] = pas[base(IR.L) - IR.M];
                }
            }
            print_execution(line, "LOD", IR, PC, BP, SP, DP, pas, GP);
            break;

        // STO Store value at top of stack location at offset OP.M from OP.L levels down.
        case 4:
            if (BP == GP)
            {
                pas[GP + IR.M] = pas[DP];
                DP = DP - 1;
            }
            else
            {
              if(base(IR.L) == GP)
              {
                pas[GP + IR.M] = pas[SP];
                SP = SP + 1;
              }
              else{
                pas[base(IR.L) - IR.M] = pas[SP];
                SP = SP + 1;
              }
            }
            print_execution(line, "STO", IR, PC, BP, SP, DP, pas, GP);
            break;

        // CAL Call procedure at code index OP.M (gen new AR and PC = M)
        case 5:
            pas[SP - 1] = base(IR.L); // Static link
            pas[SP - 2] = BP;         // Dynamic link
            pas[SP - 3] = PC;         // return adress
            BP = SP - 1;
            PC = IR.M;
            print_execution(line, "CAL", IR, PC, BP, SP, DP, pas, GP);
            break;

        // INC Allocate OP.M memory words.
        case 6:
            if (BP == GP)
            {
                DP = DP + IR.M;
                print_execution(line, "INC", IR, PC, BP, SP, DP, pas, GP);
            }
            else
            {
                SP = SP - IR.M;
                print_execution(line, "INC", IR, PC, BP, SP, DP, pas, GP);
            }
            break;

        // JMP Jump to instruction OP.M (M = PC)
        case 7:
            PC = IR.M;
            print_execution(line, "JMP", IR, PC, BP, SP, DP, pas, GP);
            break;

        // JPC Jump to instruct OP.M if top stack element is 0.
        case 8:
            if (BP == GP) {
                if (pas[DP] == 0) 
                {
                    PC = IR.M;
                }
                DP = DP - 1;
            }
            else 
            {
                if (pas[SP] == 0) 
                {
                    PC = IR.M;
                }
                SP = SP + 1;
            }
            print_execution(line, "JPC", IR, PC, BP, SP, DP, pas, GP);
            break;

        // SYS Write the top stack elelemt or data element to the screen.
        case 9:
            if (IR.M == 1)
            {
                if (BP == GP)
                {
                    printf("Top of Stack Value: %d\n", pas[DP]);
                    DP = DP - 1;
                }
                else
                {
                    printf("Top of Stack Value: %d\n", pas[SP]);
                    SP = SP + 1;
                }
                print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
            }
            else if (IR.M == 2)
            {
                if (BP == GP)
                {
                    DP = DP + 1;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &pas[DP]);
                    print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
                }
                else
                {
                    SP = SP - 1;
                    printf("Please Enter an Integer: ");
                    scanf("%d", &pas[SP]);
                    print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
                }
            }
            else if (IR.M == 3)
            {
                print_execution(line, "SYS", IR, PC, BP, SP, DP, pas, GP);
                // Set Halt flag to zero. (End of program)
                return 0;
            }

            break;

        default:
            printf("Input not supported\n");
            return 0;
            break;
        }
    }

    return 0;
} // End of main

// Given base function from the PDF
int base(int L)
{
    int arb = BP; // arb = activation record base
    //find base L levels down
    while (L > 0)
    {
        arb = pas[arb];
        L--;
    }

    return arb;
} // End of base

// Given print function from the PDF
void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
    int i;
    // Print out instruction and registers
    printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR.L, IR.M, PC, BP, SP, DP);

    // Print data section
    for (i = GP; i <= DP; i++)
        printf("%d ", pas[i]);
    printf("\n");

    // Print stack
    printf("\tstack : ");

    for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
        printf("%d ", pas[i]);
    printf("\n");
} // End of print_execution
