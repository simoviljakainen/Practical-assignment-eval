/*------------------------------------------------------*/
/*                                                      */
/*                  Valgrind functions                  */
/*                                                      */
/*--------------------------------------------------SV--*/
#include <inc/config.h>
#include <inc/utilities.h>
#include <inc/general.h>

ValgrindError *append_error_array(
	ValgrindError *vg_array, int vg_array_size, ValgrindError node){

	ValgrindError *tmp;

	tmp = reallocarray(
		vg_array,
		vg_array_size,
		sizeof(ValgrindError)
	);

	if(tmp == NULL){
		HANDLE_ERROR("Failed to allocate memory for VgError array", 1);
	}

	vg_array = tmp;
	vg_array[vg_array_size-1] = node;
	
	return vg_array;
}

void free_error_array(ValgrindError *array, int size){
	for(int i = 0; i < size; i++){
		free(array[i].error_kind);
		free(array[i].student_file);
		free(array[i].student_function);
	}
	free(array);
}

int get_leak_sum(ValgrindError *array, int array_size){
	int bytes = 0, kbytes = 0, mbytes = 0;

	for(int i = 0; i < array_size; i++){
		bytes += array[i].bytes;
	}

	kbytes = bytes/1024;
	mbytes = kbytes/1024;

	if(mbytes != 0){
		printf(
			"\nProgram is "RED("leaking")": "
			MAG("%d MBs")","MAG(" %d KBs")" and "MAG("%d bytes")".\n"
			, mbytes, kbytes-mbytes*1024, bytes%1024
		);

	}else if(kbytes != 0){
		printf(
			"\nProgram is "RED("leaking")": "
			MAG("%d KBs")" and "MAG("%d bytes")".\n"
			, kbytes-mbytes*1024, bytes%1024);

	}else if(bytes != 0){
		printf("\nProgram is "RED("leaking")": "MAG("%d bytes")".\n", bytes);
	}

	return bytes;
}

/* XML/Valgrind functions below this line */

void parse_leak_message(ValgrindError vge){
	char *dest;
	const char *format;

	if(!strcmp(vge.error_kind, DEFINITELY_LOST)){
		print_frame(" Definitely Lost ", '-', COLOR_RED, 1, 0);
		format = "Allocated memory not freed in "MAG("%d allocations") 
			" ("MAG("%d bytes")"). "
			"Check that every malloc'd string and all of the linked list nodes are freed. "
			"Memory was allocated in file '%s', function '%s', on line %d.\n";
		
		dest = memalloc_formatted(format, 5, vge.blocks, vge.bytes, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);

	}else if(!strcmp(vge.error_kind, INDIRECTLY_LOST)){
		print_frame(" Indirectly Lost ", '-', COLOR_RED, 1, 0);

		format = "Program is leaking memory in a pointer-based structure. "
			"Indirecly "MAG("%d blocks")" were not freed ("MAG("%d bytes")"). "
			"These are probably linked list nodes. "
			"Memory was allocated in file '%s', function '%s', on line %d.\n";
		
		dest = memalloc_formatted(format, 5, vge.blocks, vge.bytes, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);
		
	}else if(!strcmp(vge.error_kind, STILL_REACHABLE)){
		print_frame(" Still reachable ", '-', COLOR_RED, 1, 0);
		
		format = "Program didn't free some memory it could have. "
			"Allocated memory not freed in "MAG("%d allocations") 
			" ("MAG("%d bytes")"). "
			"Check that all opened files have been closed. "
			"Memory was allocated in file '%s', function '%s', on line %d.\n";

		dest = memalloc_formatted(format, 5, vge.blocks, vge.bytes, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);

	}else if(!strcmp(vge.error_kind, POSSIBLY_LOST)){
		print_frame(" Possibly lost ", '-', COLOR_RED, 1, 0);
		
		format = "There is something funky going on with the pointers. "
			"E.g. a pointer is not pointing to the start of the allocated space. "
			"Possibly lost "MAG("%d allocations")" ("MAG("%d bytes")"). "
			"Memory was allocated in file '%s', function '%s', on line %d.\n";

		dest = memalloc_formatted(format, 5, vge.blocks, vge.bytes, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);

	}else if(!strcmp(vge.error_kind, INVALID_FREE)){
		print_frame(" Invalid free ", '-', COLOR_RED, 1, 0);
		
		format = "Program is trying to free something that it did not allocate. "
			"Make sure that you are not freeing something twice or you have "
			"allocted the memory being freed. "
			"Invalid free happened in file '%s', function '%s', on line %d.";
		
		dest = memalloc_formatted(format, 3, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);

	}else if(!strcmp(vge.error_kind, UNINITCONDITION)){
		print_frame(" Uninitialized values ", '-', COLOR_RED, 1, 0);
		
		format = "Conditional jump/move depends on uninitialized values."
			"Make sure that you have initialized before you use them. "
			"Check for uninitialized values in file '%s', function '%s', on line %d.\n";

		dest = memalloc_formatted(format, 3, vge.student_file, 
		vge.student_function, vge.line);
		printf("%s", wrap_string(dest));

		free(dest);
	}
}

