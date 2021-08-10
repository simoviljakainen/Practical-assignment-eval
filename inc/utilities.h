/*------------------------------------------------------*/
/*                                                      */
/*          This file is for utility prototypes         */
/*                                                      */
/*--------------------------------------------------SV--*/

#ifndef UTILITIES_H
#define UTILITIES_H

char *memalloc_formatted(const char *format, int argc, ...);
char *wrap_string(char *string);
int len_with_strip(char *c);
const char *get_filename_pointer(char *path);
int instances_in_array(char **line_array, int size, const char *string);
char **append_line_array(char **line_array, int size, const char *line);
void free_line_array(char **line_array, int size);
const char *get_file_extension(char *filepath);
CommandOut run_shell_command(char *command);
void free_command_out(CommandOut out);

void handle_error(
    const char *msg, int show_err, const char *file, const int line
);

#endif