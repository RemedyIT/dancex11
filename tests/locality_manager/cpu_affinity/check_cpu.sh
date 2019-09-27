#!/bin/sh

cmd_output=$(ps -eo psr,args | grep "dancex11_deployment_manager.*-n.*inst_one.ior" | grep " 1 ")

if [ "$cmd_output" == "" ] ; then
  echo "ERROR: Wrong CPU usage detected. Current process listing:"
  ps -eo psr,args | grep "dancex11_locality_manager -o inst_one.ior"
  exit 1
else
  echo "Success!  The dancex11_locality_manager uses the correct CPU.";
  exit 0
fi
