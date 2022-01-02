#!/bin/bash

# u8g2_fonts.c:
#/*
#  Fontname: u8glib_4
#  Copyright: public domain
#  Glyphs: 224/224
#  BBX Build Mode: 0
#*/
#const uint8_t u8g2_font_u8glib_4_tf[1838] U8G2_FONT_SECTION("u8g2_font_u8glib_4_tf") = 
#  "\340\0\2\2\4\3\2\4\5\10\7\0\377\4\377\5\377\0\346\1\334\7\21 \5\0Q\2!\6AQ"

# u8g2_fonts2.c:
#/*
#  Fontname: u8glib_4
#  Copyright: public domain
#  Glyphs: 224/224
#  BBX Build Mode: 0
#*/
#const unsigned char u8g2_font_u8glib_4_tf[1838] = 
#  "\340\0\2\2\4\3\2\4\5\10\7\0\377\4\377\5\377\0\346\1\334\7\21 \5\0Q\2!\6AQ"

if [ ! -f "${DestDir}" ]; then
  mkdir -p "${DestDir}"
fi

OUTFILE="${DestDir}GenFont.cpp"

cat << EOF >${OUTFILE}
#include "../../${SrcFile}"
#include <stdio.h>
#include <string>

using namespace std;

typedef struct
 {
  string Name;
  uint8_t const * Adr;
  size_t size;
 }
 TFontInfos;

#define TableSize( x ) ( sizeof( (x) )/sizeof( *(x) ) )

TFontInfos FontInfos[] = {
EOF

for font in $(sed -e '/^const \(uint8_t\|unsigned char\) u8g2_font_/!d;s/const \(uint8_t\|unsigned char\) //g;s/\[.*//g' ${SrcFile})
 do
  echo "{ \"${font}\", ${font}, TableSize( ${font} ) }, " >>${OUTFILE}
 done

cat << EOF >>${OUTFILE}
};

void CreateFontFile( TFontInfos const & fontInfo )
 {
  FILE * f = fopen( ( string( "${DestDir}" ) + fontInfo.Name + ".tr" ).c_str(), "w" );
  fprintf( f, "/* tslint:disable */\n\n" );
  fprintf( f, "export const %s[%lu]=\n", fontInfo.Name.c_str(), fontInfo.size );
  for( int i = 0;  i < fontInfo.size;  ++i )
   {
    fprintf( f, "%d,", fontInfo.Adr[ i ] );
    if( i % 8 == 0 )
      fprintf( f, "\n" );
   }
  fprintf( f, "\n];" );
  fclose( f );
 }

int main( int argc, const char* argv[] )
 {
  for( int i = 0;  i < TableSize( FontInfos );  ++i )
    CreateFontFile( FontInfos[ i ] );
 }
EOF

g++ -o ${DestDir}GenFont $OUTFILE
${DestDir}GenFont
rm ${DestDir}GenFont $OUTFILE

