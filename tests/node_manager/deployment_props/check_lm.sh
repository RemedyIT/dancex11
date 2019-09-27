#!/bin/sh

cmd_output1=$(ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler")
cmd_output=$(echo "$cmd_output1" | grep -c "dancex11_deployment_manager.*-n.*LocalityInstance")

if [ "$cmd_output" == "2" ] ; then
  echo "Success!  The NM have been contacted and the LM have been correctly spawned.";

  rc=0

  for ior in Locality*.ior ; do
    ior_host=`$ACE_ROOT/bin/tao_catior -f $ior 2>&1 | grep "Host\ Name:"`
    if [ `echo $ior_host | grep -c "localhost"` == "1" ]; then
      echo "$ior correctly contains a hostname"
    else
      echo "ERROR: $ior does NOT contain a hostname but instead contains [$ior_host]"
      rc=1
    fi
  done

  for ior in lm*.ior ; do
    ior_host=`$ACE_ROOT/bin/tao_catior -f $ior 2>&1 | grep "Host\ Name:"`
    if [ `echo $ior_host | grep -c "127\.0\.0\.1"` == "1" ]; then
      echo "$ior correctly contains a numeric IP address"
    else
      echo "ERROR: $ior does NOT contain a numeric IP address but instead contains [$ior_host]"
      rc=1
    fi
  done

  exit $rc
else
  echo "ERROR: Unexpected number of LM processes detected:"
  echo "  Found:"
  echo $cmd_output1
  echo "  Current LM processes:"
  ps ax | grep "dancex11_deployment_manager.*dancex11_locality_dm_handler"
  exit 1
fi
