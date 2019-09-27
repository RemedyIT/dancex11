#!/bin/sh

cmd_output1=$(ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler")
cmd_output=$(echo "$cmd_output1" | grep -c "dancex11_deployment_manager.*-n.*LocalityInstance") 

if [ "$cmd_output" == "2" ] ; then
  echo "Success!  The NM have been contacted and the LM have been correctly spawned.";
  exit 0
else
  echo "ERROR: Unexpected number of LM processes detected:"
  echo "  Found:"
  echo $cmd_output1
  echo "  Current LM processes:"
  ps ax | grep "dancex11_deployment_manager.*-n.*LocalityInstance"
  exit 1
fi
