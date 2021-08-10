/*------------------------------------------------------*/
/*                                                      */
/*                       Main                           */
/*                                                      */
/*--------------------------------------------------SV--*/

#include <inc/config.h>
#include <inc/general.h>
#include <inc/tests.h>
#include <inc/utilities.h>

int main(int argc, char *argv[]){
	int full_check = 0, default_make = 0, opt;
	char student_number[COMMAND_LINE_BUFFER];

	if(argc < 2){
		HANDLE_ERROR("Usage: ./evaluate -[fgm] -n student_num", 0);
	}

	/* get arguments */
	optind = 1;

	while((opt = getopt(argc, argv, "fn:gm")) != -1){
		
		switch(opt){

			/* Student number */
			case 'n':
				strncpy(student_number, optarg, sizeof(student_number));
				break;

			/* Type of check (-full) */
			case 'f':
				full_check = 1;
				break;

			case 'g':
				generate_solution_output();
				exit(EXIT_SUCCESS);

			case 'm':
				default_make = 1;
				break;

			case '?':
				HANDLE_ERROR("Unknown argument.", 0);
		}
	}
	/* Fitech numbers are 9 digits but 6 without leading zeros */
	if(strlen(student_number) != 6){
		HANDLE_ERROR("Malformed student number.", 0);
	}

	StudentFiles stf = init(student_number, full_check, default_make);

	if(!check_student_information(student_number, stf)){
		printf(GRN("Student information passed.\n"));
	}

	if(!check_globals(stf)){
		printf(GRN("Global check passed.\n"));
	}

	if(!check_memory_leaks(stf)){
		printf(GRN("Memory-leak check passed.\n"));
	}
  
	if(!check_file_exceptions(stf)){
		printf(GRN("File exceptions check passed.\n"));
	}

	if(!check_malloc_exceptions(stf)){
		printf(GRN("Malloc exceptions check passed.\n"));
	}

	if(!check_output(full_check)){
		printf(GRN("Output checks passed.\n"));
	}

	printf("\n");
	clean(stf);

	return EXIT_SUCCESS;
}
