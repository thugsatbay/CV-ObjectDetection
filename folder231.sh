#!/bin/bash
DIR_SYMBOL='/'
STAR_SYMBOL='*';
UNDER_SYMBOL='_';
ROOT_DIR=$1$DIR_SYMBOL$STAR_SYMBOL;
DEST_DIR=$2$DIR_SYMBOL;
CONSTANT='231';
DUMP="";
JPG='.jpg';
if [ $3 == 1 ]
then
	if [ $4 == 0 ]
	then
		rm -rf $2;
		rm -rf "./test231";
		rm -rf "./ttdump"
		rm -rf "./dump";
		mkdir "./dump"
		DUMP="./dump"
	fi
	if [ $4 == 1 ]
	then
		rm -rf $2;
		rm -rf "./ttdump/";
		mkdir "./ttdump"
		DUMP="./ttdump"
	fi
	mkdir $DEST_DIR;
	for file in $ROOT_DIR;
	do
		FOLDER_NAME=$(echo "$file" | cut -d'/' -f 3);
		mkdir $DUMP$DIR_SYMBOL$FOLDER_NAME
		FOLDER_NAME=$DEST_DIR$FOLDER_NAME;
		echo "Making Directory "$FOLDER_NAME$" ..."
		mkdir $FOLDER_NAME;
	done
fi


#convert the files
#if false
#then
#for folder in $ROOT_DIR;
#do
#	echo "Inside Folder "$folder" ...";
#	for file in $folder$DIR_SYMBOL$STAR_SYMBOL;
#	do
		#echo "Converting file "$file" ---";
		#OUTPUT_FILE=${file%.*}$UNDER_SYMBOL$CONSTANT$JPG;
		#convert -resize 231X231 $file $OUTPUT_FILE;
		#mv $OUTPUT_FILE $DEST_DIR$(echo "$file" | cut -d'/' -f 3)$DIR_SYMBOL;

#	done
#done
#fi