xmlNodePtr get_xml_node(
	xmlNodePtr current, const xmlChar *element_path[], int element_count){

	int i = 0;
   
	while(current != NULL){
		if(!xmlStrcmp(current->name, element_path[i])){
			if(i == element_count-1)
				break;

			current = current->xmlChildrenNode;
			i++;
		}
		current = current->next;
	}

	return current;
}

xmlChar *get_node_value(xmlNodePtr node, xmlDocPtr doc){
	xmlChar *key;
	key = xmlNodeListGetString(doc, node->xmlChildrenNode, 1);

	return key;
}

void save_valgrind_error_int(xmlDocPtr doc, xmlNodePtr node, int *dest){
	xmlChar *value;

	value = get_node_value(node, doc);
	*dest = atoi((const char *)value);
	xmlFree(value);
}

char *save_valgrind_error_str(xmlDocPtr doc, xmlNodePtr node, char *dest){
	xmlChar *value;

	value = get_node_value(node, doc);

	dest = (char *)realloc(
		dest,
		sizeof(char) * (strlen((const char *)value) + 1)
	);
 
	if(dest == NULL){
		HANDLE_ERROR("Failed to allocate memory for a valueline", 1);
	}
	
	strcpy(dest, (const char *)value);
	xmlFree(value);

	return dest;
}

void print_valgrind_error(ValgrindError error){

	printf(
		"Error: %s\n"
		"Student file: %s\n"
		"Student function: %s\n"
		"Line: %d\n"
		"Blocks: %d\n"
		"Bytes: %d\n\n",
		error.error_kind, error.student_file, error.student_function,
		error.line, error.blocks, error.bytes
	);
}

void save_valgrind_error_stack(
	xmlDocPtr doc, xmlNodePtr parent, ValgrindError *vge, StudentFiles stf){

	int student_stack = 0;
	xmlNodePtr ptr, cur = parent->xmlChildrenNode;
	xmlChar *value;

	while(cur != NULL){
		
		if(!xmlStrcmp(cur->name, VG_FRAME)){
			ptr = cur->xmlChildrenNode;
			student_stack = 0;

			while(ptr != NULL){
				
				if(!xmlStrcmp(ptr->name, VG_FUNCTION)){
					vge->student_function = save_valgrind_error_str(
						doc,
						ptr,
						vge->student_function
					);

				}else if(!xmlStrcmp(ptr->name, VG_FILE)){
					value = get_node_value(ptr, doc);

					for(int i = 0; i < stf.source_arr_size; i++){
						if(!strcmp(get_filename_pointer(stf.source_files[i]), (const char *)value)){

							/* Top of the stack, this frame should be looked into */
							student_stack = 1;
						}
						vge->student_file = save_valgrind_error_str(
							doc,
							ptr, 
							vge->student_file
						);
					}

					xmlFree(value);

				}else if(!xmlStrcmp(ptr->name, VG_LINE)){
					save_valgrind_error_int(doc, ptr, &vge->line);
				}
				ptr = ptr->next;
			}
		}

		if(student_stack)
			break;

		cur = cur->next;
	}
}

ValgrindError parse_error_xml_node(
	xmlDocPtr doc, xmlNodePtr error_node, StudentFiles stf){

	xmlNodePtr ptr, cur = error_node;
	ValgrindError vge = {NULL, NULL, NULL, 0, 0, 0};

	while (cur != NULL){
		if(!xmlStrcmp(cur->name, VG_KIND)){
			vge.error_kind = save_valgrind_error_str(
				doc,
				cur,
				vge.error_kind
			);

		}else if(!xmlStrcmp(cur->name, VG_XWHAT)){
			ptr = cur->xmlChildrenNode;
			
			while(ptr != NULL){
				
				if(!xmlStrcmp(ptr->name, VG_BYTES)){
					save_valgrind_error_int(doc, ptr, &vge.bytes);

				}else if(!xmlStrcmp(ptr->name, VG_BLOCKS)){
					save_valgrind_error_int(doc, ptr, &vge.blocks);
				}

				ptr = ptr->next;
			}
		}else if(!xmlStrcmp(cur->name, VG_STACK)){
			save_valgrind_error_stack(doc, cur, &vge, stf);
		}

		cur = cur->next;
	}

	return vge;
}
