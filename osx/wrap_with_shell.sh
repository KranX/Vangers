#!/usr/bin/env bash
binary=$1
wrapper=$2
mv $binary $binary.bin
cp $wrapper $binary
chmod +x $binary
