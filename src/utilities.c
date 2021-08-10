/*------------------------------------------------------*/
/*                                                      */
/*   This file is for functions that checks/tests use   */
/*                                                      */
/*--------------------------------------------------SV--*/

#include <inc/config.h>
#include <malloc.h> /* for reallocarray */
#include <errno.h>

/* Call this through HANDLE_ERROR(msg, show_err) macro */
void handle_error(
	const char *msg, int show_err, const char *file, const int line){

	fprintf(
		stderr,
		"ERROR(%d) %s:%d -- "RED("%s")":%s\n",
		errno,
		file, line,
		(msg != NULL) ? msg : "-",
		(show_err) ? strerror(errno): "-"
	);
	exit(EXIT_FAILURE);
}

char *memalloc_formatted(const char *format, int argc, ...){
	va_list list;
	char *buffer;
	size_t size;

	va_start(list, argc);

	if((size = vsnprintf(NULL, 0, format, list)) < argc){
		HANDLE_ERROR("Failed to format data with vsnprintf", 1);
	}

	va_end(list);

	if((buffer = (char *)malloc((size + 1))) == NULL){
		HANDLE_ERROR("Could not allocate memory for formatted string", 1);
	}

	va_start(list, argc);

	if((vsnprintf(buffer, size + 1, format, list)) < argc){
		HANDLE_ERROR("Failed to format data with vsnprintf", 1);
	}
	
	va_end(list);

	return buffer;
}

char *wrap_string(char *string){
	char *ptr_right = NULL, *ptr_left = NULL, *ptr_match = NULL, *substr = NULL;
	int len = strlen(string), count_right = 0, count_left = 0, macro_count = 0;

	char *start = string;
	char *end = &string[len-1];
	char *ptr = start;

	if((substr = (char *)calloc(LINELEN + 1, sizeof(char))) == NULL){
		HANDLE_ERROR("Could not allocate memory for wrapped line", 1);
	}

	for(;;){

		if (ptr > end)
			break;

		memcpy(substr, ptr, ((ptr + LINELEN) <= end) ? LINELEN : end-ptr);
		
		macro_count = 0;
		ptr_match = substr;

		while((ptr_match = strstr(ptr_match, COLOR_MAG)) != NULL){
			macro_count++;
			ptr_match = ptr_match + sizeof(COLOR_MAG);
		}

		ptr = ptr + LINELEN + (sizeof(MAG("")) - 1) * macro_count;
		if (ptr > end)
			break;
		
		ptr_right = ptr_left = ptr;
		count_right = count_left = 0;
		while(((*ptr_right) != ' ') && (ptr_right != end)){
			ptr_right++;
			count_right++;
		}

		while(((*ptr_left) != ' ') && (ptr_left != start)){
			ptr_left--;
			count_left++;
		}

		if (count_right > count_left){
			*ptr_left = '\n';
			ptr = ptr_left+1;
		}
		else{
			*ptr_right = '\n';
			ptr = ptr_right+1;
		}
	}

	free(substr);

	return string;	
}

int len_with_strip(char *c){
	int i = 0;
	
	while(*c != '\n' && *c != '\0' && *c != '\r'){
		i++, c++;
	}
	return i;
}

int instances_in_array(char **line_array, int size, const char *string){
	int count = 0;

	for(int i = 0; i < size; i++){
		if(!strcmp(line_array[i], string))
			count++;
	}

	return count;
}

char **append_line_array(char **line_array, int size, const char *line){
	char **tmp;

	tmp = reallocarray(
		line_array,
		size,
		sizeof(char *)
	);

	if(tmp == NULL){
		HANDLE_ERROR("Failed to allocate memory for line array", 1);
	}

	line_array = tmp;

	/* Allocate memory for the line */
	line_array[size - 1] = (char *)malloc(
		sizeof(char) * (strlen(line) + 1)
	);

	if(line_array[size - 1] == NULL){
		HANDLE_ERROR("Failed to allocate memory for a line", 1);
	}
	
	strcpy(line_array[size - 1], line);

	return line_array;
}

void free_line_array(char **line_array, int size){

	for(int i = 0; i < size; i++){    
		free(line_array[i]);
	}

	free(line_array);
}

void free_command_out(CommandOut out){
	free_line_array(out.line_array, out.size);
}

const char *get_file_extension(const char *filepath){
	const char *ext = strrchr(filepath, '.');
	return ++ext;
}

/* Adds shell output into array. Each array element represents an output line.*/
CommandOut run_shell_command(char *command){
	char line_buffer[COMMAND_LINE_BUFFER];
	CommandOut out = {NULL, 0};
	FILE *pipe;

	if((pipe = popen(command, "r")) == NULL){
		HANDLE_ERROR("Pipe failed", 1);
	}

	while(fgets(line_buffer, COMMAND_LINE_BUFFER, pipe) != NULL){
		out.size++;
		line_buffer[strcspn(line_buffer, "\n\r")] = '\0';
		out.line_array = append_line_array(out.line_array, out.size, line_buffer);
	}

	pclose(pipe);

	return out;
}

const char *get_filename_pointer(char *path){
	int path_length = strlen(path);
	char *c = &path[path_length-1];

	for(; c > path && *(c) != '/'; c--);

	return c + 1;
}
