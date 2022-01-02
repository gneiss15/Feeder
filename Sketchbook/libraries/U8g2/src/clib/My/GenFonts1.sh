#!/bin/bash

SrcFile=u8g2_fonts.c
DestDir=My/Fonts1/

SCRIPTDIR="$(dirname -- "$(readlink -f -- "${0}")" )"
. ${SCRIPTDIR}/_GenFontsCommon.sh

