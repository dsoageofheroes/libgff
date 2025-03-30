#include <gpl/gpl.h>
#include <gpl/string.h>
#include <gpl/var.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

char arg0[1024];

typedef struct gpl_command_s {
    int (*func)(gpl_data_t *gpl);
    const char *name;
} gpl_command_t;

static void print_out(int count, va_list ap) {
    if (count > 0) {
        printf("%s (", va_arg(ap, char*));
    }
    if (count > 1) {
        printf("%s", va_arg(ap, char*));
    }
    for(int i = 2; i < count; i++) {
        printf(", %s", va_arg(ap, char*));
    }
    printf(")");
}

static void print_cmd(int count, ...) {
    va_list args;
    va_start(args, count);
    print_out(count, args);
    va_end(args);
    printf("\n");
}

static void print_inline(int count, ...) {
    va_list args;
    va_start(args, count);
    print_out(count, args);
    va_end(args);
}

static void gpl_retval(gpl_data_t *gpl, uint8_t cmd);

static int32_t gpl_global_big_num = 0;

static int gpl_preview_byte(gpl_data_t *gpl, uint8_t *b) {
    *b = (uint8_t) *(gpl->gpl);
    return EXIT_SUCCESS;
}

static int gpl_preview_halfword(gpl_data_t *gpl, uint16_t *h) {
    *h = (uint32_t) *(gpl->gpl);
    return EXIT_SUCCESS;
}

extern int gff_gpl_get_byte(gpl_data_t *gpl, uint8_t *b) {
    *b = (uint8_t) *(gpl->gpl);
    gpl->gpl++;
    return EXIT_SUCCESS;
}
#define gpl_get_byte gff_gpl_get_byte

extern int gff_gpl_peek_one_byte(gpl_data_t *gpl, uint8_t *d) {
    *d = *gpl->gpl;
    return EXIT_SUCCESS;
}

static int gpl_get_half_word(gpl_data_t *gpl, uint16_t *h) {
    uint8_t b;
    gpl_get_byte(gpl, &b);
    *h = b * 0x100;
    gpl_get_byte(gpl, &b);
    *h += b;
    return EXIT_SUCCESS;
}

static int gpl_get_word(gpl_data_t *gpl, uint16_t *w) {
    uint8_t b;
    gpl_get_byte(gpl, &b);
    *w = b * 0x100;
    gpl_get_byte(gpl, &b);
    *w += b;
    return EXIT_SUCCESS;
}

static int gpl_preview_byte16(gpl_data_t *gpl, uint16_t *d) {
    *d = *(gpl->gpl);
    return EXIT_SUCCESS;
}

static void gpl_load_simple_variable(gpl_data_t *gpl, uint16_t type, uint16_t vnum) {
    switch (type) {
        case GPL_GBIGNUM:
            //gpl_global_bnums[vnum] = (int32_t) val;
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_GBN", arg0, "accum");
            break;
        case GPL_LBIGNUM:
            //gpl_local_bnums[vnum] = (int32_t) val;
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_LBN", arg0, "accum");
            break;
        case GPL_GNUM:
            //gpl_global_nums[vnum] = (int16_t) val;
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_GN", arg0, "accum");
            break;
        case GPL_LNUM:
            //gpl_local_nums[vnum] = (int16_t) val;
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_LN", arg0, "accum");
            break;
        case GPL_GFLAG:
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_GF", arg0, "accum");
            break;
        case GPL_LFLAG:
            sprintf(arg0, "%d", vnum);
            print_cmd(3, "SET_LF", arg0, "accum");
            break;
        default:
            error("ERROR: Unknown simple variable type! 0x%x! ", type);
            *((char*)NULL)  = 2;
            break;
    }
}

static int gpl_unknown(gpl_data_t *gpl) {
    print_cmd(1, "UNKNOWN_COMMAND");
    return EXIT_FAILURE;
}

