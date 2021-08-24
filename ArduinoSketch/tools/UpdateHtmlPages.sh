#!/usr/bin/env bash

APPDIR="$(dirname -- "$(readlink -f -- "${0}")" )"

cd ${APPDIR}

DCount=0
# Redirect Output
{
ProcessLines()
 {
  if [ "$1" == "d" ]; then
    DCount=$((DCount+1))
    echo -n "\"T$DCount\":\"$1\","
   else
    echo -n "\"$2\":\"$1\","
  fi
 }

echo -n "{"
sed -e '/^\w*CD/!d;s/[^(]*[( ]*\([^)]*\).*/\1/;s/\([^, ]*\)[, ]*[^,]*[, ]*\([^, ]*\).*/\1,\2;/;s/,;/;/;s/;//' ../FeederConfig.inc.h |
while IFS=, read -r a b; do
  ProcessLines $a $b
done
echo "\"TLast\":\"d\"}"
} >../data/ConfigEnties.json

