#!/bin/sh

cmd_output=$(ls ./my_plan_repo/* | grep -c "readme\.txt\|test_resource\.dat")

if [ "$cmd_output" == "2" ] ; then
  echo "Success!  The artifacts are installed correctly.";
  exit 0
else
  echo "ERROR: Incorrect artifacts installation; the directory contains:"
  ls -la -R *
  exit 1
fi
