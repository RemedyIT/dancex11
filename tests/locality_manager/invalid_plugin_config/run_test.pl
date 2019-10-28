#---------------------------------------------------------------------
# @file   run_test.pl
# @author Marijke Hengstmengel
#
# @copyright Copyright (c) Remedy IT Expertise BV
#---------------------------------------------------------------------
eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}'
    & eval 'exec perl -S $0 $argv:q'
    if 0;

# -*- perl -*-

use lib "$ENV{'ACE_ROOT'}/bin";
use PerlACE::TestTarget;

$TAO_ROOT = "$ENV{'TAO_ROOT'}";
$DANCEX11_ROOT = "$ENV{'DANCEX11_ROOT'}";
$DANCEX11_BIN_FOLDER = $ENV{'DANCEX11_BIN_FOLDER'} || 'bin';

$sleep_time = 3;

$port = 60001;
$iorbase = "node_one.ior";
$nodename = "LocalityOne";

$daemons_running = 0;
$em_running = 0;
$ns_running = 0;

$iorfile = 0;

# ior files other than daemon
$ior_nsbase = "ns.ior";
$ior_nsfile = 0;
$ior_embase = "em.ior";
$ior_emfile = 0;

#  Processes
$E = 0;
$EM = 0;
$NS = 0;
$DEAMON = 0;

# targets
$tg_daemon = 0;
$tg_naming = 0;
$tg_exe_man = 0;
$tg_executor = 0;

$plan_fmt = "config";
$debug = 0;

while ($i = shift) {
    if ($i eq '-fmt') {
        $plan_fmt = shift;
    } elsif ($i eq '-debug') {
        $debug = 1;
    }
}

$status = 0;
$plan_file = "deployment." . $plan_fmt;

sub create_targets {
    #   naming service
    $tg_naming = PerlACE::TestTarget::create_target (1) || die "Create target for ns failed\n";
    #   daemon
    $tg_daemon = PerlACE::TestTarget::create_target (2) || die "Create target for daemon 2 failed\n";
    $tg_daemon->AddLibPath ('.');
    if ($debug == 0) {
        $tg_daemon->SetEnv ('DANCEX11_LOG_MASK', 'none'); # suppress error logs to prevent unwanted error reporting by test framework
    }
    #   execution manager
    $tg_exe_man = PerlACE::TestTarget::create_target (1) || die "Create target for EM failed\n";
    if ($debug == 0) {
        $tg_exe_man->SetEnv ('DANCEX11_LOG_MASK', 'none'); # suppress error logs to prevent unwanted error reporting by test framework
    }
    #   executor (plan_launcher)
    $tg_executor = PerlACE::TestTarget::create_target (1) || die "Create target for executor failed\n";
}

sub init_ior_files {
    $ior_nsfile = $tg_naming->LocalFile ($ior_nsbase);
    $ior_emfile = $tg_exe_man->LocalFile ($ior_embase);
    $iorfile = $tg_daemon->LocalFile ($iorbase);
    delete_ior_files ();
}

# Delete if there are any .ior files.
sub delete_ior_files {
    $tg_daemon->DeleteFile ($iorbase);
    $tg_naming->DeleteFile ($ior_nsbase);
    $tg_exe_man->DeleteFile ($ior_embase);
    $iorfile = $tg_daemon->LocalFile ($iorbase);
}

sub kill_node_daemon {
    $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
    $d_param = "-x -p $port -n $nodename=$iorfile --deployment-nc ". $ENV{'NameServiceIOR'};
    $daemon_stopper = $tg_daemon->CreateProcess ($d_cmd, $d_param);
    $daemon_stopper->SpawnWaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * $tg_daemon->ProcessStartWaitInterval ());
    $DEAMON->WaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * 5);
}

sub kill_open_processes {
    if ($daemons_running == 1) {
        kill_node_daemon ();
    }

    if ($em_running == 1) {
        $EM->Kill (); $EM->TimedWait (1);
    }

    if ($ns_running == 1) {
        $NS->Kill (); $NS->TimedWait (1);
    }
    # in case shutdown did not perform as expected
    $tg_daemon->KillAll ('dancex11_deployment_manager');
}

