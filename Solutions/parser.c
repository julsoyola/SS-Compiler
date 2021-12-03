#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

/**

 * Juliana Oyola-Pabon;
 * 
 * Filename:    parser.c
 * To Compile:  gcc <filename.c>
 * To Run:      <./a.out> <inputfile.txt>
 */

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100

// Global variables
instruction *code;
symbol *table;
int cIndex;
int tIndex;
int token;
int level;

// Given function declarations
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// Utility function declarations
void program(lexeme *list);
void block(lexeme *list);
void constDeclaration(lexeme *list);
int varDeclaration(lexeme *list);
void procedureDeclaration(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
int multipleDeclarationCheck(lexeme *list);
int findSymbol(lexeme *list, int kind);
void mark();
void getNextToken();

// Function that parses the tokens.
instruction *parse(lexeme *list, int printTable, int printCode)
{
	// Initialize gloabal varibles to 0
	level = 0, token = 0, cIndex = 0, tIndex = 0;

	// Allocate memory for pointers instruction code and symbol table.
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);

	// Call program
	program(list);

	// Print symbol table as long as printTable is true.
	if (printTable)
		printsymboltable();

	// Print assembly code as long as printCode is true.
	if (printCode)
		printassemblycode();

	// marks the end of the code
	code[cIndex].opcode = -1;

	return code;
}
void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
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
void program(lexeme *list)
{
	int line;

	// call JMP
	emit(7, 0, 0);

	addToSymbolTable(3, "main", 0, 0, 0, 0);
	level = -1;

	// Call block function
	block(list);

	if (list[token].type != periodsym)
	{
		// print error
		printparseerror(1);
		exit(0);
	}
	// call Halt.
	emit(9, 0, 3);

	for (line = 0; line < cIndex; line++)
	{
		if (code[line].opcode == 5)
		{
			code[line].m = table[code[line].m].addr;
		}
	}
	code[0].m = table[0].addr;
}
void block(lexeme *list)
{
	// Increment level.
	level++;
	int procedure_idx = tIndex - 1;

	// Call the constDeclaration function.
	constDeclaration(list);
	int x = varDeclaration(list);

	// Call procedure Declaration.
	procedureDeclaration(list);
	table[procedure_idx].addr = cIndex * 3;
	// call INC
	if (level == 0)
		emit(6, 0, x);
	// call INC
	else
		emit(6, 0, x + 3);

	// Call statement function.
	statement(list);

	// Call mark function.
	mark();

	// Decrement level.
	level--;
}
void constDeclaration(lexeme *list)
{
	// Check if the type is equal to constsym
	if (list[token].type == constsym)
	{
		do
		{
			getNextToken();
			if (list[token].type != identsym)
			{
				// print parse error
				printparseerror(2);
				exit(0);
			}

			// Store return value of multipleDecCheck in variable.
			int symidx = multipleDeclarationCheck(list);

			// when symIdx is -1 or 'emptt' print error
			if (symidx != -1)
			{
				// print parse error
				printparseerror(18);
				exit(0);
			}
			// Save indent name.
			char saveName[12];
			strcpy(saveName, list[token].name);

			getNextToken();

			if (list[token].type != assignsym)
			{
				// print error
				printparseerror(5);
				exit(0);
			}

			getNextToken();

			if (list[token].type != numbersym)
			{
				// print error
				printparseerror(2);
				exit(0);
			}

			addToSymbolTable(1, saveName, list[token].value, level, 0, 0);
			getNextToken();
		} while (list[token].type == commasym);

		if (list[token].type != semicolonsym)
		{
			if (list[token].type != identsym)
			{
				// print error
				printparseerror(14);
				exit(0);
			}
			else
			{
				printparseerror(13);
				exit(0);
			}
		}
		getNextToken();
	}
}
int varDeclaration(lexeme *list)
{
	int numVars = 0;
	if (list[token].type == varsym)
	{
		do
		{
			// move numVars up
			numVars++;
			getNextToken();

			if (list[token].type != identsym)
			{
				// error
				printparseerror(3);
				exit(0);
			}
			// Store return value of multipleDecCheck in variable.
			int symidx = multipleDeclarationCheck(list);

			// check if its empty
			if (symidx != -1)
			{
				// if so print error
				printparseerror(18);
				exit(0);
			}

			if (level == 0)
			{
				addToSymbolTable(2, list[token].name, 0, level, numVars - 1, 0);
			}
			else
			{
				addToSymbolTable(2, list[token].name, 0, level, numVars + 2, 0);
			}
			getNextToken();
		} while (list[token].type == commasym);

		if (list[token].type != semicolonsym)
		{
			if (list[token].type != identsym)
			{
				// error
				printparseerror(14);
				exit(0);
			}
			else
			{
				printparseerror(13);
				exit(0);
			}
		}
		getNextToken();
	}
	return numVars;
}
void procedureDeclaration(lexeme *list)
{
	while (list[token].type == procsym)
	{
		getNextToken();

		if (list[token].type != identsym)
		{
			// error
			printparseerror(4);
			exit(0);
		}

		// Store return value of multipleDecCheck in variable.
		int symidx = multipleDeclarationCheck(list);

		if (symidx != -1)
		{
			// error
			printparseerror(18);
			exit(0);
		}

		addToSymbolTable(3, list[token].name, 0, level, 0, 0);

		getNextToken();

		if (list[token].type != semicolonsym)
		{
			// print error
			printparseerror(4);
			exit(0);
		}
		getNextToken();

		// Call block function.
		block(list);

		// Error handling prints error message and exits the program
		if (list[token].type != semicolonsym)
		{
			// error if its not ';'
			printparseerror(17);
			exit(0);
		}
		getNextToken();
		// call RTN
		emit(2, 0, 0);
	}
}

