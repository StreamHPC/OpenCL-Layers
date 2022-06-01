#!/bin/sh

awk -f parse.awk cl-avl.xml > res.cpp
awk -f enums.awk cl-avl.xml >> res.cpp
awk -f bitfield.awk cl-avl.xml >> res.cpp
awk -f lists.awk cl-avl.xml >> res.cpp

cat list_violation.cpp >> res.cpp
cat struct_violation.cpp >> res.cpp