/*------------------------------------------------------*/
/*                                                      */
/* This file is for the checks. One function per check. */
/*                                                      */
/*--------------------------------------------------SV--*/

#include <inc/config.h>
#include <inc/utilities.h>
#include <inc/general.h>
#include <inc/valgrind.h>

/* Check if student number is included in every source file */
int check_student_information(const char *student_num, StudentFiles stf){
	char *command_buffer;
	CommandOut grep;
	int status = 0;
	
	print_frame(" STUDENT-INFORMATION-CHECK ", '=', COLOR_YLW, 1, 0);

	for(int i = 0; i < stf.source_arr_size; i++){

		command_buffer = memalloc_formatted(
			GREP_STUDENT_NUM,
			2,
			student_num,
			stf.source_files[i]
		);

		grep = run_shell_command(command_buffer);

		if (strcmp(grep.line_array[0], "1")){
			
			printf(
				RED("Did not find the right student number "
				"(or in right format) from %s.\n"),
				stf.source_files[i]
			);
			status = 1;
		}

		free_command_out(grep);
		free(command_buffer);
	}

	return(status);
}

/* Global variable check, extern, headers. Uses nm. */
int check_globals(StudentFiles stf){
	char *command_buffer;
	char *variablename, *scope;
	CommandOut globals;
	int status = 0;

	print_frame(" GLOBAL-CHECK ", '=', COLOR_YLW, 1, 0);

	for(int i = 0; i < stf.object_arr_size; i++){

		command_buffer = memalloc_formatted(
			NM_GLOBALS,
			1,
			stf.object_files[i]
		);

		/* Check the file for globals with nm (B, C, D)*/
		globals = run_shell_command(command_buffer);
		
		for(int j = 0; j < globals.size; j++){

			if((variablename = strtok(globals.line_array[j], " ")) == NULL)
				continue;

			if((scope = strtok(NULL, " ")) == NULL)
				continue;            

			if(scope[0] == 'B' || scope[0] == 'C' || scope[0] == 'D'){
				printf(
					"Variable '%s' in file %s is "
					RED("a global variable.\n"),
					variablename,
					stf.object_files[i]
				);
				status = 1;

			}else if(scope[0] == 'R'){
				printf(
					"Variable '%s' in file %s is "
					GRN("a global constant.\n"),
					variablename,
					stf.object_files[i]
				);
			}
		}

		free_command_out(globals);
		free(command_buffer);
	}

	return(status);
}

/* Run test input and check for memory leaks */
int check_memory_leaks(StudentFiles stf){
	const xmlChar *path[] = {VG_ERROR, VG_UNIQUE};
	ValgrindError *vg_array = NULL;
	ValgrindError vge;
	int vg_array_size = 0, bytes = 0;

	xmlDocPtr doc; /* Tree pointer */
	xmlNodePtr parent, cur; /* Pointer to tree node */

	print_frame(" MEMORY-LEAK-CHECK ", '=', COLOR_YLW, 1, 0);

	doc = xmlParseFile(VG_XMLFILE);

	if (doc == NULL) {
		HANDLE_ERROR("Document not parsed successfully.", 0);
	}

	parent = xmlDocGetRootElement(doc);

	if (parent == NULL) {
			fprintf(stderr,"Empty document\n");
			xmlFreeDoc(doc);
			exit(EXIT_FAILURE);
	}

	if (xmlStrcmp(parent->name, VG_ROOTNODE)) {
			fprintf(
				stderr,
				"document of the wrong type, root node != %s",
				VG_ROOTNODE
			);
			xmlFreeDoc(doc);
			exit(EXIT_FAILURE);
	}

	parent = parent->xmlChildrenNode;

	/* Get all error nodes */
	while (parent != NULL){

		cur = get_xml_node(parent, path, 2);

		if(cur != NULL){
			vge = parse_error_xml_node(doc, cur, stf);
			vg_array = append_error_array(vg_array, ++vg_array_size, vge);
			parse_leak_message(vge);

			/* Update parent to the current node's parent (otherwise duplicate results as the key is found again) */
			parent = cur->parent;
		}
		
		parent = parent->next;
	}

	bytes = get_leak_sum(vg_array, vg_array_size);

	free_error_array(vg_array, vg_array_size);
	xmlFreeDoc(doc);
	xmlCleanupParser();

	if(bytes != 0)
		return 1;

	return 0;
}

int check_exception(StudentFiles stf, char *variable_search){
	CommandOut variable_grep, excp_grep;
	char *command_buffer;
	int status = 0, count = 0;

	for(int i = 0; i < stf.source_arr_size; i++){

		command_buffer = memalloc_formatted(
			variable_search,
			1,
			stf.source_files[i]
		);

		/* Grep the file handles */
		variable_grep = run_shell_command(command_buffer);
		free(command_buffer);

		for(int j = 0; j < variable_grep.size; j++){

			command_buffer = memalloc_formatted(
				ERROR_HANDLING,
				3,
				variable_grep.line_array[j],
				variable_grep.line_array[j],
				stf.source_files[i]
			);

			excp_grep = run_shell_command(command_buffer);
			
			count = instances_in_array(
				&variable_grep.line_array[j],
				variable_grep.size-j,
				variable_grep.line_array[j]);
			
			/* Regex does not match if the condition is not the first one */
			if(excp_grep.size < count){
				printf(
					RED("There is no error handling for"
					" pointer '%s' in file '%s'.\n"),
					variable_grep.line_array[j],
					stf.source_files[i]
				);

				status = 1;
			}
			free_command_out(excp_grep);
			free(command_buffer);
		}
		
		free_command_out(variable_grep);
	}

	return status;
}

/* Exception checks: Alloc.*/
int check_malloc_exceptions(StudentFiles stf){
	int status = 0;

	print_frame(" MALLOC-EXCEPTIONS-CHECK ", '=', COLOR_YLW, 1, 0);
	status = check_exception(stf, GREP_ALLOCATED_MEMORY);
	
	return status;
}

/* Exception checks: Open.*/
int check_file_exceptions(StudentFiles stf){
	int status = 0;

	print_frame(" FILE-EXCEPTIONS-CHECK ", '=', COLOR_YLW, 1, 0);
	printf(GRN("Cleaning source code from comments...")"\n");
	clean_source_code_files(stf);
	status = check_exception(stf, GREP_OPENED_FILES);

	return status;
}

/* File output test */
int check_output(const int full_check){

	print_frame(" OUTPUT-CHECK ", '=', COLOR_YLW, 1, 0);
	printf(GRN(
		"Moving output files..."
		"(note that any txt or csv file is moved from CWD)")"\n"
	);

	/* Will return a 0 if one of basic run as the advanced file does not exist */
	system(MOVE_OUTPUT_FILES(OUTPUT));

	if(system((full_check)?DIFF_OUT(OUTPUT_A):DIFF_OUT(OUTPUT_B)) != EXIT_SUCCESS){
		printf(RED("Some output differ from the example solution\n"));
		
		return 1;
	}

	return 0;
}
