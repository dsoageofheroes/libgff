#include <gpl/gpl.h>
#include <gpl/string.h>
#include <stdio.h>
#include <string.h>

//extern gff_gpl_string_t* gpl_global_strings;
//extern gff_gpl_string_t* gpl_local_strings;
//extern uint8_t*          gpl_global_string;
//static uint8_t*          gff_gpl_global_string = NULL;
static char          gff_gpl_global_string[4096];

#define INTRODUCE           (0x01)
#define STRING_UNCOMPRESSED (0x02)
#define STRING_COMPRESSED   (0x05)

static char* read_compressed(gpl_data_t *gpl);
static char* introduce();

extern int gff_gpl_read_text(gpl_data_t *gpl, char **ret) {
    uint8_t b;

    if (!ret) { return EXIT_FAILURE; }

    gff_gpl_peek_one_byte(gpl, &b);
    switch(b) {
        case INTRODUCE:
            gff_gpl_get_byte(gpl, &b);
            strcpy((char*)gff_gpl_global_string, introduce());
            //printf("INTRODUCE: gpl_global_string = '%s'\n", gpl_global_string);
            *ret = (char*)gff_gpl_global_string;
            return EXIT_SUCCESS;
            break;
        case STRING_UNCOMPRESSED:
            printf("read_text: STRING_UNCOMPRESSED not implemented!\n");
            gff_gpl_get_byte(gpl, &b);
            return EXIT_SUCCESS;
        case STRING_COMPRESSED:
            gff_gpl_get_byte(gpl, &b);
            *ret = read_compressed(gpl);
            //printf("STRING_COMPRESSED: gpl_global_string = '%s'\n", gff_gpl_global_string);
            //*ret = (char*)gff_gpl_global_string;
            return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

static char* read_compressed(gpl_data_t *gpl) {
    uint16_t i = 0;
    uint32_t buffer = 0, inword = 0;
    uint8_t idx = 1, b;

    while (i < (TEXTSTRINGSIZE - 1)) {
        if (idx > 0) {
            buffer = ( (buffer << 8) & 0xFF00);
            gff_gpl_get_byte(gpl, &b);
            buffer |= (uint32_t)b;
        }

        inword = ( (buffer >> idx) & 0x7F);
        gff_gpl_global_string[i] = (char)inword;
        if (gff_gpl_global_string[i] == 0x03) {
            gff_gpl_global_string[i] = 0x00;
            return (char*)gff_gpl_global_string;
        }
        
        if (gff_gpl_global_string[i] < 0x20 || gff_gpl_global_string[i] > 0x7E) {
            gff_gpl_global_string[i] = 0x20;
        }

        i++;
        idx++;

        if (idx > 7) {
            idx = 0;
        }
    }

    gff_gpl_global_string[i] = 0;

    return (char*)gff_gpl_global_string;
}

static char tempstr[1024];
static char* introduce() {
    sprintf(tempstr, "I'm \" GET_ACTIVE_NAME() \"");
    return tempstr;
}
