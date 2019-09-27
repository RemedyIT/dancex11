#!/bin/sh

cmd_output=$(ls .dancex11_artifacts/* | grep -c "readme\.txt\|test_resource\.dat")

if [ "$cmd_output" == "2" ] ; then
  echo "Success!  The artifacts are installed correctly.";
  exit 0
else
  echo "ERROR: Incorrect artifacts installation; the repository contains:"
  ls -la -R ./.dancex11*
  exit 1
fi
