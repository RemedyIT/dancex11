#!/bin/sh

cmd_output=$(ps auxc | grep newlocality_tes | wc -l)

if [ "$cmd_output" -ne "1" ] ; then
    echo "ERROR: Process name does not appear to have changed."
    echo "Process listing:"
    ps auxc
    exit 1
else
  echo "Success!  Process name appears to have been changed appropriately.";
  exit 0
fi
