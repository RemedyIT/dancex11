#---------------------------------------------------------------------
# @file   run_test.pl
# @author Martin Corino
#
# @copyright Copyright (c) Remedy IT Expertise BV
# Chamber of commerce Rotterdam nr.276339, The Netherlands
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

$daemons_running = 0;
$em_running = 0;
$ns_running = 0;

$nr_daemon = 1;
@ports = ( 60001 );
@iorbases = ( "InstOne.ior" );
@iorfiles = 0;
@nodenames = ( "LocalityOne" );

# ior files other than daemon
# ior files other than daemon
$ior_nsbase = "ns.ior";
$ior_nsfile = 0;
$ior_embase = "EM.ior";
$ior_emfile = 0;

#  Processes
$E = 0;
$EM = 0;
$NS = 0;
@DEAMONS = 0;

# targets
@tg_daemons = 0;
$tg_naming = 0;
$tg_exe_man = 0;
$tg_executor = 0;

$status = 0;

$plan_fmt = "config";

while ($i = shift) {
    if ($i eq '-fmt') {
        $plan_fmt = shift;
    }
}

$plan_file = "deployment." . $plan_fmt;

sub create_targets {
    #   naming service
    $tg_naming = PerlACE::TestTarget::create_target (1) || die "Create target for ns failed\n";
    $tg_naming->AddLibPath ('.');
    #   daemon
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $tg_daemons[$i] = PerlACE::TestTarget::create_target ($i+2) || die "Create target for daemon $i failed\n";
        $tg_daemons[$i]->AddLibPath ('.');
    }
    #   execution manager
    $tg_exe_man = PerlACE::TestTarget::create_target (1) || die "Create target for EM failed\n";
    $tg_exe_man->AddLibPath ('.');
    #   executor (plan_launcher)
    $tg_executor = PerlACE::TestTarget::create_target (1) || die "Create target for executor failed\n";
    $tg_executor->AddLibPath ('.');
}

sub init_ior_files {
    $ior_nsfile = $tg_naming->LocalFile ($ior_nsbase);
    $ior_emfile = $tg_exe_man->LocalFile ($ior_embase);
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $iorfiles[$i] = $tg_daemons[$i]->LocalFile ($iorbases[$i]);
    }
    delete_ior_files ();
}

# Delete if there are any .ior files.
sub delete_ior_files {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $tg_daemons[$i]->DeleteFile ($iorbases[$i]);
    }
    $tg_naming->DeleteFile ($ior_nsbase);
    $tg_exe_man->DeleteFile ($ior_embase);
}

sub kill_node_daemon {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $DEAMONS[$i]->Kill (); $DEAMONS[$i]->TimedWait (1);
    }
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
    $tg_executor->KillAll ('dancex11_deployment_manager');
}

sub run_node_daemons {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $iorbase = $iorbases[$i];
        $iorfile = $iorfiles[$i];
        $port = $ports[$i];
        $nodename = $nodenames[$i];

        $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
        $d_param = "--handler dancex11_node_dm_handler -p $port -N -n $nodename=$iorfile --deployment-nc " . $ENV{'NameServiceIOR'};

        print "Run dancex11_deployment_manager with $d_param\n";
        $DEAMONS[$i] = $tg_daemons[$i]->CreateProcess ($d_cmd, $d_param);
        $DEAMONS[$i]->Spawn ();

        if ($tg_daemons[$i]->WaitForFileTimed($iorbase,
                                              $tg_daemons[$i]->ProcessStartWaitInterval ()) == -1) {
            print STDERR
              "ERROR: The ior $iorfile file of node daemon $i could not be found\n";
            for (; $i >= 0; --$i) {
                $DEAMONS[$i]->Kill (); $DEAMONS[$i]->TimedWait (1);
            }
            return -1;
        }
    }
    return 0;
}

create_targets ();
init_ior_files ();

# Invoke naming service

$my_ip = $tg_naming->IP_Address ();

$NS = $tg_naming->CreateProcess ("$TAO_ROOT/orbsvcs/Naming_Service/tao_cosnaming", " -ORBEndpoint iiop://$my_ip:60003 -o $ior_nsfile");

print STDERR "Starting Naming Service with  -ORBEndpoint iiop://$my_ip:60003 -o ns.ior\n";

$ns_status = $NS->Spawn ();

if ($ns_status != 0) {
    print STDERR "ERROR: Unable to execute the naming service\n";
    kill_open_processes ();
    exit 1;
}

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
$status = run_node_daemons ();

if ($status != 0) {
    print STDERR "ERROR: Unable to execute the node daemon\n";
    kill_open_processes ();
    exit 1;
}

$daemons_running = 1;

# Invoke executor - start the application -.
print "Invoking executor - launch the application -\n";

print "Start dancex11_plan_launcher.exe with -l $plan_file -r file://$ior_emfile\n";
$E = $tg_executor->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_plan_launcher",
                        "-l " . $tg_executor->LocalFile($plan_file) . " -r file://$ior_emfile -t 10");

$pl_status = $E->Spawn ();

if ($pl_status != 0) {
    print STDERR "ERROR: dancex11_plan_launcher returned $pl_status\n";
    kill_open_processes ();
    exit 1;
}

#Sleep a moment to make sure the PL is properly holding.
print "Sleeping 3 seconds to ensure that PL properly waits on EM reference.\n";
sleep 3;

# Invoke execution manager.
print "Invoking domain manager (dancex11_deployment_manager --handler dancex11_domain_dm_handler) with -n ExecutionManager=$ior_emfile\n";
$EM = $tg_exe_man->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager",
                                  "--handler dancex11_domain_dm_handler -n ExecutionManager=$ior_emfile --deployment-nc " . $ENV{'NameServiceIOR'});
$em_status = $EM->Spawn ();

if ($em_status != 0) {
    print STDERR "ERROR: dancex11_deployment_manager returned $em_status";
    exit 1;
}

if ($tg_exe_man->WaitForFileTimed ($ior_embase,
                                   $tg_exe_man->ProcessStartWaitInterval ()) == -1) {
    print STDERR
      "ERROR: The ior file of execution manager could not be found\n";
    kill_open_processes ();
    exit 1;
}

$em_running = 1;

for ($i = 0; $i < $nr_daemon; ++$i) {
    if ($tg_daemons[$i]->WaitForFileTimed ($iorbases[$i],
                            $tg_daemons[$i]->ProcessStopWaitInterval ()) == -1) {
        print STDERR "ERROR: The ior file of daemon $i could not be found\n";
        kill_open_processes ();
        exit 1;
    }
}

$pl_status = $E->WaitKill (5);

if ($pl_status != 0) {
    print STDERR "ERROR: dancex11_plan_launcher returned $pl_status\n";
    kill_open_processes ();
    exit 1;
}

print "Sleeping 10 seconds to allow task to complete\n";
sleep (10);

# Invoke executor - stop the application -.
print "Invoking executor - stop the application -\n";
print "by running dancex11_plan_launcher with -x $plan_file -r file://$ior_emfile\n";

$E = $tg_executor->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_plan_launcher",
                        "-x " . $tg_executor->LocalFile($plan_file) . " -r file://$ior_emfile");
$pl_status = $E->SpawnWaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * $tg_executor->ProcessStartWaitInterval ());

if ($pl_status != 0) {
    print STDERR "ERROR: dancex11_plan_launcher returned $pl_status\n";
    kill_open_processes ();
    exit 1;
}

print "Executor returned.\n";
print "Shutting down rest of the processes.\n";

delete_ior_files ();
kill_open_processes ();

exit $status;
