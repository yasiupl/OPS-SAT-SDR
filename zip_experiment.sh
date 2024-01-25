#!/bin/bash
USER=exp266
NAME=yasiu
version=$(git describe --tags)
if [[ $(git diff --stat) != '' ]]; then
version="${version}-dirty"
else
version="${version}-clean"
fi
package_name="$USER"_"$version"_"$NAME"_soft.zip
cd src/
zip -r ../delivery/$package_name home/*
echo "Finished preparing ZIP file"
cd ..
ls -lart delivery/$package_name