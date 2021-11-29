#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define MAX_IDENT_LEN 11

instruction *code;
// code index
int cIndex = 0;
symbol *table;
// token index
int tIndex = 0;
// Added variables
lexeme *list;
lexeme token;
int listIndex = 0;
int lexLevel;
int level = -1;
int symidx;
int numVars;
char symbols[][MAX_IDENT_LEN] = {"NULL", "const", "var", "procedure", "begin", "end", 
                                "while", "do", "if", "then", "else", 
                                "call", "write", "read", "identsym", "", 
                                ":=", "+", "-", "*", "/", 
                                "%", "==", "!=", "<", "<=",
                                ">", ">=", "odd", "(", ")", 
                                ",", ".", ";"};



void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// Fxn prototypes
int findSymbol(char name[12], int value, int value2);
void mark();

void program();
void block();
lexeme getToken();
void constDeclaration();
int varDeclaration();
void procedureDeclaration();


int multDeclaration();




instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = NULL;
	
	// v important line of code
	code[cIndex].opcode = -1;

	program(list);
	
	return code;
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}
// // Function that accepts current token
// void accept()
// {
// 	mark();
// 	tIndex++;
// }

// // Function that returns current token
// int nextToken()
// {
// 	return list[tIndex].type;
// }
// // Function that marks symbols in table based on current lexigraphical level

// does a linear search for the given name. An entry only matches if it has the correct name AND kind value AND is unmarked. Then it tries to maximize the level value
int findSymbol(char name[12], int value, int value2)
{
	for(int i = listIndex; i > 0; i--)
		if(strcmp(table[i].name, name) == 0 && table[i].mark == 0 && (table[i].kind == value || table[i].kind == value2)) return i;
	return -1;
}
void mark()
{
	int temp_level = lexLevel;
	for(int i = listIndex - 1; i >= 0; i--)
	{
		if(temp_level > table[i].level) temp_level = table[i].level;
		if(temp_level < table[i].level) table[i].mark = 1;
		else table[i].mark = 0;
	}
}

// 
void program()
{
	// emit JMP
	emit(7, 0, 0);

	// add to symbol table
	addToSymbolTable(3, "main", 0, 0, 0, 0);

	// intialize level
  	level = -1;

	// call block
	block();

	// for '.' if its not '.' print error
	if (strcmp(symbols[list[listIndex].value], ".") != 0) 
    	printparseerror(1);
	
	// emit halt
	emit(9, 0, 3);

	// for each line
	for (int i = 0; i < cIndex; i++)
	{
		// OPR 5 - / 
		if (strcmp(symbols[list[listIndex].value], "/") == 0)
		{
			code[i].m = table[code[i].m].addr;
		}
	}
	code[0].m = table[0].addr;
}
void block()
{
	// increment
	level++;

	// assign procedure to the index at -1
	int procedure_idx;
	// procedure_idx = current symbol table index - 1
	procedure_idx = tIndex - 1;

	constDeclaration();

	int x;
	x = varDeclaration();

	procedureDeclaration();
	table[procedure_idx].addr = cIndex * 3;

	if (level == 0)
	{
		// emit for INC, M = x 
		emit(6, 0, x);
	}
	else
	{
		// emit for INC, M = x + 3
		emit(6, 0, x + 3);
	}
	// call statement and mark
	STATEMENT();
	MARK();

	// decrement level
	--level;
}
lexeme getToken(lexeme * list)
{
  // get the next token in the list
   return list[listIndex++];
}
void constDeclaration()
{
	char name[12];

	// if token == const
	if (strcmp(symbols[list[listIndex].value], "const") == 0)
 	{
		do
		{
			// gets the next token
			token = list[listIndex++];

			// if token != identsym
			if (strcmp(symbols[list[listIndex].value], "identsym") != 0)
			{
				// return an error
				printparseerror(2);
			}
			symidx = multDeclaration(token);

			// save name
			strcpy(name, symidx);
			// get next token
			listIndex++;
			// check if its the token
			if (token.type != assignsym)
			{
				// check the error
				printparseerror(2);
			}
			// get next token
			listIndex++;
			if (token.type != numbersym)
			{
				// check the error
				printparseerror(11);
			}

			// add to symbol table (kind 1, saved name, number, level, 0, unmarked
			addToSymbolTable(1, name, symidx, level, 0, !mark);
			// get next token
			listIndex++;

		}while(token.type == commasym);

		if (token.type != semicolonsym)
		{
			if (token.type == identsym)
			{
				// check the error
				printparseerror(14);
			}
			else{
				// check the error
				printparseerror(14);
			}
		}
		// get next token
		listIndex++;
	}
}
int varDeclaration()
{
	char name[12];
	numVars = 0;
	if (token.type == varsym)
	{
		do
		{
			numVars++;
			listIndex++;

			if (token.type != identsym)
			{
				printparseerror(3);
			}
			symidx = multDeclaration(token);

			if (symidx != - 1)
			{
				printparseerror(3);
			}
			if (level == 0)
			{
				// add to symbol table (kind 1, saved name, number, level, 0, unmarked
			addToSymbolTable(2, list[tIndex].name, 0, numVars-1, 0, !mark);
			}
			else{
				addToSymbolTable(2, list[tIndex].name, 0, numVars+2, 0, !mark);
			}
			listIndex++;

		}while(token.type == commasym);

		if (token.type != semicolonsym)
		{
			if (token.type == identsym)
			{
				// check the error
				printparseerror(14);
			}
			else{
				// check the error
				printparseerror(14);
			}
		}
		listIndex++;
	}
	return numVars;
}
void procedureDeclaration()
{
	while (token.type == procsym)
	{
		// gets next token
		listIndex++;
		if (token.type != identsym)
		{
			printparseerror(3);
		}
			symidx = multDeclaration(token);

		if (symidx != - 1)
		{
			printparseerror(3);
		}
		addToSymbolTable(3, list[tIndex].name, 0, level, 0, !mark);
		// gets next token
		listIndex++;
		if (token.type != semicolonsym)
		{
			printparseerror(4);
		}
		// gets next token
		listIndex++;
		block();

		if (token.type != semicolonsym)
		{
			printparseerror(4);
		}
		// gets next token
		listIndex++;

		// RTN COMMAND
		emit(2, 0, 0);

	}
}
// Non Terminal grammar function
void statement()
{

}

int multDeclaration()
{

}


void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}


void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}