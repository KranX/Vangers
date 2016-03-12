#!/bin/bash

dd if=$1 bs=1c skip=18 of=$2_clean
printf "0: %.4x" $3 | sed -E 's/0: (..)(..)/0: \2\1/' | xxd -r -g0 > $2
printf "0: %.4x" $4 | sed -E 's/0: (..)(..)/0: \2\1/' | xxd -r -g0 >> $2
cat $2_clean >> $2
rm $2_clean