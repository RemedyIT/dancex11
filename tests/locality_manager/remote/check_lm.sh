#!/bin/sh

cmd_output1=$(ps ax | grep "dancex11_deployment_manager")
cmd_output=$(echo "$cmd_output1" | grep -c "dancex11_deployment_manager.*dancex11_locality_dm_handler") 

if [ "$cmd_output" == "1" ] ; then
  echo "Success! Only a single LM instance is running.";
  
  rc=0
  
  cmd_output=$(echo "$cmd_output1" | grep -c "dancex11_deployment_manager.*-n.*LocalityInstance")
  
  if [ "$cmd_output" == "1" ] ; then
      echo "Success! Single LM instance is pre-started LM process.";
  else
      echo "ERROR: Unexpected LM instance running."
      echo "Found:"
      echo $(echo "$cmd_output1" | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler")
      rc=1
  fi
  
  exit $rc
else
  echo "ERROR: Unexpected number of LM processes detected:"
  echo "  Found:"
  echo $(echo "$cmd_output1" | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler")
  echo "  Current LM processes:"
  ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler"
  exit 1
fi
