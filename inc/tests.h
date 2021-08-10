/*------------------------------------------------------*/
/*                                                      */
/*       This is the header for test prototypes         */
/*                                                      */
/*--------------------------------------------------SV--*/
#ifndef TESTS_H
#define TESTS_H

int check_student_information(const char *student_num, StudentFiles stf);
int check_globals(StudentFiles stf);
int check_memory_leaks(StudentFiles stf);
int check_exception(StudentFiles stf, char *variable_search);
int check_file_exceptions(StudentFiles stf);
int check_malloc_exceptions(StudentFiles stf);
int check_output(const int full_check);

#endif