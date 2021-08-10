/*------------------------------------------------------*/
/*                                                      */
/*            This is the configuration file            */
/*                                                      */
/*--------------------------------------------------SV--*/

#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define COMMAND_LINE_BUFFER 255

/* Paths/filenames*/
#define ADVANCED "advanced"
#define BASIC "basic"

#define DIR "test/evaluate"
#define HT_RETURNS "submissions"
#define OUTPUT "test/output"
#define INPUT "test/input"
#define INPUT_DATA "test/data"
#define SOLUTION_OUTPUT "test/expected/output"
#define SOLUTIONS "test/expected/solutions"
#define VALGRIND_OUT "test/valgrind"
#define TEST_MAKE "test/expected/testmake"

#define OUTPUT_A SOLUTION_OUTPUT"/"ADVANCED
#define OUTPUT_B SOLUTION_OUTPUT"/"BASIC
#define SOLUTION_A SOLUTIONS"/"ADVANCED
#define SOLUTION_B SOLUTIONS"/"BASIC

#define ADVANCED_IN "test_input_tavot.txt"
#define BASIC_IN "test_input_perus.txt"
#define HT_BINARY "HT"
#define VG_XMLFILE VALGRIND_OUT"/file.xml"

#define HANDLE_ERROR(msg, show_err) handle_error(msg, show_err, __FILE__, __LINE__);

/* Display */
#define BORDERLEN 80
#define LINELEN 80

#define COLOR_RED "\x1B[31m"
#define COLOR_GRN "\x1B[32m"
#define COLOR_YLW "\x1B[33m"
#define COLOR_MAG "\x1B[35m"
#define END_COLOR "\033[0m"

#define RED(text) COLOR_RED""text""END_COLOR
#define GRN(text) COLOR_GRN""text""END_COLOR
#define YLW(text) COLOR_YLW""text""END_COLOR
#define MAG(text) COLOR_MAG""text""END_COLOR

/* Shell commands */
#define IGNORE_ERRORS "2>/dev/null"

#define FIND_ALL_FILES "find ./" DIR " -name '[!.]*'"
#define FIND_PROJECT_DIR "find ~ -type d -name \"" HT_RETURNS "\" "IGNORE_ERRORS
#define FIND_BINARIES "find ./" DIR " -type f -perm -111 -printf \"%f\n\""
#define GREP_STUDENT_NUM "grep -E \"[Oo]piskelijanumero:[^1-9]*%s\" %s | wc -l"
#define ERROR_HANDLING "grep -Eno '(\\(\\s*(%s[^a-zA-Z_0-9].*(=|\\!)=\\s*(NULL|0|-1))|if[\\(\\!\\s]+%s\\s*\\)+)' %s | cut -f2 -d:"
#define GREP_OPENED_FILES "grep -Po '([a-zA-Z_][a-zA-Z_0-9]*)(?=\\s*=\\s*[f]{0,1}open\\s*\\()' %s"
#define GREP_ALLOCATED_MEMORY "grep -Po '([a-zA-Z_][a-zA-Z_0-9]*)(?=\\s*=\\s*[\\(\\)\\*a-zA-Z_0-9\\s]*(m|re|c)alloc\\s*\\()' %s"
#define MOVE(from, to) "mv "from" "to
#define MOVE_OUTPUT_FILES(to) "mv -t"to"/ *\\.csv *\\.txt " IGNORE_ERRORS

#define NM_GLOBALS "nm -g -C -l -f \"posix\" %s"
#define CLEAN_OLD_PROJECT "rm -fr " DIR "/*[^\\.gitignore];rm -f " OUTPUT "/*;rm -f " VALGRIND_OUT "/*"
#define COPY_MAKEFILE(from, to) "cp "from"/[mM]akefile "to"/ "IGNORE_ERRORS
#define COPY_PROJECT_FILES "cp %s/%s/*\\.[ch] %s/ "IGNORE_ERRORS" && "COPY_MAKEFILE("%s/%s", "%s")
#define INVOKE_MAKE(path) "cd " path "&&make"
#define REMOVE_COMMENTS "gcc -fpreprocessed -dD -E %s > %s_temp&&mv %s_temp %s"

#define INVOKE_VALGRIND(input_file) "valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all -v "\
									"--xml=yes --xml-file="VALGRIND_OUT"/file.xml --log-file="VALGRIND_OUT"/valgrind.txt ./"\
									DIR "/"HT_BINARY" < "INPUT"/"input_file" > "OUTPUT"/output.txt\n"

#define DIFF_OUT(input_dir) "diff -rqs "input_dir" "OUTPUT

#define RUN_SOLUTION_A "./"SOLUTION_A"/"HT_BINARY" < "INPUT"/"ADVANCED_IN" > " OUTPUT_A"/output.txt"
#define RUN_SOLUTION_B "./"SOLUTION_B"/"HT_BINARY" < "INPUT"/"BASIC_IN" > " OUTPUT_B"/output.txt"


/* Valgrind */
#define DEFINITELY_LOST "Leak_DefinitelyLost"
#define INDIRECTLY_LOST "Leak_IndirectlyLost"
#define STILL_REACHABLE "Leak_StillReachable"
#define POSSIBLY_LOST "Leak_PossiblyLost"
#define INVALID_FREE "InvalidFree"
#define UNINITCONDITION "UninitCondition"

#define VG_FRAME (const xmlChar *)"frame"
#define VG_FUNCTION (const xmlChar *)"fn"
#define VG_FILE (const xmlChar *)"file"
#define VG_LINE (const xmlChar *)"line"
#define VG_KIND (const xmlChar *)"kind"
#define VG_XWHAT (const xmlChar *)"xwhat"
#define VG_BYTES (const xmlChar *)"leakedbytes"
#define VG_BLOCKS (const xmlChar *)"leakedblocks"
#define VG_STACK (const xmlChar *)"stack"
#define VG_ERROR (const xmlChar *)"error"
#define VG_UNIQUE (const xmlChar *)"unique"

#define VG_ROOTNODE (const xmlChar *) "valgrindoutput"


/* Structs */
typedef struct CommandOutput{
	char **line_array;
	int size;
} CommandOut;

typedef struct Files{
	char **source_files;
	char **header_files;
	char **object_files;
	char *binary;
	int source_arr_size;
	int header_arr_size;
	int object_arr_size;
} StudentFiles;

typedef struct ValgrindError_{
	char *error_kind;
	char *student_file;
	char *student_function;
	int line;
	int blocks;
	int bytes;
} ValgrindError;

#endif