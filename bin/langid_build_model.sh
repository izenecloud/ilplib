#!/bin/bash
# this script builds two binary models: encoding.bin and language.bin

# train profiles for encoding.bin
# the range of N-grams
n=2
# the length of profile
l=2000

PROJECT_HOME="`dirname $0`/.."
BIN_DIR="$PROJECT_HOME/bin"
TRAIN_DIR="$PROJECT_HOME/db/langid/data/train"
OUTPUT_DIR="$BIN_DIR/profile"
PROFILE_EXE="$BIN_DIR/langid_create_profile"

if test -d $OUTPUT_DIR
then
    echo $OUTPUT_DIR exists.
else
    echo $OUTPUT_DIR not exists, create it.
    mkdir $OUTPUT_DIR
fi

count=0
for file in `find ${TRAIN_DIR}/`
do
	if test -f $file
	then
        INPUT_NAME="`basename $file`"

		echo === training file $INPUT_NAME

        OUTPUT_FILE="$OUTPUT_DIR/$INPUT_NAME.profile"

        $PROFILE_EXE $file $OUTPUT_FILE -n $n -l $l

        let ++count
        echo
	fi
done

echo "=== $count profiles are created."

# create encoding.bin
ENCODING_EXE="$BIN_DIR/langid_build_encoding_model"
$ENCODING_EXE $PROJECT_HOME/db/langid/config/encoding.config encoding.bin
echo "=== encoding.bin is created."

# create language.bin
LANGUAGE_EXE="$BIN_DIR/langid_build_language_model"
$LANGUAGE_EXE language.bin
echo "=== language.bin is created."