sub run_node_daemon {
    $iiop = "iiop://:$port";
    $dancex11_root = $tg_daemon->LocalEnvDir("$DANCEX11_ROOT");

    $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
    $d_param = "--handler dancex11_node_dm_handler -p $port -N -n $nodename=$iorfile --deployment-nc ". $ENV{'NameServiceIOR'};

    print "Run dancex11_deployment_manager with $d_param\n";

    $DEAMON = $tg_daemon->CreateProcess ($d_cmd, $d_param);
    $DEAMON->Spawn ();

    if ($tg_daemon->WaitForFileTimed($iorbase,
                                     $tg_daemon->ProcessStartWaitInterval ()) == -1) {
        print STDERR
            "ERROR: The ior $iorfile file of node daemon $i could not be found\n";
        $DEAMON->Kill (); $DEAMON->TimedWait (1);
        return -1;
    }
    return 0;
}

create_targets ();
init_ior_files ();

$my_ip = $tg_naming->IP_Address ();

# Invoke naming service

$NS = $tg_naming->CreateProcess ("$TAO_ROOT/orbsvcs/Naming_Service/tao_cosnaming", " -ORBEndpoint iiop://$my_ip:60003 -o $ior_nsfile");

$ns_status = $NS->Spawn ();

if ($ns_status != 0) {
    print STDERR "ERROR: Unable to execute the naming service\n";
    kill_open_processes ();
    exit 1;
}

print STDERR "Starting Naming Service with  -ORBEndpoint iiop://$my_ip:60003 -o ns.ior\n";

if ($tg_naming->WaitForFileTimed ($ior_nsbase,
                                  $tg_naming->ProcessStartWaitInterval ()) == -1) {
    print STDERR "ERROR: cannot find naming service IOR file\n";
    $NS->Kill (); $NS->TimedWait (1);
    exit 1;
}

$ns_running = 1;
# Set up NamingService environment
$ENV{"NameServiceIOR"} = "corbaloc:iiop:$my_ip:60003/NameService";

# Invoke node daemon.
print "Invoking node daemon\n";
$status = run_node_daemon ();

if ($status != 0) {
    print STDERR "ERROR: Unable to execute the node daemon\n";
    kill_open_processes ();
    exit 1;
}

$daemons_running = 1;

# Invoke execution manager.
print "Invoking domain deployment manager (dancex11_deployment_manager --handler dancex11_domain_dm_handler) with -l $plan_file\n";
$EM = $tg_exe_man->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager",
                                  "--handler dancex11_domain_dm_handler -l " . $tg_exe_man->LocalFile($plan_file) .
                                  " -n ExecutionManager=$ior_emfile --deployment-nc " .
                                  $ENV{'NameServiceIOR'});
$em_status = $EM->Spawn ();

if ($em_status != 0) {
    print STDERR "ERROR: spawning dancex11_deployment_manager returned $em_status\n";
    exit 1;
}

if ($tg_exe_man->WaitForFileTimed ($ior_embase,
                                   $tg_exe_man->ProcessStartWaitInterval ()) == -1) {
    $em_status = $EM->WaitKill ($tg_daemon->ProcessStartWaitInterval ());
    if ($em_status == 0)
    {
      print STDERR "ERROR: The ior file of execution manager could not be found\n";
    }
}

if ($em_status != 0) {
  print STDERR "OK: deployment failed as expected!\n"
} else {
  print STDERR "ERROR: deployment succeeded and should have failed\n";

  $em_running = 1;

  $corrected_sleep_time = $sleep_time * $PerlACE::Process::WAIT_DELAY_FACTOR;
  print "Sleeping $corrected_sleep_time seconds to allow task to complete\n";
  sleep ($corrected_sleep_time);

  # Invoke executor manager - stop the application -.
  print "Invoking executor - stop the application -\n";
  print "by running dancex11_deployment_manager with -n ExecutionManager=$ior_emfile -x\n";

  $E = $tg_executor->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager",
                          "-n ExecutionManager=$ior_emfile -x");
  $pl_status = $E->SpawnWaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * $tg_executor->ProcessStartWaitInterval ());

  if ($pl_status != 0) {
      print STDERR "ERROR: dancex11_deployment_manager returned $pl_status\n";
      kill_open_processes ();
      exit 1;
  }
}

print "Executor returned.\n";
print "Shutting down rest of the processes.\n";

kill_open_processes ();
delete_ior_files ();

exit $status;
