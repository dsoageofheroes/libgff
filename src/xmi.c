#include <stdlib.h>
#include <string.h>
#include "gff/xmi.h"
#include "xmi2mid.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"

extern unsigned char* gff_xmi_to_midi_type(const unsigned char *xmi_data, const unsigned int xmi_len, unsigned int *midi_len,
    const int type) {
    unsigned char *buf = NULL, *midi_data = NULL;

    _WM_xmi2midi(xmi_data, xmi_len, (unsigned char**)&(buf), midi_len, type );

    if (!buf) { return NULL; }

    midi_data = malloc(*midi_len);
    if (!midi_data) { return NULL; }

    memcpy(midi_data, buf, *midi_len);
    free(buf);

    return midi_data;
}

extern unsigned char* gff_xmi_to_midi(const unsigned char *xmi_data, const unsigned int xmi_len, unsigned int *midi_len) {
    // I think MT32 to GS is the best, followed by MT32 to GM.
    // No conversion and GS127 to GS are not authentic.

    //_WM_xmi2midi(xmi_data, xmi_len, (unsigned char**)&(buf), midi_len, XMIDI_CONVERT_NOCONVERSION);
    //_WM_xmi2midi(xmi_data, xmi_len, (unsigned char**)&(buf), midi_len, XMIDI_CONVERT_MT32_TO_GM );
    //_WM_xmi2midi(xmi_data, xmi_len, (unsigned char**)&(buf), midi_len, XMIDI_CONVERT_MT32_TO_GS );
    //_WM_xmi2midi(xmi_data, xmi_len, (unsigned char**)&(buf), midi_len, XMIDI_CONVERT_GS127_TO_GS );
    return gff_xmi_to_midi_type(xmi_data, xmi_len, midi_len, XMIDI_CONVERT_MT32_TO_GS);
}

static int load_seq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type, int conv) {
    unsigned char *midi = NULL;
    unsigned int midi_len;
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, type, res_id)) {
        printf("Unable to read chunk header for GSEQ\n");
        return EXIT_FAILURE;
    }

    *data = malloc(chunk.length);
    if (gff_read_chunk(gff, &chunk, *data, chunk.length) != chunk.length) {
        printf("Unable to read chunk for SEQ\n");
        return EXIT_FAILURE;
    }

    printf("chunk.length = %d\n", chunk.length);
    if (!(midi = gff_xmi_to_midi_type((unsigned char*)*data, chunk.length, &midi_len, conv))) {
        fprintf(stderr, "Unable to convert to midi!\n");
        printf("midi_len = %d\n", midi_len);
        goto convert_failure;
    }

    free(*data);
    *data = (uint8_t*)midi;
    *len = midi_len;

    return EXIT_SUCCESS;
convert_failure:
    free(*data);
    *data = NULL;
    return EXIT_FAILURE;
}

extern int gff_load_gseq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len) {
    return load_seq(gff, res_id, data, len, GFF_GSEQ, XMIDI_CONVERT_MT32_TO_GS);
}

extern int gff_load_pseq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len) {
    return load_seq(gff, res_id, data, len, GFF_PSEQ, XMIDI_CONVERT_MT32_TO_GS);
}

extern int gff_load_cseq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len) {
    return load_seq(gff, res_id, data, len, GFF_CSEQ, XMIDI_CONVERT_MT32_TO_GS);
}

extern int gff_load_lseq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len) {
    return load_seq(gff, res_id, data, len, GFF_LSEQ, XMIDI_CONVERT_MT32_TO_GS);
}

extern int gff_load_gseq_type(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type) {
    return load_seq(gff, res_id, data, len, GFF_GSEQ, type);
}

extern int gff_load_pseq_type(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type) {
    return load_seq(gff, res_id, data, len, GFF_PSEQ, type);
}

extern int gff_load_cseq_type(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type) {
    return load_seq(gff, res_id, data, len, GFF_CSEQ, type);
}

extern int gff_load_lseq_type(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type) {
    return load_seq(gff, res_id, data, len, GFF_LSEQ, type);
}
