/*------------------------------------------------------*/
/*                                                      */
/*         This header is for general prototypes        */
/*                                                      */
/*--------------------------------------------------SV--*/

#ifndef GENERAL_H
#define GENERAL_H

void clean(StudentFiles stf);
void generate_solution_output(void);
StudentFiles get_student_files(void);
void get_project(const char *student_num);
void build_project(void);
void clean_source_code_files(StudentFiles stf);

void print_frame(
    char *title, const char c, const char *color,
    const int lines, const int title_line
);
StudentFiles init(
    const char *student_num, const int full_check,
    const int default_make
);

#endif