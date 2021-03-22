#include "cli.h"

#include "stdio.h"
#include "string.h"


void printHelp(){
	printf("Bzo Compiler\nCreated by Charles Rosenbauer\n\n");

	printf("This compiler is a work-in-progress, and in heavy development.\n");
	
	printf("Commands:\n");
	
	printf("-h           : Show help page. You are here.\n");
}


void checkCmd(int argc, char** args){
	if(argc == 2){
		if(!strcmp(args[1], "-h")){
			printHelp();
		}else{
			printf("Unknown command: %s\n", args[1]);
		}
	}else{
		printf("Unknown command:");
		for(int i = 1; i < argc; i++) printf("%s ", args[i]);
		printf("\n");
	}
}
