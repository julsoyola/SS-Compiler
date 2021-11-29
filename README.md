# SS-Compiler
Creating a Compiler
The compiler driver supports the following compiler directives:
-l : print the list and table of lexemes/tokens (Lex) to the screen
-s : print the symbol table
-a : print the generated assembly code (parser/codegen output) to the screen
-v : print virtual machine execution trace (VM) to the screen
<filename>.txt : input file name, for e.g. input.txt
Example commands:
./a.out input.txt –l –a –v Print all types of output to the console
./a.out input.txt –v Print only the VM execution trace to the console
./a.out input.txt Print  nothing  to  the  console  except  for  program  read 
and write instructions.
