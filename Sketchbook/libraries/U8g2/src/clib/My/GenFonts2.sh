#!/bin/bash

SrcFile=u8g2_fonts2.c
DestDir=My/Fonts2/

SCRIPTDIR="$(dirname -- "$(readlink -f -- "${0}")" )"

cp ${SCRIPTDIR}/u8g2_fonts2.c.tmp ${SCRIPTDIR}/../u8g2_fonts2.c
trap "rm ${SCRIPTDIR}/../u8g2_fonts2.c" EXIT

. ${SCRIPTDIR}/_GenFontsCommon.sh

