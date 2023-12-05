#!/usr/bin/env sh

id=$RANDOM

mkdir -p output

sed -i -e "s/output_path =.*/duration_sec = output\/sidlock_$id_/"  config.ini

cat config.ini

./exp202 config.ini

./renderfall.sh output/* test

