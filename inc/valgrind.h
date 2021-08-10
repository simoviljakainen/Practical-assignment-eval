/*------------------------------------------------------*/
/*                                                      */
/*       This is the header file for VG prototypes      */
/*                                                      */
/*--------------------------------------------------SV--*/

#ifndef VALGRIND_H
#define VALGRIND_H

ValgrindError *append_error_array(ValgrindError *vg_array, int vg_array_size, ValgrindError node);
void free_error_array(ValgrindError *array, int size);
int get_leak_sum(ValgrindError *array, int array_size);
void parse_leak_message(ValgrindError vge);

xmlNodePtr get_xml_node(xmlNodePtr current, const xmlChar *element_path[], int element_count);
xmlChar *get_node_value(xmlNodePtr node, xmlDocPtr doc);
void save_valgrind_error_int(xmlDocPtr doc, xmlNodePtr node, int *dest);
char *save_valgrind_error_str(xmlDocPtr doc, xmlNodePtr node, char *dest);
void print_valgrind_error(ValgrindError error);
void save_valgrind_error_stack(xmlDocPtr doc, xmlNodePtr parent, ValgrindError *vge, StudentFiles stf);
ValgrindError parse_error_xml_node(xmlDocPtr doc, xmlNodePtr error_node, StudentFiles stf);

#endif