static int gpl_read_simple_num_var(gpl_data_t *gpl) {
    uint8_t b;
    int16_t temps16;
   
    gpl_get_byte(gpl, &b);
    temps16 = b;

    if ((gpl_global_big_num & EXTENDED_VAR) != 0) {
        temps16 *= 0x100;
        gpl_get_byte(gpl, &b);
        temps16 += b;
        gpl_global_big_num &= 0x3F;
    }
    //if (debug) { printf("temps16 = %d\n", temps16); }

    //printf("sol_gpl_global_big_num = %d, temps16 = %d\n", sol_gpl_global_big_num, temps16);
    switch(gpl_global_big_num) {
        case GPL_GFLAG: {
            //return snprintf(buf, buf_size, "gpl.get_gf(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_GF", arg0);
            break;
        }
        case GPL_LFLAG: {
            //return snprintf(buf, buf_size, "gpl.get_lf(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_LF", arg0);
            break;
        }
        case GPL_GNUM: {
            //return snprintf(buf, buf_size, "gpl.get_gn(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_GN", arg0);
            break;
        }
        case GPL_LNUM: {
            //return snprintf(buf, buf_size, "gpl.get_ln(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_LN", arg0);
            break;
        }
        case GPL_GBIGNUM: {
            //return snprintf(buf, buf_size, "gpl.get_gbn(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_GBN", arg0);
            break;
        }
        case GPL_LBIGNUM: {
            //return snprintf(buf, buf_size, "gpl.get_lbn(%d)", temps16);
            sprintf(arg0, "%d", temps16);
            print_inline(2, "GET_LBN", arg0);
            break;
        }
        case GPL_GNAME: {
            if (temps16 >= 0x20 && temps16 < 0x2F) {
                //return snprintf(buf, buf_size, "global_simple_var[%d]", temps16 - 0x20);
                //return snprintf(buf, buf_size, "\"gpl.get_gname(%d)\"", temps16 - 0x20);
                //printf("gpl.get_gname(%d)\n", temps16 - 0x20);
                //return snprintf(buf, buf_size, "gpl.get_gname(%d)", temps16 - 0x20);
                //printf("GET_GNAME(%d) ", temps16 - 0x20);
                sprintf(arg0, "%d", temps16 - 0x20);
                print_inline(2, "GET_GNAME", arg0);
            } else {
                //printf("NO VARIABLE GNAME!\n");
                error("ERROR: No variable GNAME!!!\n");
                return EXIT_FAILURE;
            }
            //debug("reading gname @ %d is equal to %d\n", temps16, gpl_global_big_num);
            //lua_exit ("GPL_GNAME not implemented.\n");
            break;
        }
        case GPL_GSTRING: {
            //gpl_global_big_numptr = (int32_t*) gpl_global_strings[temps16];
            //debug("reading gstring @ %d is equal to '%s'\n", temps16, (char*)gpl_global_big_numptr);
            //lua_exit ("GPL_GString not implemented.\n");
            /*
            gpl_global_big_numptr = (int32_t*) gpl_global_strings[temps16];
            */
            //return snprintf(buf, buf_size, "\" .. gpl.get_gstr(%d) .. \"", temps16);
            printf("GET_GSTR(%d) ", temps16);
            //return snprintf(buf, buf_size, "gpl.get_gstr(%d)", temps16);
            break;
        }
        case GPL_LSTRING: {
            //return snprintf(buf, buf_size, "gpl.get_lstr(%d)", temps16);
            //lua_exit ("GPL_LString not implemented.\n");
            /*
            gpl_global_big_numptr = (int32_t*) gpl_local_strings[temps16];
            debug("reading lstring @ %d is equal to '%s'\n", temps16, (char*)gpl_global_big_numptr);
            */
            printf("GET_LSTR(%d) ", temps16);
            break;
        }
        default:
            error("ERROR: Unknown type in read_simple_num_var.\n");
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int gpl_access_complex(gpl_data_t *gpl, int16_t *header, uint16_t *depth, uint16_t *element, int32_t *obj_name) {
    uint16_t i;
    uint8_t b;
    
    gpl_get_word(gpl, &i);
    *obj_name = (int32_t)i;
    //debug("header = %d, depth = %d, element = %d, obj_name = %d\n", *header, *depth, *element, obj_name);
    if (*obj_name < 0x8000) {
        //lprintf("--access_complex: I need to convert from ID to header!\n");
        printf(" ID = ( %d ) ", *obj_name);
    } else {
        //lprintf("--access_complex: I need to set the *head to the correct view\n");
        switch (*obj_name & 0x7FFF) {
            case 0x25: // POV, which is active character.
                printf(" SET CURRENT TO POV ");
                //lprintf("--access_complex: get POV, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            case 0x26: // ACTIVE
                printf(" SET CURRENT TO ACTIVE ");
                //lprintf("--access_complex: get ACTIVE, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            case 0x27: // PASSIVE
                printf(" SET CURRENT TO PASSIVE ");
                //lprintf("--access_complex: get PASSIVE, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            case 0x28: // OTHER
                printf(" SET CURRENT TO OTHER ");
                //lprintf("--access_complex: get OTHER, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            case 0x2C: // OTHER1
                printf(" SET CURRENT TO OTHER1 ");
                //lprintf("--access_complex: get OTHER1, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            case 0x2B: // THING
                printf(" SET CURRENT TO THING ");
                //lprintf("--access_complex: get THING, valid obj_name(%d), need to set header\n", *obj_name & 0x7FFF);
                break;
            default:
                error("--ACCESS_COMPLEX: UNKNOWN ARG: %d", *obj_name & 0x7FFF);
                return EXIT_FAILURE;
        }
    }
    gpl_get_byte(gpl, &b);
    *depth = b;
    //printf("ELEMENT: [");
    for (i = 1; i <= *depth; i++) {
        gpl_get_byte(gpl, &b);
        element[i-1] = b;
        //printf("%d ", b);
    }
    //printf("]");

    return EXIT_SUCCESS;
}

static void gpl_write_complex_var(gpl_data_t *gpl) {
    uint16_t depth = 0;
    int16_t header = 0;
    uint16_t element[MAX_SEARCH_STACK];
    int32_t obj_name;

    memset(element, 0x0, sizeof(uint16_t) * MAX_SEARCH_STACK);
 
    if (gpl_access_complex(gpl, &header, &depth, element, &obj_name) == EXIT_SUCCESS) {
        //lprintf("--complex_write: I need to write 'accum' to header (%d) at depth (%d)\n", header, depth);
        //lprintf("print (\"writing complex: header=\", %d, \"depth = \", %d, \"obj_name = \", %d)", header, depth, obj_name);
        //smart_write_data(header, depth, element, data);
        printf("gpl.set_element(%d, %d, %d, %d, accum) ", obj_name, header, depth, *element);
    } else {
        error("print (\"write complex failed\") ");
    }
}


int32_t gpl_get_complex_data(int16_t header, uint16_t depth, uint16_t *element) {
    /*
    uint16_t i = 0;
    int32_t ret;

    ret = header; // start with header.

    do {
        ret = data_field((int16_t) ret, i);
        element++;
        i++;
    } while ((i < depth));
    */
    //} while (/*(field_error == 0) && */ (i < depth));
    //return ret;
    printf("--GET_COMPLEX_DATA: HEADER:%d, DEPTH:%d ELEMENT:%d\n", header, depth, *element);
    //exit(1);
    return 1;
}

static int32_t gpl_read_complex(gpl_data_t *gpl) {//, char *buf, size_t *buf_pos, const size_t size) {
    int32_t ret = 0;
    uint16_t depth = 0;
    int16_t header = 0;
    uint16_t element[MAX_SEARCH_STACK];
    int32_t obj_name;
    memset(element, 0x0, sizeof(uint16_t) * MAX_SEARCH_STACK);

    if (gpl_access_complex(gpl, &header, &depth, element, &obj_name) == EXIT_SUCCESS) {
        // So if depth == 0 then get type, is the type, 1 is the id (like -319)
        switch (depth) {
            case 0:
                //lprintf("gpl.get_type(obj)\n"); 
                //*buf_pos += snprintf(buf + *buf_pos, size - *buf_pos, "gpl.get_type(obj)");
                printf("GET_TYPE(obj)\n");
                break;
            case 1:
                //lprintf("gpl.get_id(obj)\n");
                //printf("read_complex: %d %d %d\n", obj_name, header, depth);
                //*buf_pos += snprintf(buf + *buf_pos, size - *buf_pos, "gpl.get_id(obj, %d)", obj_name);
                //*buf_pos += snprintf(buf + *buf_pos, size - *buf_pos,
                     //"gpl.get_element(%d, %d, %d, %d)", obj_name, header, depth, *element);
                printf("GET_ELEMENT(%d, %d, %d, %d)", obj_name, header, depth, *element);
                break;
            default:
                //*buf_pos += snprintf(buf + *buf_pos, size - *buf_pos, "gpl.read_complex(%d, %d, %d)", obj_name, header, depth);
                printf("READ_COMPLEX(%d, %d, %d)", obj_name, header, depth);
                break;
        }
        //printf("--read_complex:reading header (%d) at depth (%d)\n", header, depth);
        //printf("--gpl.get_element(%d, %d, %d, %d)\n", obj_name, header, depth, *element);
        ret = gpl_get_complex_data(header, depth, element);
        return ret;
    } 
    printf(" SET GBIGNUMPTR TO PREVIOUS ");

    return ret;
}

//static size_t gpl_read_number(r *buf, const size_t size) {
static size_t gpl_read_number(gpl_data_t *gpl) {
    int32_t paren_level = 0;
    int8_t do_next;
    uint8_t b;
    uint16_t h, w;
    //int16_t opstack[MAX_PARENS];
    //int32_t accums[MAX_PARENS];
    //char taccum[BUF_SIZE];
    uint16_t cop, next_op; // current operation
    int32_t cval = 0; // current value, temporary value
    size_t buf_pos = 0;

    //memset(opstack, 0, sizeof(opstack));
    //memset(accums, 0, sizeof(accums));
    //memset(buf, 0, size); // TODO: FIXME: we shouldn't need to memset this EVERY time.
    //lprintf("---- read_number_start\n");
    do {
     //   taccum[0] = '\0';
        do_next = 0;
        //lprintf("---- buf_pos = %zu\n", buf_pos);
        gpl_get_byte(gpl, &b);
        cop = b; // current operation
        //printf("current operation = 0x%x\n", cop);
        if (cop < 0x80) {
            gpl_get_byte(gpl, &b);
            cval = cop * 0x100 + b;
            //buf_pos += snprintf(buf + buf_pos, size - buf_pos, "%d", cval);
            printf("read_number: %d ", cval);
        } else {
            if (cop < OPERATOR_OFFSET) {
                if (cop & EXTENDED_VAR) { // variable is > 255
                    cop -= EXTENDED_VAR;
                    gpl_global_big_num = EXTENDED_VAR;
                } else {
                    gpl_global_big_num = 0;
                }
            }
            switch (cop) {
                case GPL_ACCM|0x80: {
                    //cval = accum;
                    printf("accum error! ");
                    exit(1);
                    break;
                }
                case GPL_LNAME|0x80:
                case GPL_GNAME|0x80:
                case GPL_LNUM|0x80:
                case GPL_LBYTE|0x80:
                case GPL_LBIGNUM|0x80:
                case GPL_GNUM|0x80:
                case GPL_GBYTE|0x80:
                case GPL_GBIGNUM|0x80:
                case GPL_GFLAG|0x80:
                case GPL_LFLAG|0x80:
                case GPL_GSTRING|0x80:
                case GPL_LSTRING|0x80: {
                    gpl_global_big_num += cop & 0x7F;
                    cval = gpl_global_big_num;
                    //int amt = gpl_lua_read_simple_num_var(buf + buf_pos, size - buf_pos);
                    gpl_read_simple_num_var(gpl);
                    //printf("amt = %d, buf = '%s'\n", amt, buf);
                    //buf_pos += amt;
                    break;
                }
                case GPL_RETVAL|0x80: {
                    //printf("DOUBLE PAREN\n");
                    //int tpos = lua_pos;
                    //char *tptr = lua_buf + lua_pos;

                    printf("((");
                    gpl_get_byte(gpl, &b);
                    gpl_retval(gpl, b);

                    printf("))");

                    break;
                }
                case GPL_IMMED_BIGNUM|0x80: {
                    gpl_get_word(gpl, &w);
                    cval = 0x0;
                    uint16_t *t = (uint16_t*)&cval;
                    t[1] = w;
                    gpl_get_word(gpl, &w);
                    cval += ((int32_t)w);
                    //printf("HERE: %d %d: %d\n", t[0], t[1], cval);
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, "%d", cval);
                    printf("GPL_IMMED_GN: %d ", cval);
                    break;
                }
                case GPL_IMMED_BYTE|0x80: {
                    gpl_get_byte(gpl, &b);
                    cval = (int32_t)((int8_t)b);
                    //debug("GPL_IMMED_BYTE, cval = %d\n", cval);
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, "%d", cval);
                    printf("GPL_IMMED_BYTE:%d ", cval);
                    //printf("updating buf_pos = %d\n", buf_pos);
                    break;
                }
                case GPL_IMMED_WORD|0x80: {
                    error("GPL_IMMED_WORD not implemented!\n");
                    exit(1);
                    break;
                }
                case GPL_IMMED_NAME|0x80: {
                    gpl_get_half_word(gpl, &h);
                    cval = h * -1;
                    printf("IMMED_NAME: %d ", cval);
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, "%d", cval);
                    //printf("updating buf_pos = %d\n", buf_pos);
                    //lua_exit("GPL_IMMED_NAME not implemented!\n");
                    break;
                }
                case GPL_COMPLEX_VAL|0x80: {
                    cval = gpl_read_complex(gpl);//, buf, &buf_pos, size);
                    //lua_exit("GPL_COMPLEX_VAL not implemented!\n");
                    break;
                }
                case GPL_IMMED_STRING|0x80: {
                    //cval = 0;
                    //gpl_global_big_numptr = (int32_t*) read_text();
                    //debug("GPL_IMMED_STRING, cval = %d, '%s'\n", cval, (char*) gpl_global_big_numptr);
                    //TODO FIXME: This should just copy directly in!
                    char *tmp;
                    gff_gpl_read_text(gpl, &tmp);//), "Unable to read immediate string in GPL.");
                    printf("GPL_IMMED_STRING: %s ", tmp);
                    //buf_pos += strlen(strncpy(buf + buf_pos, tmp, size - buf_pos));
                    //printf("updating buf_pos = %d\n", buf_pos);
                    break;
                }
                case GPL_OP_ADD:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " + ");
                    printf("GPL_OP_ADD ");
                    do_next = 1;
                    break;
                case GPL_OP_MINUS:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " - ");
                    printf("GPL_OP_MINUS ");
                    do_next = 1;
                    break;
                case GPL_OP_TIMES:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " * ");
                    printf("GPL_OP_TIMES ");
                    do_next = 1;
                    break;
                case GPL_OP_DIVIDE:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " / ");
                    printf("GPL_OP_DIVIDE ");
                    do_next = 1;
                    break;
                case GPL_OP_AND:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " and ");
                    printf("GPL_OP_AND ");
                    do_next = 1;
                    break;
                case GPL_OP_OR:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " or ");
                    printf("GPL_OP_OR ");
                    do_next = 1;
                    break;
                case GPL_OP_EQUAL:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " == ");
                    printf("GPL_OP_== ");
                    do_next = 1;
                    break;
                case GPL_OP_NEQUAL:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " ~= ");
                    printf("GPL_OP_~= ");
                    do_next = 1;
                    break;
                case GPL_OP_GTR:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " > ");
                    printf("GPL_OP_> ");
                    do_next = 1;
                    break;
                case GPL_OP_LESS:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " < ");
                    printf("GPL_OP_< ");
                    do_next = 1;
                    break;
                case GPL_OP_GTE:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " >= ");
                    printf("GPL_OP_>= ");
                    do_next = 1;
                    break;
                case GPL_OP_LTE:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " <= ");
                    printf("GPL_OP_<= ");
                    do_next = 1;
                    break;
                case GPL_OP_BAND:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " & ");
                    printf("GPL_OP_& ");
                    do_next = 1;
                    break;
                case GPL_OP_BOR:
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " | ");
                    printf("GPL_OP_| ");
                    do_next = 1;
                    break;
                case GPL_OP_BCLR: {
                    do_next = 1;
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, " | ");
                    printf("GPL_OP_| ");
                    //lua_exit ("BCLR not implemented\n");
                    //opstack[paren_level] = cop;
                    //found_operator = 1;
                    break;
                }
                case GPL_HI_OPEN_PAREN: {
                    /*
                    lua_exit ("Basic Paren structures not implemented\n");
                    if (++paren_level >= MAX_PARENS) {
                        fprintf(stderr, "ERROR: exceeded max paren level!\n");
                        exit(1);
                    }
                    */
                    //printf("SINGLE PAREN\n");
                    ++paren_level;
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, "(");
                    printf("GPL_OP_( ");
                    //printf("updating buf_pos = %d\n", buf_pos);
                    do_next = 1;
                    cval = 0;
                    break;
                }
                case GPL_HI_CLOSE_PAREN: {
                    //lua_exit ("Basic close Paren structures not implemented\n");
                    //cval = accums[paren_level];
                    //if (--paren_level < 0) {
                        //fprintf(stderr, "ERROR: paren level is < 0!\n");
                        //exit(1);
                    //}
                    //printf("CLOSE PAREN, buf_pos = %ld\n", buf_pos);
                    --paren_level;
                    //printf("CLOSEbuf_pos = %d\n", buf_pos);
                    //buf_pos += snprintf(buf + buf_pos, size - buf_pos, ")");
                    printf("GPL_OP_) ");
                    //printf("buf = '%s' ", buf);
                    break;
                }
                case 0xb3:
                    printf(" THIS OP APPEARS TO BE SETTING A PASSIVE'S flag value to something ");
                    cval = gpl_read_complex(gpl);//, buf, &buf_pos, size);
                    break;
                default: {
                    printf(" UNKNOWN OP: 0x%x or 0x%x \n", cop&0x7F, cop);
                    printf("Next Bytes: ");
                    for (int i = 0; i < 16; i++) {
                        gpl_get_byte(gpl, (uint8_t*)&cop);
                        printf("0x%x, ", cop);
                    }
                    printf("\n");
                    exit(1);
                    break;
                }
            }
        }
        if (!do_next) {
            //do_next = ((sol_gpl_preview_byte(0, &next_op) == SOL_SUCCESS && next_op > OPERATOR_OFFSET && next_op <= OPERATOR_LAST));
            do_next = ((gpl_preview_byte16(gpl, &next_op) == EXIT_SUCCESS && next_op > OPERATOR_OFFSET && next_op <= OPERATOR_LAST));
        }
    } while (do_next || paren_level > 0);
        //((sol_gpl_preview_byte(0, &next_op) == SOL_SUCCESS && next_op > OPERATOR_OFFSET && next_op <= OPERATOR_LAST)));
            //|| (paren_level > 0 && next_op == GPL_HI_CLOSE_PAREN)));
    //printf("exiting with buf_pos = %ld, buf = '%s'\n", buf_pos, buf);
    return buf_pos;
}

