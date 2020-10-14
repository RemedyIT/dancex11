#!/bin/sh

cmd_output1=$(ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler")
cmd_output=$(echo "$cmd_output1" | grep -c "dancex11_deployment_manager.*-n.*LocalityInstance")

if [ "$cmd_output" == "0" ] ; then
  echo "Success!  The NM have been contacted and no LM have been spawned.";

  rc=0

  cmd_output=$(ps auxc | grep NM_with_inproce | wc -l)

  if [ "$cmd_output" -eq "0" ] ; then
      echo "ERROR: Process name does not appear to have changed."
      echo "Process listing:"
      ps auxc
      rc=1
  else
    echo "Success!  NM Process name appears to have been changed appropriately.";
  fi

  exit $rc
else
  echo "ERROR: Unexpected number of LM processes detected:"
  echo "  Found:"
  echo $cmd_output1
  echo "  Current LM processes:"
  ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler"
  exit 1
fi
