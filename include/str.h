#ifndef GFF_GPL_STRING_H
#define GFF_GPL_STRING_H

#include <stdlib.h>
#include <stdint.h>
#include "status.h"

#define DSSTRINGLENGTH (40)
#define DSSTRLEN (20)

typedef char gpl_string_t[DSSTRINGLENGTH+2];

/*
extern gff_gpl_string_t* gff_gpl_global_strings;
extern gff_gpl_string_t* gff_gpl_local_strings;
extern uint8_t*          gff_gpl_global_string;
*/

#define MAXGSTRINGS    (10)
#define GSTRINGVARSIZE (MAXGSTRINGS*sizeof(gff_gpl_string_t))
#define MAXLSTRINGS    (10)
#define LSTRINGVARSIZE (MAXLSTRINGS*sizeof(gff_gpl_string_t))

extern gff_status_t gff_gpl_read_text(char **ret);
#endif
