
#include "../includes/turtle.h"

int main(int argc, char *argv[])

{

    checkInput(argc, argv);


}


void checkInput(int argc, char *argv[])
{
    if(argc == 1 || argc > 3) {
        printCommandLineError();
    }
    if(argc == 3) {
        if(strcmp(argv[1],"test") != 0) {
            printCommandLineError();
        }
        if(strcmp(argv[2],"all") != 0 && strcmp(argv[2],"white") != 0 && strcmp(argv[2],"black") != 0) {
            printCommandLineError();
        }
    }
}


void printCommandLineError()
{
    fprintf(stderr,"please run the turtle program with one of the command line arguments as follows:\n\nTo parse a .txt file and draw a shape:\n./turtle <FILENAME>.txt\n\nFor testing enter one of the below:\n./turtle test all\n./turtle test white\n./turtle test black\n");
}

void runTurtleTests()
{  
	  sput_start_testing();
	  sput_set_output_stream(NULL);	

	  sput_finish_testing();
}