static void gpl_get_parameters(gpl_data_t *gpl, int16_t amt) {
    for (int16_t i = 0; i < amt; i++) {
        printf(" PARAM %d { ", i);
        gpl_read_number(gpl);
        printf(" } ");
        //printf("parameter[%d] = %s\n", i, lparams.params[i]);
    }
}

static int load_accum(gpl_data_t *gpl) {
    printf("ACCUM <= ");
    gpl_read_number(gpl);
    return EXIT_SUCCESS;
}

int gpl_load_accum(gpl_data_t *gpl) {
    load_accum(gpl);
    printf("\n");
    return EXIT_SUCCESS;
}

int gpl_global_ret(gpl_data_t *gpl) {
    printf("END OF FUNCTION\n");
    return EXIT_SUCCESS;
}

int gpl_nextto(gpl_data_t *gpl) {
    printf("NEXTTO (");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_if(gpl_data_t *gpl) {
    // The paramter is ignored, in the original it probably was the address
    // to jump to if the if was not taken.
    printf(" IF ACCUM == ");
    gpl_get_parameters(gpl, 1);
    //lprintf("if accum == true then -- \"if\"\n");
    printf(" THEN \n");
    gpl->depth++;

    return EXIT_SUCCESS;
}

static int gpl_else(gpl_data_t *gpl) {
    gpl_get_parameters(gpl, 1); // What is the purpose of this parameter?
    printf(" ELSE \n");

    return EXIT_SUCCESS;
}

static int gpl_setrecord(gpl_data_t *gpl) {
    uint16_t depth = 0;
    int16_t header = 0;
    uint16_t element[MAX_SEARCH_STACK];
    uint16_t tmp;
    int32_t obj_name;

    printf("SET RECORD: (");

    gpl_preview_halfword(gpl, &tmp);
    if (tmp > 0x8000) {
        gpl_access_complex(gpl, &header, &depth, element, &obj_name);
        printf(" accum = (");
        gpl_read_number(gpl);//buf, BUF_SIZE);
        printf("), ");
        //smart_write_data(header, depth, element, accum);
        return EXIT_SUCCESS;
    }
    if (tmp == 0) {
        //error("gpl_setrecord: need to implement party...\n");
        gpl_access_complex(gpl, &header, &depth, element, &obj_name);
        printf(" PARTY accum = (");
        gpl_read_number(gpl);//buf, BUF_SIZE);
        printf(")\n");
        //smart_write_data(header, depth, element, accum);
        return EXIT_SUCCESS;
    }
    if (tmp < 0x8000) {
        gpl_access_complex(gpl, &header, &depth, element, &obj_name);
        printf("accum = (");
        gpl_read_number(gpl);
        printf(")");
        //smart_write_data(header, depth, element, accum);
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}

static int gpl_endif(gpl_data_t *gpl) {
    printf(" ENDIF\n");
    gpl->depth--;
    return EXIT_SUCCESS;
}

static int gpl_clear_los(gpl_data_t *gpl) {
    printf(" CLEAR_LOS ( ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_nametonum(gpl_data_t *gpl) {
    printf(" NAMETONUM ( ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_numtoname(gpl_data_t *gpl) {
    printf(" NUMTONAME ( ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_bitsnoop(gpl_data_t *gpl) {
    printf("(( ");
    gpl_get_parameters(gpl, 1);
    printf(" & ");
    gpl_get_parameters(gpl, 1);
    printf(") == SECOND_PARAM");
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_award(gpl_data_t *gpl) {
    printf("AWARD: ");
    gpl_get_parameters(gpl, 2);
    printf("%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_clone(gpl_data_t *gpl) {
    printf(" CLONE ( ");
    gpl_get_parameters(gpl, 6);
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_ifcompare(gpl_data_t *gpl) {
    printf(" IFIS_COMPARE ( ");
    gpl_get_parameters(gpl, 2);
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_orelse(gpl_data_t *gpl) {
    printf(" ORELSE_COMPARE ( ");
    gpl_read_number(gpl);//buf, BUF_SIZE);
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_clearpic(gpl_data_t *gpl) {
    printf("CLEAR PIC\n");

    return EXIT_SUCCESS;
}

static int gpl_continue(gpl_data_t *gpl) {
    printf("PRESENT CONTINUE DIALOG BUTTON\n");

    return EXIT_SUCCESS;
}

static int gpl_log(gpl_data_t *gpl) {
    char *tmp;

    printf("LOG:");
    gff_gpl_read_text(gpl, &tmp);//), "Unable to read immediate string in GPL.");
    printf(" '%s'\n", tmp);
    printf("\n");

    free(tmp);
    return EXIT_SUCCESS;
}

static int gpl_damage(gpl_data_t *gpl) {
    printf("DAMAGE: ");
    gpl_get_parameters(gpl, 2);
    printf("\n");

    return EXIT_SUCCESS;
}

static int gpl_drop(gpl_data_t *gpl) {
    printf("DROP: ");
    gpl_get_parameters(gpl, 3);
    if (gpl->in_retval) {
        printf("\n");
    }

    /*
    lprintf("if %s == %d then \n", lparams.params[2], PARTY);
    lua_depth++;
    lprintf("-- all pary members need to drop %s of %s\n", lparams.params[0], lparams.params[1]);
    lua_depth--;
    lprintf("elseif %s == %d then\n", lparams.params[2], ALL);
    lua_depth++;
    lprintf("--do nothing\n");
    lua_depth--;
    lprintf("else\n");
    lua_depth++;
    lprintf("accum = gpl.drop(%s, %s, %s) -- %s needs to drop %s of %s\n",
       lparams.params[0], lparams.params[1], lparams.params[2],
       lparams.params[0], lparams.params[1], lparams.params[2]);
    lua_depth--;
    lprintf("end -- drop\n");
    */

    return EXIT_SUCCESS;
}

static int gpl_passtime(gpl_data_t *gpl) {
    printf("PASS TIME (");
    gpl_read_number(gpl);
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_exit_gpl(gpl_data_t *gpl) {
    printf("EXIT_GPL ( ");
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_fetch(gpl_data_t *gpl) {
    printf("FETCH:  ");
    gpl_get_parameters(gpl, 2);
    printf("\n");

    return EXIT_SUCCESS;
}

#define EQU_SEARCH (4)
#define LT_SEARCH (5)
#define GT_SEARCH (6)
#define SEARCH_QUAL (0x53)

enum {INT8_ID,
      UINT8_ID,
      INT16_ID,
      UINT16_ID,
      INT32_ID,
      UINT32_ID,
      CHAR_ID,
      CONTAINER_ID,
      NEXT_ID,
      NAME_ID,
      STRUCT_ID
};

static int gpl_search(gpl_data_t *gpl) {
    printf("SEARCH (");

    gpl_read_number(gpl);

    uint8_t low, high, btmp; // should these be 16 bit?
    uint8_t field, type;
    int16_t level = -1;
    //int32_t temp_for;
    gpl_get_byte(gpl, &low);
    gpl_get_byte(gpl, &high);

// multiple searches?
    do {
        gpl_preview_byte(gpl, &btmp);
        if (btmp == SEARCH_QUAL) {
            //gpl_get_byte(gpl, &btmp);
            gpl_get_byte(gpl, &btmp);
        }

        level++;

        //searchField[searchFieldLevel] = GetByte();
        gpl_get_byte(gpl, &field);
        //searchType = GetByte();
        gpl_get_byte(gpl, &type);
        printf (" FIELD ( %d ) TYPE ( %d ) ", field, type);
        if (type >= EQU_SEARCH && type <= GT_SEARCH) {
            switch (type) {
                case INT8_ID:
                case UINT8_ID:
                case INT16_ID:
                case UINT16_ID:
                case INT32_ID:
                case UINT32_ID:
                case NAME_ID:
                    printf(" FOR : ");
                    //temp_for = 
                    gpl_read_number(gpl);
                    break;
            }
        }

        //if (level > 0) {
            // we need to set something from temp_for...
        //} else {
            // search_for is temp_for
        //}
        level--;
        gpl_preview_byte(gpl, &btmp);
        //gpl_get_byte(gpl, &btmp);
    } while (btmp == SEARCH_QUAL);

    return EXIT_SUCCESS;
}

static int gpl_getparty(gpl_data_t *gpl) {
    printf("GETPARTY ( ");
    gpl_read_number(gpl);//buf, BUF_SIZE);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_fight(gpl_data_t *gpl) {
    printf("FIGHT && EXIT_GPL");
    printf("%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_flee(gpl_data_t *gpl) {
    printf("FLEE ( ");
    gpl_read_number(gpl);//buf, BUF_SIZE);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_follow(gpl_data_t *gpl) {
    printf("FOLLOW ( ");
    gpl_get_parameters(gpl, 2);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_getyn(gpl_data_t *gpl) {
    printf("ASK_YES_NO ( ");
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_give(gpl_data_t *gpl) {
    printf("GIVE ( ");
    gpl_get_parameters(gpl, 4);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_go(gpl_data_t *gpl) {
    printf("GO ( ");
    gpl_get_parameters(gpl, 2);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_goxy(gpl_data_t *gpl) {
    printf("GOXY ( ");
    gpl_get_parameters(gpl, 3);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_readorders(gpl_data_t *gpl) {
    printf("READORDERS ( ");
    gpl_read_number(gpl);
    printf(") %s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_setother(gpl_data_t *gpl) {
    printf("SET_OTHER1 = ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_input_string(gpl_data_t *gpl) {
    printf("INPUT STRING( ");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_input_number(gpl_data_t *gpl) {
    printf("INPUT NUMBER (");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_input_money(gpl_data_t *gpl) {
    printf("INPUT MONEY (");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

#define MAXMENU (24)

static int gpl_menu(gpl_data_t *gpl) {
    int items = 0;
    uint8_t b;

    printf("MENU ( name = {");
    gpl_read_number(gpl);//buf, BUF_SIZE);
    printf("} )\n");

    while ((gpl_preview_byte(gpl, &b) == EXIT_SUCCESS && b != 0x4A) && (items <= MAXMENU)) {
        printf("  MENU_ENTRY ( TEXT (");
        gpl_read_number(gpl);
        printf(" ) FUNCTION (");
        //menu = (char*) gpl_global_big_numptr;
        gpl_read_number(gpl); // function
        printf(") IF (");
        gpl_read_number(gpl); // address
        printf(" == 1)) \n");
        items++;
    }
    gpl_get_byte(gpl, &b);  // get rid of the mend...

    //lprintf("gpl.narrate_show() --narrate_wait for input\n");
    printf("MENU_END\n");

    return EXIT_SUCCESS;
}

static int gpl_setthing(gpl_data_t *gpl) {
    // 1st char , second is item index
    printf("SET_THING ( ");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_print_string(gpl_data_t *gpl) {
    printf("PRINT_STRING ( ");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_print_number(gpl_data_t *gpl) {
    printf("PRINT_NUMBER ( ");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_printnl(gpl_data_t *gpl) {
    printf("PRINT_NEWLINE ( ");
    //gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_rand(gpl_data_t *gpl) {
    printf("RAND ( ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_show_pic(gpl_data_t *gpl) {
    printf("SHOW_PIC ( ");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_statroll(gpl_data_t *gpl) {
    printf("STAT_ROLL ( ");
    gpl_get_parameters(gpl, 3);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_string_compare(gpl_data_t *gpl) {
    printf("STRING COMPARE ( ");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_take(gpl_data_t *gpl) {
    printf("TAKE ( ");
    gpl_get_parameters(gpl, 4);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_sound(gpl_data_t *gpl) {
    printf("SOUND ( ");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_zero(gpl_data_t *gpl) {
    printf("EXIT_GPL ( ");
    //gpl_read_number(gpl);//buf, BUF_SIZE);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_long_div_eq(gpl_data_t *gpl) {
    printf("LONG_DIV_EQ ( ");
    gpl_get_parameters(gpl, 2);
    printf(") IE: param0 /= param1 \n");

    return EXIT_SUCCESS;
}

static int gpl_byte_dec(gpl_data_t *gpl) {
    printf("BYTE_DEC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_word_dec(gpl_data_t *gpl) {
    printf("WORD_DEC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_long_dec(gpl_data_t *gpl) {
    printf("LONG_DEC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_byte_inc(gpl_data_t *gpl) {
    printf("BYTE_INC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_word_inc(gpl_data_t *gpl) {
    printf("WORD_INC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_long_inc(gpl_data_t *gpl) {
    printf("LONG_INC ( ");
    gpl_get_parameters(gpl, 1);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_hunt(gpl_data_t *gpl) {
    printf("HUNT ( ");
    gpl_read_number(gpl);//buf, BUF_SIZE);
    printf(") \n");

    return EXIT_SUCCESS;
}

static int gpl_getxy(gpl_data_t *gpl) {
    printf("GETXY ( ");
    gpl_read_number(gpl);
    printf(") SET THE GNAME of 1 = get_X(param0), 2 = get_Y(param0), 3 = get_Z(param0)\n");

    return EXIT_SUCCESS;
}

static int gpl_string_copy(gpl_data_t *gpl) {
    //char buf[BUF_SIZE];
    //size_t pos = 0;

    printf("SET GSTR (");
    gpl_get_parameters(gpl, 1);
    printf(" == GSTR (");
    gpl_get_parameters(gpl, 1);
    printf(")\n");
    //lprintf("var%d = \"%s\"\n", varnum++, (char*)lparams.params[1]);
    /*
    if (strncmp("gpl.get_gstr(", lparams.params[0], 13) == 0) {
        strncpy(buf, lparams.params[0], BUF_SIZE);
        buf[4] = 's';
        buf[strlen(buf) - 1] = '\0';
        lprintf("%s, var%d)\n", buf, varnum - 1);
    } else {
        for (size_t i = 0; i < strlen(lparams.params[0]); i++) {
            if (isdigit(lparams.params[0][i])) {
                pos = i;
                break;
            }
        }
        lprintf("gpl.set_gstr(%d, \"%s\")\n", atoi(lparams.params[0] + pos), lparams.params[1]);
    }
    */
    return EXIT_SUCCESS;
}

static int gpl_pdamage(gpl_data_t *gpl) {
    printf(" PHYSICAL_DAMAGE (");
    gpl_get_parameters(gpl, 2);
    printf(")\n");

    return EXIT_SUCCESS;
}

static int gpl_change_money(gpl_data_t *gpl) {
    printf("CHANGE MONEY: ");
    gpl_read_number(gpl);
    printf("%s", gpl->in_retval ? "" : "\n");

    return EXIT_SUCCESS;
}

static int gpl_toggle_accum(gpl_data_t *gpl) {
    printf("TOGGLE ACCUM()");
    printf("%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_get_status(gpl_data_t *gpl) {
    printf("GET_STATUS (");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_get_los(gpl_data_t *gpl) {
    printf("GET_LOS (");
    gpl_get_parameters(gpl, 3);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_jump(gpl_data_t *gpl) {
    printf("JUMP (");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_call_local(gpl_data_t *gpl) {
    printf("CALL_LOCAL_FUNCTION (");
    gpl_get_parameters(gpl, 1);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_call_global(gpl_data_t *gpl) {
    printf("CALL FUNCTION (ADDR, FILE) (");
    gpl_get_parameters(gpl, 2);
    printf(")\n");
    return EXIT_SUCCESS;
}

static int gpl_local_ret(gpl_data_t *gpl) {
    printf("RETURN FROM LOCAL FUNCTION\n");
    return EXIT_SUCCESS;
}

static int gpl_compare(gpl_data_t *gpl) {
    printf("COMPARE: ");
    load_accum(gpl);
    gpl->depth++;
    printf("\n");
    return EXIT_SUCCESS;
}

static int gpl_load_variable(gpl_data_t *gpl) {
    int8_t extended = 0;
    uint8_t datatype, b;
    uint16_t varnum;

    load_accum(gpl);

    gpl_get_byte(gpl, &datatype);
    datatype &= 0x7F;
    if ((datatype & EXTENDED_VAR) != 0) {
        extended = 1;
        datatype -= EXTENDED_VAR;
    }
    if (datatype < 0x10) { // simple data type
        gpl_get_byte(gpl, &b);
        varnum = b;
        if (extended == 1) {
            varnum *= 0x100;
            gpl_get_byte(gpl, &b);
            varnum += b;
        }
        //load_simple_variable(datatype, varnum, 0);
        gpl_load_simple_variable(gpl, datatype, varnum);
    } else {
        gpl_write_complex_var(gpl);
        //lua_exit("write complex_var not implemented!\n");
    }
    printf("\n");
    return EXIT_SUCCESS;
}

static int gpl_request(gpl_data_t *gpl) {
    printf("REQUEST (");
    gpl_get_parameters(gpl, 4);
    printf(")\n");
    return EXIT_SUCCESS;
}

static int gpl_shop(gpl_data_t *gpl) {
    printf("SHOP: ");
    gpl_read_number(gpl);
    printf("\n");
    return EXIT_SUCCESS;
}

static int gpl_tport(gpl_data_t *gpl) {
    printf("TELEPORT (everything/part) ( ");
    gpl_get_parameters(gpl, 5);
    /*
    if ((param.val[0] != PARTY) && (param.val[0] != IS_POV)) {
        //warn("I need to teleport everything to region %d at (%d, %d) priority: %d, onwindow %d\n",
            //param.val[1], param.val[2], param.val[3], param.val[4], param.val[5]);
        lprintf("gpl.tport_everything( %s, %s, %s, %s, %s)\n",
            lparams.params[1], lparams.params[2], lparams.params[3], lparams.params[4], lparams.params[5]);
    } else {
        //warn("I need to teleport party to region %d at (%d, %d) priority: %d, onwindow %d\n",
            //param.val[1], param.val[2], param.val[3], param.val[4], param.val[5]);
        lprintf("gpl.tport_party( %s, %s, %s, %s) -- param 5: '%s')\n",
            lparams.params[1], lparams.params[2], lparams.params[3], lparams.params[4], lparams.params[5]);
    }
    */
    printf(")\n");
    return EXIT_SUCCESS;
}

static int gpl_cmpend(gpl_data_t *gpl) {
    printf("CMPEND\n");
    gpl->depth--;
    return EXIT_SUCCESS;
}

static int gpl_wait(gpl_data_t *gpl) {
    printf("WAIT (");
    gpl_read_number(gpl);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static int gpl_while(gpl_data_t *gpl) {
    printf("WHILE (");
    gpl_get_parameters(gpl, 1);
    printf(")\n");
    gpl->depth++;
    return EXIT_SUCCESS;
}

static int gpl_wend(gpl_data_t *gpl) {
    printf("WHILE END (");
    gpl_read_number(gpl);
    printf(")\n");
    gpl->depth--;
    return EXIT_SUCCESS;
}

static int gpl_template(gpl_data_t *gpl, const char *cmd, const int amt) {
    printf("%s (%s) (",
            cmd,
            gpl->is_mas ? "GLOBAL" : "LOCAL");
    gpl_get_parameters(gpl, amt);
    printf(")\n");
    return EXIT_SUCCESS;
}

static int gpl_attack_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_ATTACK_TRIGGER", 3);
}

static int gpl_look_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_LOOK_TRIGGER", 3);
}

static int gpl_use_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_USE_TRIGGER", 3);
}

static int gpl_move_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_MOVE_TILE_TRIGGER", 5);
}

static int gpl_door_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_DOOR_TILE_TRIGGER", 5);
}

static int gpl_move_box_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_MOVE_BOX_TRIGGER", 7);
}

static int gpl_door_box_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_DOOR_BOX_TRIGGER", 7);
}

static int gpl_pickup_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_PICKUP_TRIGGER", 3);
}

static int gpl_talk_to_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_TALKTO_TRIGGER", 3);
}

static int gpl_noorders_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_NOORDERS_TRIGGER", 3);
}

static int gpl_usewith_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_USEWITH_TRIGGER", 4);
}

static int gpl_inlos_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_INLOS_TRIGGER", 4);
}

static int gpl_notinlos_trigger(gpl_data_t *gpl) {
    return gpl_template(gpl, "CREATE_NOTINLOS_TRIGGER", 4);
}

static int gpl_type_op_equal(gpl_data_t *gpl, const char *type, const char *op) {
    printf("%s (", type);
    gpl_get_parameters(gpl, 1);
    printf(" %s ", op);
    gpl_get_parameters(gpl, 1);
    printf(")\n");
    return EXIT_SUCCESS;
}

static int gpl_byte_plus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "BYTE", "+=");
}

static int gpl_byte_minus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "BYTE", "-=");
}

static int gpl_byte_times_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "BYTE", "*=");
}

static int gpl_byte_div_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "BYTE", "/=");
}

static int gpl_word_plus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "WORD", "+=");
}

static int gpl_word_minus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "WORD", "-=");
}

static int gpl_word_times_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "WORD", "*=");
}

static int gpl_word_div_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "WORD", "/=");
}

static int gpl_long_plus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "LONG", "+=");
}

static int gpl_long_minus_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "LONG", "-=");
}

static int gpl_long_times_equal(gpl_data_t *gpl) {
    return gpl_type_op_equal(gpl, "LONG", "*=");
}

static int gpl_range(gpl_data_t *gpl) {
    printf("RANGE (");
    gpl_get_parameters(gpl, 2);
    printf(")%s", gpl->in_retval ? "" : "\n");
    return EXIT_SUCCESS;
}

static gpl_command_t gpl_commands[] = {
    { gpl_zero, "gpl zero" }, // 0x0
    { gpl_long_div_eq, "gpl long divide equal" }, // 0x1
    { gpl_byte_dec, "gpl byte dec" }, // 0x2
    { gpl_word_dec, "gpl word dec" }, // 0x3
    { gpl_long_dec, "gpl long dec" }, // 0x4
    { gpl_byte_inc, "gpl byte inc" }, // 0x5
    { gpl_word_inc, "gpl word inc" }, // 0x6
    { gpl_long_inc, "gpl long inc" }, // 0x7
    { gpl_hunt, "gpl hunt" }, // 0x8
    { gpl_getxy, "gpl getxy" }, // 0x9
    { gpl_string_copy, "gpl string copy" }, // 0xA
    { gpl_pdamage, "gpl p damage" }, // 0xB
    { gpl_change_money, "gpl changemoney" }, // 0xC
    { gpl_unknown, "gpl setvar" }, // 0xD
    { gpl_toggle_accum, "gpl toggle accum" }, // 0xE
    { gpl_get_status, "gpl getstatus" }, // 0xF
    { gpl_get_los, "gpl getlos" }, // 0x10
    { gpl_long_times_equal, "gpl long times equal" }, // 0x11
    { gpl_jump, "gpl jump" }, // 0x12
    { gpl_call_local, "gpl local sub" }, // 0x13
    { gpl_call_global, "gpl global sub" }, // 0x14
    { gpl_local_ret, "gpl local ret" }, // 0x15
    { gpl_load_variable, "gpl load variable" }, // 0x16
    { gpl_compare, "gpl compare" }, // 0x17
    { gpl_load_accum, "gpl load accum" }, // 0x18
    { gpl_global_ret, "gpl global ret" }, // 0x19
    { gpl_nextto, "gpl nextto" }, // 0x1A
    { gpl_inlos_trigger, "gpl inlostrigger" }, // 0x1B
    { gpl_notinlos_trigger, "gpl notinlostrigger" }, // 0x1C
    { gpl_clear_los, "gpl clear los" }, // 0x1D
    { gpl_nametonum, "gpl nametonum" }, // 0x1E
    { gpl_numtoname, "gpl numtoname" }, // 0x1F
    { gpl_bitsnoop, "gpl bitsnoop" }, // 0x20
    { gpl_award, "gpl award" }, // 0x21
    { gpl_request, "gpl request" }, // 0x22
    { gpl_unknown, "gpl source trace" }, // 0x23
    { gpl_shop, "gpl shop" }, // 0x24
    { gpl_clone, "gpl clone" }, // 0x25
    { gpl_unknown, "gpl default" }, // 0x26
    { gpl_ifcompare, "gpl ifcompare" }, // 0x27
    { gpl_unknown, "gpl trace var" }, // 0x28
    { gpl_orelse, "gpl orelse" }, // 0x29
    { gpl_clearpic, "gpl clearpic" }, // 0x2A
    { gpl_continue, "gpl continue" }, // 0x2B
    { gpl_log, "gpl log" }, // 0x2C
    { gpl_damage, "gpl damage" }, // 0x2D
    { gpl_unknown, "gpl source line num" }, // 0x2E
    { gpl_drop, "gpl drop" }, // 0x2F
    { gpl_passtime, "gpl passtime" }, // 0x30
    { gpl_exit_gpl, "gpl exit gpl" }, // 0x31
    { gpl_fetch, "gpl fetch" }, // 0x32
    { gpl_search, "gpl search" }, // 0x33
    { gpl_getparty, "gpl getparty" }, // 0x34
    { gpl_fight, "gpl fight" }, // 0x35
    { gpl_flee, "gpl flee" }, // 0x36
    { gpl_follow, "gpl follow" }, // 0x37
    { gpl_getyn, "gpl getyn" }, // 0x38
    { gpl_give, "gpl give" }, // 0x39
    { gpl_go, "gpl go" }, // 0x3A
    { gpl_unknown, "gpl input bignum" }, // 0x3B
    { gpl_goxy, "gpl goxy" }, // 0x3C
    { gpl_readorders, "gpl readorders" }, // 0x3D
    { gpl_if, "gpl if" }, // 0x3E
    { gpl_else, "gpl else" }, // 0x3F
    { gpl_setrecord, "gpl setrecord" }, // 0x40
    { gpl_setother, "gpl setother" }, // 0x41
    { gpl_input_string, "gpl input string" }, // 0x42
    { gpl_input_number, "gpl input number" }, // 0x43
    { gpl_input_money, "gpl input money" }, // 0x44
    { gpl_unknown, "gpl joinparty" }, // 0x45
    { gpl_unknown, "gpl leaveparty" }, // 0x46
    { gpl_unknown, "gpl lockdoor" }, // 0x47
    { gpl_menu, "gpl menu" }, // 0x48
    { gpl_setthing, "gpl setthing" }, // 0x49
    { gpl_unknown, "gpl default" }, // 0x4A
    { gpl_unknown, "gpl local sub trace" }, // 0x4B
    { gpl_unknown, "gpl default" }, // 0x4C
    { gpl_unknown, "gpl default" }, // 0x4D
    { gpl_unknown, "gpl default" }, // 0x4E
    { gpl_print_string, "gpl print string" }, // 0x4F
    { gpl_print_number, "gpl print number" }, // 0x50
    { gpl_printnl, "gpl printnl" }, // 0x51
    { gpl_rand, "gpl rand" }, // 0x52
    { gpl_unknown, "gpl default" }, // 0x53
    { gpl_show_pic, "gpl showpic" }, // 0x54
    { gpl_unknown, "gpl default" }, // 0x55
    { gpl_unknown, "gpl default" }, // 0x56
    { gpl_unknown, "gpl default" }, // 0x57
    { gpl_unknown, "gpl skillroll" }, // 0x58
    { gpl_statroll, "gpl statroll" }, // 0x59
    { gpl_string_compare, "gpl string compare" }, // 0x5A
    { gpl_unknown, "gpl match string" }, // 0x5B
    { gpl_take, "gpl take" }, // 0x5C
    { gpl_sound, "gpl sound" }, // 0x5D
    { gpl_tport, "gpl tport" }, // 0x5E
    { gpl_unknown, "gpl music" }, // 0x5F
    { gpl_unknown, "gpl default" }, // 0x60
    { gpl_cmpend, "gpl cmpend" }, // 0x61
    { gpl_wait, "gpl wait" }, // 0x62
    { gpl_while, "gpl while" }, // 0x63
    { gpl_wend, "gpl wend" }, // 0x64
    { gpl_attack_trigger, "gpl attacktrigger" }, // 0x65
    { gpl_look_trigger, "gpl looktrigger" }, // 0x66
    { gpl_endif, "gpl endif" }, // 0x67
    { gpl_move_trigger, "gpl move tiletrigger" }, // 0x68
    { gpl_door_trigger, "gpl door tiletrigger" }, // 0x69
    { gpl_move_box_trigger, "gpl move boxtrigger" }, // 0x6A
    { gpl_door_box_trigger, "gpl door boxtrigger" }, // 0x6B
    { gpl_pickup_trigger, "gpl pickup itemtrigger" }, // 0x6C
    { gpl_use_trigger, "gpl usetrigger" }, // 0x6D
    { gpl_talk_to_trigger, "gpl talktotrigger" }, // 0x6E
    { gpl_noorders_trigger, "gpl noorderstrigger" }, // 0x6F
    { gpl_usewith_trigger, "gpl usewithtrigger" }, // 0x70
    { gpl_unknown, "gpl default" }, // 0x71
    { gpl_unknown, "gpl default" }, // 0x72
    { gpl_unknown, "gpl default" }, // 0x73
    { gpl_unknown, "gpl default" }, // 0x74
    { gpl_unknown, "gpl default" }, // 0x75
    { gpl_byte_plus_equal, "gpl byte plus equal" }, // 0x76
    { gpl_byte_minus_equal, "gpl byte minus equal" }, // 0x77
    { gpl_byte_times_equal, "gpl byte times equal" }, // 0x78
    { gpl_byte_div_equal, "gpl byte divide equal" }, // 0x79
    { gpl_word_plus_equal, "gpl word plus equal" }, // 0x7A
    { gpl_word_minus_equal, "gpl word minus equal" }, // 0x7B
    { gpl_word_times_equal, "gpl word times equal" }, // 0x7C
    { gpl_word_div_equal, "gpl word divide equal" }, // 0x7D
    { gpl_long_plus_equal, "gpl long plus equal" }, // 0x7E
    { gpl_long_minus_equal, "gpl long minus equal" }, // 0x7F
    { gpl_range, "gpl get range" }, // 0x80
};

static int gpl_execute_func(gpl_data_t *gpl, uint8_t cmd) {
    //printf("CMD: %d\n", cmd); fflush(stdout);
    return (*gpl_commands[cmd].func)(gpl);
    //(*gpl_lua_operations[command].func)();
}

extern int gff_gpl_parse(uint8_t *gpl, const size_t len, gff_gpl_transformer_t *ggt, uint8_t is_mas) {
    gpl_data_t gd;

    memset(&gd, 0x0, sizeof(gpl_data_t));

    gd.is_mas = is_mas;
    gd.gpl = gpl;
    gd.end = gpl + len;
    gd.ggt = ggt;
    //char **scripts = is_mas ? mas_scripts : gpl_scripts;
    //size_t size = is_mas ? mas_max : gpl_max;
    //char func[BUF_SIZE];

    //debug("execute_script(" PRI_SIZET ", " PRI_SIZET ", %d)\n", file, addr, is_mas);
    //snprintf(func, BUF_SIZE, "func" PRI_SIZET "", addr);

    //if (file < 0 || file >= size) { return EXIT_FAILURE; }

    //if (scripts[file] == NULL) {
    /*
    unsigned char *data_ptr;
    if (!in_func) {
        //lprintf("function func%d (accum) -- address %ld\n", funcnum++, ((size_t)gpl_get_data_ptr()) - gpl_lua_start_ptr);
        sol_gpl_get_data_ptr(&data_ptr);
        cfunc_num = ((size_t)data_ptr) - gpl_lua_start_ptr;
        lprintf("function func%ld ()\n", cfunc_num);
        //lprintf("function %c%ldfunc%ld ()\n", 
            //is_mas ? 'm' : 'g',
            //script_id, ((size_t)gpl_get_data_ptr()) - gpl_lua_start_ptr);
        lua_depth++;
        //lprintf("print(\"I'm in func%d\")\n", cfunc_num);
        //lprintf("gpl.debug()\n");
        in_func = 1;
        compare_level = 0;
        compare_start = 1;
        //lprintf("gpl.debug(\"func%ld\")\n", cfunc_num);
    } else {
        print_label();
    }   
    */
    uint8_t command;
    do {
        gpl_get_byte(&gd, &command);
        //printf("command = 0x%x\n", command);
    } while (gd.gpl < gd.end && gpl_execute_func(&gd, command) == EXIT_SUCCESS);

    if (gd.gpl < gd.end) {
        printf("FAILED TO PARSE COMMAND: 0x%x\n", command);
    }
    //if (!gpl_lua_operations[command].func) {
        //printf("Lua code so far:\n%s\n", lua_buf);
        //printf("Unimplemented command = 0x%2x\n", command);
        //exit(1);
        //return 0;
    //}
    //printf("print_cmd: command = 0x%x, '%s'\n", command, gpl_lua_operations[command].name);
    //(*gpl_lua_operations[command].func)();
    return 1;

    /*
    gff_gpl_lua_print(script_id, is_mas, &script_len, &script_ptr);
    script[script_id] = malloc(sizeof(char) * (script_len + 1)); // (A)
    strncpy(script[script_id], script_ptr, script_len);
    script[script_id][script_len] = '\0'; // Okay because of (A)
    sprintf(buf, "gpl/%d-%s.lua", script_id,
        is_mas ? "mas" : "gpl");
    return write_lua(buf, script[script_id], script_len);
    */
    //}

    //if (scripts[file] == NULL) { return EXIT_FAILURE; }

    /*
    gff_gpl_push_context();
    if (luaL_dostring(clua, scripts[file])) {
        error("Error: unable to load %s script " PRI_SIZET ":" PRI_SIZET "\n",
            is_mas ? "MAS" : "GPL",
            file, addr);
        error("error: %s\n", lua_tostring(clua, -1));
        gff_gpl_pop_context();
        return EXIT_FAILURE;
    }
    lua_getglobal(clua, func);
    if (lua_pcall(clua, 0, 0, 0)) {
        error("error running function: '%s' in %s file " PRI_SIZET ", addr " PRI_SIZET "\n", func,
            is_mas ? "MAS" : "GPL",
            file, addr);
        error("error: %s\n", lua_tostring(clua, -1));
        return EXIT_FAILURE;
    }

    debug("exiting execute_script(" PRI_SIZET ", " PRI_SIZET ", %d)\n", file, addr, is_mas);
    gff_gpl_pop_context();

    return EXIT_SUCCESS;
    */
    return EXIT_FAILURE;
}

static void gpl_retval(gpl_data_t *gpl, uint8_t cmd) {
    gpl->in_retval = 1;
    //This is a bit of a precarious situation because we are inside the calculation
    //of an accum. So only certain operations will work here.
    switch(cmd) {
        case 0x0f:// get status
        case 0x10:// get los
        case 0x1e:// nametonum
        case 0x1f:// numtoname
        case 0x20:// bitsnoop
        case 0x22:// gplrequest
        case 0x25:// gplclone
        case 0x2f:// drop
        case 0x33:// search
        case 0x34:// get_party
        case 0x38:// yes_no
        case 0x39:// give
        case 0x3d:// readorders
        case 0x41:// setother
        case 0x49:// setthing
        case 0x52:// rand
        case 0x5a:// string compare
        case 0x5c:// take
        case 0x59:// statroll
        case 0x80:// range
            // The above look good.
            // These are okay
            break;
        case 0x1a:// global return?
            //error("false -- global return, this is probably wrong.\n");
            break;
        default:
            error ("unrecognized command in a RETVAL: 0x%x\n", cmd);
            exit(1);
            
    }
    //printf("RETVAL cmd = 0x%x (%s)\n", cmd, gpl_lua_operations[cmd].name);
    //(*gpl_lua_operations[cmd].func)();
    if ((*gpl_commands[cmd].func)(gpl) == EXIT_FAILURE) {
        printf("FAILURE IN NESTED RETVAL--can't exit cleanly....\n");
        printf("RETVAL: 0x%x\n", cmd);fflush(stdout);
        exit(1);
    }
    gpl->in_retval = 0;
    //print_vars(0);
}

