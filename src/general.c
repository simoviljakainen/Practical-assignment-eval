/*------------------------------------------------------*/
/*                                                      */
/*         This file is for general functions           */
/*                                                      */
/*--------------------------------------------------SV--*/

#include <inc/config.h>
#include <inc/utilities.h>

void clean(StudentFiles stf){
	free_line_array(stf.source_files, stf.source_arr_size);
	free_line_array(stf.header_files, stf.header_arr_size);
	free_line_array(stf.object_files, stf.object_arr_size);
}

void generate_solution_output(void){

	/* Basic level */
	printf(GRN("Compiling basic level solution...")"\n");

	if(system(INVOKE_MAKE(SOLUTION_B)) != EXIT_SUCCESS){
		HANDLE_ERROR("Basic level solution did not compile.", 0);
	}

	printf(GRN("\nGenerating basic level output...")"\n");

	if(system(RUN_SOLUTION_B) != EXIT_SUCCESS){
		HANDLE_ERROR("Failed to create basic level output.", 0);
	}

	printf(GRN(
		"Moving output files..."
		"(note that any txt or csv file is moved from CWD)")"\n"
	);
	system(MOVE_OUTPUT_FILES(OUTPUT_B));

	/* Advanced level */
	printf(GRN("\nCompiling advanced level solution...")"\n");

	if(system(INVOKE_MAKE(SOLUTION_A)) != EXIT_SUCCESS){
		HANDLE_ERROR("Advanced level solution did not compile.", 0);
	}

	printf(GRN("\nGenerating advanced level output...")"\n");

	if(system(RUN_SOLUTION_A) != EXIT_SUCCESS){
		HANDLE_ERROR("Failed to create advanced level output.", 0);
	}

	printf(GRN(
		"Moving output files..."
		"(note that any txt or csv file is moved from CWD)")"\n"
	);
	system(MOVE_OUTPUT_FILES(OUTPUT_A));
}

StudentFiles get_student_files(void){
	StudentFiles stf = {NULL, NULL, NULL, HT_BINARY, 0, 0, 0};
	CommandOut files;
	char ext[COMMAND_LINE_BUFFER];

	/* Get all files and sort them */
	files = run_shell_command(FIND_ALL_FILES);
	
	for(int i = 0; i < files.size; i++){
		strcpy(ext, get_file_extension(files.line_array[i]));
		
		if(!strcmp(ext, "c")){
			stf.source_files = append_line_array(
				stf.source_files,
				++stf.source_arr_size,
				files.line_array[i]
			);

		}else if(!strcmp(ext, "h")){
			stf.header_files = append_line_array(
				stf.header_files,
				++stf.header_arr_size,
				files.line_array[i]
			);
			
		}else if(!strcmp(ext, "o")){
			stf.object_files = append_line_array(
				stf.object_files,
				++stf.object_arr_size,
				files.line_array[i]
			);
		}
	}

	free_command_out(files);

	return stf;
}

void clean_source_code_files(StudentFiles stf){
	char *command_buffer;

	for(int i = 0; i < stf.source_arr_size; i++){

		command_buffer = memalloc_formatted(
			REMOVE_COMMENTS,
			4,
			stf.source_files[i],
			stf.source_files[i],
			stf.source_files[i],
			stf.source_files[i]
		);

		if(system(command_buffer) != EXIT_SUCCESS){
			HANDLE_ERROR("Failed to clean the source code.", 0);
		}

		free(command_buffer);
	}
}

void print_frame(
	char *title, const char c, const char *color,
	const int lines, const int title_line){

	char m;
	char *ptr = title;
	int title_len = strlen(title);
	int title_pos = BORDERLEN/2-(title_len/2);

	printf("\n%s", color);

	for(int i = 0; i < lines; i++){
		printf("|");
		for(int j = 0; j < BORDERLEN; j++){
			if(i == title_line && *ptr != '\0' && j >= title_pos){
				m = *ptr++;
			 }else{
				m = c;
			}
			putc(m, stdout);
		}
		printf("|\n");
	}
	printf(END_COLOR);
}

void build_project(void){
	CommandOut out;
	char *command_buffer;

	printf(GRN("Building project...")"\n\n");

	if(system(INVOKE_MAKE(DIR)) != EXIT_SUCCESS){
		HANDLE_ERROR("The program did not compile.", 0);
	}
	
	if(access(DIR"/"HT_BINARY, F_OK) == -1){
		printf(
			YLW("\n"HT_BINARY" binary does not exist.\n"
			"Changing binary name...")"\n"
		);

		out = run_shell_command(FIND_BINARIES);

		command_buffer = memalloc_formatted(
			MOVE("%s/%s", "%s/"HT_BINARY),
			3,
			DIR,
			out.line_array[0],
			DIR
		);

		free_command_out(out);

		if(system(command_buffer) != EXIT_SUCCESS){
			printf(RED("Could not rename the binary.\n"));
			free(command_buffer);
			exit(EXIT_FAILURE);
		}

		free(command_buffer);
	}
}

void get_project(const char *student_num){
	char *copy_project_files;

	CommandOut project_path;
	project_path = run_shell_command(FIND_PROJECT_DIR);
	
	printf(GRN("Finding the project directory...\n"));

	if(project_path.size < 1){
		printf(RED("Could not find the '%s' directory.\n"), HT_RETURNS);
		exit(EXIT_FAILURE);
	}

	if(project_path.size > 1){
		HANDLE_ERROR("More than one path found.", 0);
	}

	printf(GRN("Cleaning the project folder and output files...\n"));

	if(system(CLEAN_OLD_PROJECT) != EXIT_SUCCESS){
		HANDLE_ERROR("Failed to remove the old project.", 0);
	}

	printf(GRN("Moving project files...\n"));

	/* Curly braces in globbing did not cut it.*/
	copy_project_files = memalloc_formatted(
		COPY_PROJECT_FILES,
		6,
		project_path.line_array[0],
		student_num,
		DIR,
		project_path.line_array[0],
		student_num,
		DIR
	);

	if(system(copy_project_files) != EXIT_SUCCESS){
		printf(RED("Failed to copy the project files.\n"));
		free(copy_project_files);
		exit(EXIT_FAILURE);
	}

	free(copy_project_files);
	free_command_out(project_path);
}

/* Init testing environment */
StudentFiles init(
	const char *student_num, const int full_check, const int default_make){
	
	print_frame(" INITIALIZATION ", '=', COLOR_YLW, 1, 0);

	/* Move student's source code */
	get_project(student_num);

	if(default_make){
		if(system(COPY_MAKEFILE(TEST_MAKE, DIR)) != EXIT_SUCCESS){
			HANDLE_ERROR("Couldn't copy test makefile.", 0);
		}
	}
	 
	build_project();
	printf(GRN("\nRunning project...")"\n");

	if(system((full_check)?INVOKE_VALGRIND(ADVANCED_IN):INVOKE_VALGRIND(BASIC_IN)) != EXIT_SUCCESS){
		fprintf(stderr, RED("Program did not exit with code '%d'.\n"), EXIT_SUCCESS);
		exit(EXIT_FAILURE);
	}

	StudentFiles stf = get_student_files();

	return stf;
}
