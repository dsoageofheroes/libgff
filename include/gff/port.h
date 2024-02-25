// Collection of defs/function to support porting different OS.
// Really just Windoze being wacky.

#ifndef GFF_PORT_H
#define GFF_PORT_H

#ifdef _WIN32                         
#  ifdef _WIN64          
#    define PRI_SIZET "%I64u"
#  else                  
#    define PRI_SIZET "%I32u"                                                                  
#  endif                        
#else                   
#  define PRI_SIZET "%zu"              
#endif

#endif