void statement(lexeme *list)
{
	if (list[token].type == identsym)
	{
		// Store findSymbol in symIdx
		int symIdx = findSymbol(list, 2);

		// check if its empty
		if (symIdx == -1)
		{
			// find the symbols
			if (findSymbol(list, 1) != findSymbol(list, 3))
			{
				printparseerror(6);
				exit(0);
			}
			else
			{
				printparseerror(17);
				exit(0);
			}
		}

		getNextToken();
		if (list[token].type != assignsym)
		{
			// print error
			printparseerror(5);
			exit(0);
		}
		getNextToken();
		// Call expression function.
		expression(list);

		emit(4, level - table[symIdx].level, table[symIdx].addr);
		return;
	}

	if (list[token].type == beginsym)
	{
		do
		{
			getNextToken();
			// Call statement
			statement(list);
		} while (list[token].type == semicolonsym);
		// check whether these types are identified if not print error
		if (list[token].type != endsym)
		{
			if (list[token].type == identsym || list[token].type == beginsym || list[token].type == ifsym || list[token].type == whilesym || list[token].type == readsym || list[token].type == writesym || list[token].type == callsym)
			{
				printparseerror(15);
				exit(0);
			}
			else
			{
				printparseerror(16);
				exit(0);
			}
		}
		getNextToken();
		return;
	}
	if (list[token].type == ifsym)
	{
		getNextToken();

		// Call condition
		condition(list);
		int jpcIdx = cIndex;

		// call JPC with jpcIdx as mvalue paramater.
		emit(8, 0, jpcIdx);

		if (list[token].type != thensym)
		{
			// print error
			printparseerror(8);
			exit(0);
		}
		getNextToken();

		statement(list);

		if (list[token].type == elsesym)
		{
			getNextToken(); 
			int jmpIdx = cIndex;

			// call JMP with jpcIdx
			emit(7, 0, jmpIdx);
			code[jpcIdx].m = cIndex * 3;

			// Call statement recursivley
			statement(list);

			code[jmpIdx].m = cIndex * 3;
		}
		else
		{
			code[jpcIdx].m = cIndex * 3;
		}
		// now just return
		return;
	}
	if (list[token].type == whilesym)
	{
		getNextToken();
		int loopIdx = cIndex;

		// Call condition
		condition(list);

		// Error handling prints error message and exits the program
		if (list[token].type != dosym)
		{
			printparseerror(9);
			exit(0);
		}
		getNextToken();
		int jpcIdx = cIndex;

		// call JPC
		emit(8, 0, 0);

		// Call statement recursively.
		statement(list);

		// call JMP with m value with loopIdx multiplied by 3.
		emit(7, 0, loopIdx * 3);
		code[jpcIdx].m = cIndex * 3;
		return;
	}
	if (list[token].type == readsym)
	{
		getNextToken();

		if (list[token].type != identsym)
		{
			printparseerror(6);
			exit(0);
		}

		int symIdx = findSymbol(list, 2);

		if (symIdx == -1)
		{
			if (findSymbol(list, 1) != findSymbol(list, 3))
			{
				// print error
				printparseerror(17);
				exit(0);
			}
			else
			{
				printparseerror(19);
				exit(0);
			}
		}
		getNextToken();

		// call READ
		emit(9, 0, 2);
		emit(4, level - table[symIdx].level, table[symIdx].addr);
		return;
	}
	if (list[token].type == writesym)
	{
		getNextToken();
		// Call expression
		expression(list);
		// call WRITE
		emit(9, 0, 1);
		return;
	}
	if (list[token].type == callsym)
	{
		getNextToken();
		// Storevalue of findSymbol 
		int symIdx = findSymbol(list, 3);
		if (symIdx == -1)
		{
			if (findSymbol(list, 1) != findSymbol(list, 2))
			{
				// print error
				printparseerror(7);
				exit(0);
			}
			else
			{
				printparseerror(19);
				exit(0);
			}
		}
		// get the next token
		getNextToken();
		// call CAL
		emit(5, level - table[symIdx].level, symIdx);
	}
}
void condition(lexeme *list)
{
	if (list[token].type == oddsym)
	{
		getNextToken();
		// Call expression
		expression(list);
		// call ODD
		emit(2, 0, 6);
	}
	else
	{
		// Call expression
		expression(list);
		if (list[token].type == eqlsym)
		{
			getNextToken();
			// Call expression 
			expression(list);
			// call EQL
			emit(2, 0, 8);
		}
		else if (list[token].type == neqsym)
		{
			getNextToken();
			// Call expression
			expression(list);
			// call NEQ
			emit(2, 0, 9);
		}
		else if (list[token].type == lsssym)
		{
			getNextToken();
			// Call expression
			expression(list);
			// call LSS
			emit(2, 0, 10);
		}
		else if (list[token].type == leqsym)
		{
			getNextToken();
			// Call expression
			expression(list);
			// call LEQ
			emit(2, 0, 11);
		}
		else if (list[token].type == gtrsym)
		{
			getNextToken();
			// Call expression
			expression(list);
			// call GTR
			emit(2, 0, 12);
		}
		else if (list[token].type == geqsym)
		{
			getNextToken();
			// Call expression
			expression(list);
			// call GEQ
			emit(2, 0, 13);
		}
		else
		{
			// print error
			printparseerror(10);
			exit(0);
		}
	}
}
void expression(lexeme *list)
{
	if (list[token].type == subsym)
	{
		getNextToken();
		// Call term
		term(list);
		// call NEG
		emit(2, 0, 1);
		while (list[token].type == addsym || list[token].type == subsym)
		{
			if (list[token].type == addsym)
			{
				getNextToken();
				// Call term 
				term(list);
				// call ADD
				emit(2, 0, 2);
			}
			else
			{
				getNextToken();
				// Call term
				term(list);
				// call SUB
				emit(2, 0, 3);
			}
		}
	}
	else
	{
		if (list[token].type == addsym)
			getNextToken();
		// Call term
		term(list);
		while (list[token].type == addsym || list[token].type == subsym)
		{
			if (list[token].type == addsym)
			{
				getNextToken();
				// Call term
				term(list);
				//call ADD
				emit(2, 0, 2);
			}
			else
			{
				getNextToken();
				// Call term
				term(list);
				// call SUB
				emit(2, 0, 3);
			}
		}
	}
	if (list[token].type == oddsym)
	{
		// print error
		printparseerror(17);
		exit(0);
	}
}
void term(lexeme *list)
{
	// Call factor 
	factor(list);
	while (list[token].type == multsym || list[token].type == divsym ||
		   list[token].type == modsym)
	{
		if (list[token].type == multsym)
		{
			getNextToken();
			// Call factor
			factor(list);
			//call MUL
			emit(2, 0, 4);
		}
		else if (list[token].type == divsym)
		{
			getNextToken();
			// Call factor
			factor(list);
			// call  DIV
			emit(2, 0, 5);
		}
		else
		{
			getNextToken();
			// Call factor
			factor(list);
			//call  MOD
			emit(2, 0, 7);
		}
	}
}
void factor(lexeme *list)
{
	if (list[token].type == identsym)
	{
		// Store findSymbol in variables
		int symIdx_var = findSymbol(list, 2);
		int symIdx_const = findSymbol(list, 1);
		if (symIdx_var == -1 && symIdx_const == -1)
		{
			if (findSymbol(list, 3) != -1)
			{
				// print error
				printparseerror(11);
				exit(0);
			}
			else
			{
				printparseerror(19);
				exit(0);
			}
		}
		if (symIdx_var == -1)
			emit(1, 0, table[symIdx_const].val);
		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr);
		else
			emit(1, 0, table[symIdx_const].val);
		// get next token
		getNextToken();
	}
	else if (list[token].type == numbersym)
	{
		emit(1, 0, list[token].value);
		getNextToken();
	}
	else if (list[token].type == lparensym)
	{
		// get next token
		getNextToken();
		// Call expression
		expression(list);
		if (list[token].type != rparensym)
		{
			printparseerror(12);
			exit(0);
		}
		getNextToken();
	}
	else
	{
		printparseerror(11);
		exit(0);
	}
}
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark)
		{
			continue;
		}
		if (table[i].level == level)
		{
			table[i].mark = 1;
		}
		if (!table[i].mark && table[i].level < level)
		{
			break;
		}
	}
}
int multipleDeclarationCheck(lexeme *list)
{
	int i;
	for (i = 0; i <= tIndex; i++)
		if (!strcmp(table[i].name, list[token].name) && table[i].level == level && !table[i].mark)
			return i;

	return -1;
}
// does a linear search for the given name. An entry only matches if it has the correct name AND kind value AND is unmarked. Then it tries to maximize the level value
int findSymbol(lexeme *list, int kind)
{
	int i;
	for (i = tIndex; i >= 0; i--)
		if (!strcmp(table[i].name, list[token].name) && table[i].kind == kind && !table[i].mark)
			return i;

	return -1;
}
void getNextToken()
{
	token++;
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
