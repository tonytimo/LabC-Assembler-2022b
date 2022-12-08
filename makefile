$(shell mkdir -p ObjectFiles)
$(shell mkdir -p Final_Project)


Final_Project/FINAL_PROJECT: ObjectFiles/assembler.o ObjectFiles/converters.o ObjectFiles/errorHandler.o ObjectFiles/secondRun.o ObjectFiles/cmdTable.o ObjectFiles/dataTable.o ObjectFiles/firstRun.o ObjectFiles/saveFiles.o ObjectFiles/symbolTable.o ObjectFiles/main.o ObjectFiles/preprocces.o
	gcc  -o Final_Project/"FINAL_PROJECT"  ./ObjectFiles/assembler.o ./ObjectFiles/cmdTable.o ./ObjectFiles/converters.o ./ObjectFiles/dataTable.o ./ObjectFiles/errorHandler.o ./ObjectFiles/firstRun.o ./ObjectFiles/main.o ./ObjectFiles/saveFiles.o ./ObjectFiles/secondRun.o ./ObjectFiles/symbolTable.o ObjectFiles/preprocces.o  -lm

ObjectFiles/assembler.o: src/assembler.c src/errorHandler.h src/assembler.h
	gcc -ansi -pedantic -Wall -c src/assembler.c -o ObjectFiles/assembler.o

ObjectFiles/converters.o: src/converters.c src/converters.h src/assembler.h
	gcc -ansi -pedantic -Wall -c src/converters.c -o ObjectFiles/converters.o -lm

ObjectFiles/errorHandler.o: src/errorHandler.c src/errorHandler.h src/constants.h
	gcc -ansi -pedantic -Wall -c src/errorHandler.c -o ObjectFiles/errorHandler.o

ObjectFiles/secondRun.o: src/secondRun.c src/assembler.h src/dataTable.h src/cmdTable.h src/symbolTable.h src/errorHandler.h
	gcc -ansi -pedantic -Wall -c src/secondRun.c -lm -o ObjectFiles/secondRun.o

ObjectFiles/cmdTable.o: src/cmdTable.c src/converters.h src/cmdTable.h src/dataTable.h
	gcc -ansi -pedantic -Wall -c src/cmdTable.c -o ObjectFiles/cmdTable.o

ObjectFiles/dataTable.o: src/dataTable.c src/dataTable.h src/constants.h src/converters.h src/converters.h
	gcc -ansi -pedantic -Wall -c src/dataTable.c -lm -o ObjectFiles/dataTable.o

ObjectFiles/firstRun.o: src/firstRun.c src/assembler.h src/dataTable.h src/cmdTable.h src/symbolTable.h
	gcc -ansi -pedantic -Wall -c src/firstRun.c -o ObjectFiles/firstRun.o

ObjectFiles/saveFiles.o: src/saveFiles.c src/cmdTable.h src/dataTable.h src/assembler.h src/converters.h src/symbolStructs.h
	gcc -ansi -pedantic -Wall -c src/saveFiles.c -o ObjectFiles/saveFiles.o

ObjectFiles/symbolTable.o: src/symbolTable.c src/assembler.h src/symbolTable.h src/symbolStructs.h src/converters.h src/constants.h
	gcc -ansi -pedantic -Wall -c src/symbolTable.c -o ObjectFiles/symbolTable.o

ObjectFiles/preprocces.o: src/preprocces.c src/preprocces.h src/assembler.h src/constants.h src/structs.h src/errorHandler.h
	gcc -ansi -pedantic -Wall -c src/preprocces.c -o ObjectFiles/preprocces.o


ObjectFiles/main.o: src/main.c src/assembler.h src/symbolTable.h src/cmdTable.h src/dataTable.h src/saveFiles.h src/converters.h src/preprocces.h
	gcc -ansi -pedantic -Wall -c src/main.c -o ObjectFiles/main.o

clean:
	rm -r bin
	rm FINAL_PROJECT
