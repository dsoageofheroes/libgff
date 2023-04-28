#include "gfftypes.h"

const char* gff_type_to_str(uint32_t type) {
    switch(type) {
        case GFF_FORM: return "GFF internal chunk";
        case GFF_GFFI: return "GFFI internal chunk";
        case GFF_GFRE: return "GFF internal chunk";
        case GFF_GFFOC: return "GFF internal chunk";
        case GFF_ACCL: return "Accelertor resource";
        case GFF_ADV : return "AIL Audio Driver";
        case GFF_APFM: return "Application frame resource";
        case GFF_BMAP: return "3DX Bump map (GIL bitmap table, 1 bitmap)";
        case GFF_BMP : return "Bitmap (GIL bitmap table, 1+ bitmaps)";
        case GFF_BVOC: return "Background play samples";
        case GFF_BUTN: return "Button resource";
        case GFF_CMAP: return "Color map table";
        case GFF_DADV: return "AIL and .COM drivers (MEL version 1.x only)";
        case GFF_DATA: return "DATA?";
        case GFF_DBOX: return "Dialog box resource";
        case GFF_DRV : return "Run-time loadable code/data module";
        case GFF_EDAT: return "3DX Environment database";
        case GFF_EBOX: return "Edit box resource";
        case GFF_ETAB: return "Object entry table";
        case GFF_FONT: return "Font (GIL font)";
        case GFF_FVOC: return "Foreground play samples";
        case GFF_GMAP: return "Region map flags";
        case GFF_GPL : return "Compiled GPL files";
        case GFF_GPLX: return "GPL index file (GPLSHELL)";
        case GFF_ICON: return "Icon resource (GIL bitmap table, 1-4 bitmaps)";
        case GFF_MAS : return "Compiled MAS (GPL master) files";
        case GFF_MENU: return "Menu resource";
        case GFF_MONR: return "monsters by region ids and level";
        case GFF_MGFFL: return "Global timbre library";
        case GFF_MSEQ: return "XMIDI sequence files (.XMI)";
        case GFF_OMAP: return "Opacity map (GIL bitmap table, 1 bitmap)";
        case GFF_PAL : return "VGA 256 color palette";
        case GFF_POBJ: return "PolyMesh object database";
        case GFF_RMAP: return "Region tile map";
        case GFF_SCMD: return "Animation script command table";
        case GFF_SBAR: return "Scroll-bar resource";
        case GFF_SINF: return "Sound card info";
        case GFF_SJMP: return "OENGINE animation script jump table";
        case GFF_TEXT: return "Text resource";
        case GFF_TILE: return "Tile graphic (GIL bitmap table, 1 bitmap)";
        case GFF_TMAP: return "Texture map (GIL bitmap table, 1 bitmap)";
        case GFF_TXRF: return "Texture map (GIL bitmap table, 1 bitmap)";
        case GFF_WIND: return "Window resource";
        case GFF_ACF: return "Cinematic Binary Script File";
        case GFF_BMA: return "Cinematic Binary File";
        case  GFF_CBMP: return "Color Bit Map";
        case GFF_WALL: return "WALL?";
        case GFF_OJFF: return "Object data?";
        case GFF_RDFF: return "RDFF?";
        case  GFF_FNFO: return "FNFO: Appears to have the object data for the game.";
        case  GFF_RDAT: return "Names?";
        case GFF_IT1R: return "IT1R";
        case GFF_NAME: return "NAME";
        case GFF_ALL: return "ALL";
        case  GFF_VECT: return "VECT?   rotate vector?";

// sound!
        case GFF_MERR: return "text error file";
        case GFF_PSEQ: return "PCSPKR";
        case GFF_FSEQ: return "FM";
        case GFF_LSEQ: return "LAPC";
        case GFF_GSEQ: return "GENERAL MIDI";
        case GFF_CSEQ: return "Clock Sequences";

// Transition art?
        case GFF_CMAT: return "CMAT?";
        case GFF_CPAL: return "CPAL?";
        case GFF_PLYL: return "PLYL?";
        case GFF_VPLY: return "VPLY?";

        case GFF_SPIN: return "Spell text.";

        case GFF_PORT: return "Portrait";

        case GFF_SPST: return "spell list bit-mask";
        case GFF_PSST: return "psionc list bytes";
        case GFF_CHAR: return "saved character slot.";
        case GFF_PSIN: return "psionic and sphere selection";
        case GFF_CACT: return "valid char id (0 if entry is invalid.)";

// SAVE Entries:
        case GFF_STXT: return "name of the save file entry";
        case GFF_ETME: return "copyright notice";
        case GFF_SAVE: return "save entries...";
        case GFF_POS: return "entry pos (not part of original engine.)";
        case GFF_ROBJ: return "save entry region (not part of original engine.)";
        case GFF_TRIG: return "save entry triggers (not part of original engine.)";
        case GFF_GDAT: return "save entry GPL data (not part of original engine.)";
        case GFF_PLAY: return "save entry player data (not part of original engine.)";
        case GFF_RENT: return "save entry entity data (not part of original engine.)";

    }
    return "Unknown type";
}
