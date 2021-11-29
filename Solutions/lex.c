/* 
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.
	
	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 
	5 points.
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index = 0, file_index = 0, file_length = 0;

void printlexerror(int type);
void printtokens();
void exitProgramAfterError(int value);
lexeme makeNumberLexeme(char * value);
lexeme makeLexeme(char * name);
lexeme makeSymbolLexeme(char * name);


lexeme *lexanalyzer(char *input)
{
	// Determine the number of characters to loop through
	file_length = strlen(input);

	// Allocate max space for the struct array
	list = (lexeme*)malloc(MAX_NUMBER_TOKENS * sizeof(lexeme));
	
  // Declare temp string to read in digits and identifers
  char temp_string[MAX_IDENT_LEN + 1];
  int temp_index = 0;

	// Loop through the input and assign the symbools to tokens
	// Throw an error is the symbols don't exist
	while (file_index <= file_length) 
	{
		// If the current character is a space or a control character just move to the next one.
		if (isspace(input[file_index]) || iscntrl(input[file_index])) 
		{
			file_index++;
			continue;
		}
    // Logic for a number
		/**
    * If a number is followed by an identifier with no whitespace, it is an invalid identifier error.
    * If a number is followed by a reserved word with no whitespace, it is an invalid identifier error.
    * If a number is followed by a number with no whitespace, it is a number.
    */
    else if (isdigit(input[file_index])) 
    {
      // Store the first character and increment our indicies.
      temp_string[temp_index++] = input[file_index++]; 

      // Get the next character as long as it's not a space or a control character.
      while ( !(isspace(input[file_index]) || iscntrl(input[file_index])) )  
      {
        // Read in the digit if the character is a digit else throw an error.
        if (isdigit(input[file_index])) 
        {
          // If we have a digit with more than 5 characters then print the error and exit
          if (temp_index == MAX_NUMBER_LEN) {
            exitProgramAfterError(3);
          }
          // Else just read in the number
          temp_string[temp_index++] = input[file_index++];
        }
        // We are only here if we are reading in a number and the next character is a letter.
        // This is an error so print it and exit.
        else if (isalpha(input[file_index]))
        {
          exitProgramAfterError(2);
        }

        // We are here if the current character is a symbol
        // We should go to the next iteration of the while loop and read it in.
        // But only after we make our lexeme for the valid number.
        if ( !(isspace(input[file_index]) || iscntrl(input[file_index]) || isdigit(input[file_index]) || isalpha(input[file_index])))
          break;
      }
      
      //
      // We are only here if the string is a valid number symbol 
      //
      
      // Create the lexeme for the number and go to the next lexeme.
      list[lex_index++] = makeNumberLexeme(temp_string);

      // Clear out the string and reset the index for the next iteration of the loop.
      temp_string[temp_index] = '\0';
      temp_index = 0;      
      strcpy(temp_string, "           ");
    } // End of number case

    // Logic for an identifier
		/**
		 * If an identifier is followed by a number with no whitespece, it is an identifer.
     
     * a2 q1328391swadasd2131
		 * If an identifer is followed by a reserved word with no whitespace, it is an identifer.
		 * If and identifer is followed by an identifer with no whitespce, it is an identifer.
		 */

		// Logic for a reserved word
		/**
		 * If a reserved word is followed by an identifer with no whitespace, it is an identifer.
		 * If a reserved word is followed by a number with no whitespace it is an identifer.
		 * If a reserved word is followed by a reserved word with no whitespace it is an identifer. 
		 */
     
    else if (isalpha(input[file_index]))
    { 
      // The first character in our new temp string is alphabetic
      // Store the first character and increment our indicies.
      temp_string[temp_index++] = input[file_index++]; 

      // Get the next character as long as it's not a space or a control character.
      while ( !(isspace(input[file_index]) || iscntrl(input[file_index])) ) 
      {
        if (isalpha(input[file_index]) || isdigit(input[file_index])) 
        {
          // If we have a digit with more than 11 characters then print the error and exit
          if (temp_index == MAX_IDENT_LEN) {
            exitProgramAfterError(4);
          }
          // read in
          temp_string[temp_index++] = input[file_index++];
        }
        else 
        {
          // We are here if the current character is a symbol
          // We should go to the next iteration of the while loop and read it in.
          // But only after we make our lexeme for the valid identifier.
          break;
        }
      }

      //
      // We are only here if the string is a valid identifer symbol 
      //

      // Add the null terminator for the string before we pass it in to the function.
      temp_string[temp_index] = '\0';

      // Create the lexeme for the identifer and go to the next lexeme.
      list[lex_index++] = makeLexeme(temp_string);

      // Clear out the string and reset the index for the next iteration of the loop.
      temp_index = 0;      
      strcpy(temp_string, "           ");
    } // End of identifier/keyword case
    // Logic for symbols
    else 
    { 
      if (input[file_index] == ':') 
      {
        file_index++;
        if (input[file_index] == '=') 
        {
          list[lex_index++] = makeSymbolLexeme(":=");
        }
        else 
        {
          exitProgramAfterError(1);
        }
      }
      else if (input[file_index] == '=') {
        if (input[file_index + 1] == '=') {
          list[lex_index++] = makeSymbolLexeme("==");
          file_index++;
        }
        else {
          exitProgramAfterError(1);
        }
      }
       else if (input[file_index] == '<') {
          if (input[file_index + 1] == '=') {
            list[lex_index++] = makeSymbolLexeme("<=");
            file_index++;
          }
          else {
            list[lex_index++] = makeSymbolLexeme("<");
          }
      }
       else if (input[file_index] == '>') {
        if (input[file_index + 1] == '=') {
            list[lex_index++] = makeSymbolLexeme(">="); 
            file_index++;
          }
          else {
            list[lex_index++] = makeSymbolLexeme(">");
          }
      }
      else if (input[file_index] == '!') {
        if (input[file_index + 1] == '=') {
            list[lex_index++] = makeSymbolLexeme("!=");
            file_index++;
          }
          else {
            list[lex_index++] = makeSymbolLexeme("!");
          }
      }
      else if (input[file_index] == '+') {
        list[lex_index++] = makeSymbolLexeme("+");
      }
      else if (input[file_index] == '-') {
        list[lex_index++] = makeSymbolLexeme("-");
      }
      else if (input[file_index] == '*') {
        list[lex_index++] = makeSymbolLexeme("*");
      }
      else if (input[file_index] == '/') {
        if (input[file_index + 1] == '/') {
          // We are only here if it's a comment
          // Read in the whole line until a new line character.
          file_index++;
          while(input[file_index] != '\n') {
            file_index++;
          }
        }
        else {
          list[lex_index++] = makeSymbolLexeme("/");
        }
      }
      else if (input[file_index] == '%') {
        list[lex_index++] = makeSymbolLexeme("%");
      }      
      else if (input[file_index] == '(') {
        list[lex_index++] = makeSymbolLexeme("(");
      }
      else if (input[file_index] == ')') {
        list[lex_index++] = makeSymbolLexeme(")");
      }
      else if (input[file_index] == ',') {
        list[lex_index++] = makeSymbolLexeme(",");
      }
      else if (input[file_index] == '.') {
        list[lex_index++] = makeSymbolLexeme(".");
      }
      else if (input[file_index] == ';') {
        list[lex_index++] = makeSymbolLexeme(";");
      }
      else 
      {
        exitProgramAfterError(1);
      }
      // Increment for the next loop
      file_index++;
    }
	} // End of symbol case
  
	printtokens();
	return list;
}

