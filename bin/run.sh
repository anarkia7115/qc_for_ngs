#!/bin/bash

set -e

BASEDIR=$(dirname $0)
cd $BASEDIR

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <input> <s3Path>" >&2
  exit 1
fi

input=`echo $1 | sed 's/\"//g'`
s3Path=`echo $2 | sed 's/\"//g'`

pid=$$
localOutput=/mnt2/localqc/$pid

mkdir -p $localOutput

export s3Path="$s3Path/"

logPath="/mnt2/log/integratedWorks/quality_check.log"

./qc $input $localOutput > $logPath

aws s3 cp $localOutput $s3Path --recursive >> $logPath

aws s3 ls $s3Path 2>>$logPath | awk '{print ENVIRON["s3Path"]$4}' | paste -sd "," -

rm -r $localOutput
rm -r $input
