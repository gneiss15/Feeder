#!/usr/bin/env bash

APPDIR="$(dirname -- "$(readlink -f -- "${0}")" )"

cd ${APPDIR}

DoIt1()
 {
  # Redirect Output
  {
  echo "#define CD_( htmlType, typ, name, defValue, extraHtmlText, ... ) :#name:[#htmlType,extraHtmlText],"
  echo "#define CD( htmlType, typ, name, defValue, ... ) CD_( htmlType, typ, name, defValue, ##__VA_ARGS__, \"\" )"
  echo "#define MkStr( c ) #c"
  echo "#define C2( c ) MkStr( T ## c )"
  echo "#define C( c ) C2( c )"
  echo "#define CD_Div :C( __COUNTER__ ):[\"d\",\"\"],"

  echo "#include \"../Feeder$1.inc.h\""
  } >t.h
  
  echo "$1Entries.json:"
  echo -en "{$(gcc -E t.h | sed -e '/^:/!d;s/^://g' | sed -ze 's/\n//g')\"TLast\":[\"d\",\"\"]}\n"
  echo -en "{$(gcc -E t.h | sed -e '/^:/!d;s/^://g' | sed -ze 's/\n//g')\"TLast\":[\"d\",\"\"]}\n" >../data/$1Entries.json
  rm t.h
 }

DoIt2()
 {
  # Redirect Output
  {
  echo "#define CD( htmlType, typ, name, defValue, ... ) :#name:defValue,"
  echo "#define CD_Div"

  echo "#define WIFI_OFF    0"
  echo "#define WIFI_STA    1"
  echo "#define WIFI_AP     2"
  echo "#define WIFI_AP_STA 3"

  echo "#include \"../Feeder$1.inc.h\""
  } >t.h
  
  echo "$1.json:"
  echo -en "{\n$(gcc -E t.h | sed -e '/^:/!d;s/^:/ /g' | sed '$ s/.$//' )\n}\n"
  echo -en "{\n$(gcc -E t.h | sed -e '/^:/!d;s/^:/ /g' | sed '$ s/.$//' )\n}\n" >../data/$1.json
  rm t.h
 }

DoIt()
 {
  DoIt1 $1
  DoIt2 $1
 }

DoIt Config