lexeme makeLexeme(char * name) 
{
  lexeme ret;
  if (strcmp(name, "const") == 0)
    ret.type = constsym;
  else if (strcmp(name, "var") == 0)
    ret.type = varsym;
  else if (strcmp(name, "procedure") == 0)
    ret.type = procsym;
  else if (strcmp(name, "begin") == 0)
    ret.type = beginsym;
  else if (strcmp(name, "end") == 0)
    ret.type = endsym;
  else if (strcmp(name, "while") == 0)
    ret.type = whilesym;
  else if (strcmp(name, "do") == 0)
    ret.type = dosym;
  else if (strcmp(name, "if") == 0)
    ret.type = ifsym;
  else if (strcmp(name, "then") == 0)
    ret.type = thensym;
  else if (strcmp(name, "else") == 0)
    ret.type = elsesym;
  else if (strcmp(name, "call") == 0)
    ret.type = callsym;
  else if (strcmp(name, "write") == 0)
    ret.type = writesym;
  else if (strcmp(name, "read") == 0)
    ret.type = readsym;
  else if (strcmp(name, "odd") == 0)
    ret.type = oddsym;
  else
  {
    strcpy(ret.name, name);
    ret.type = identsym;
  }
  return ret;
}

lexeme makeNumberLexeme(char * value) 
{
  lexeme ret;
  ret.value = atoi(value);
  ret.type = numbersym;
  return ret;
}

lexeme makeSymbolLexeme(char * name) 
{
  lexeme ret;
  if (strcmp(name, ":=") == 0) 
    ret.type = assignsym;
  else if (strcmp(name, "+") == 0)
     ret.type = addsym;
  else if (strcmp(name, "-") == 0)
     ret.type = subsym;
  else if (strcmp(name, "*") == 0) 
    ret.type = multsym;
  else if (strcmp(name, "/") == 0) 
    ret.type = divsym;
  else if (strcmp(name, "%") == 0) 
    ret.type = modsym;
  else if (strcmp(name, "==") == 0) 
    ret.type = eqlsym;
  else if (strcmp(name, "!=") == 0) 
    ret.type = neqsym;
  else if (strcmp(name, "<") == 0)
    ret.type = lsssym;
  else if (strcmp(name, "<=") == 0)
    ret.type = leqsym;
  else if (strcmp(name, ">") == 0)
    ret.type = gtrsym;
  else if (strcmp(name, ">=") == 0)
    ret.type = geqsym;
  else if (strcmp(name, "(") == 0)
    ret.type = lparensym;
  else if (strcmp(name, ")") == 0) 
    ret.type = rparensym;
  else if (strcmp(name, ",") == 0)
    ret.type = commasym;
  else if (strcmp(name, ".") == 0)
    ret.type = periodsym;
  else if (strcmp(name, ";") == 0)
    ret.type = semicolonsym;
  return ret;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}

void exitProgramAfterError(int value) 
{
  printlexerror(value);
  exit(0);
}