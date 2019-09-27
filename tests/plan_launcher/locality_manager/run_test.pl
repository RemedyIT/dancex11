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

$DANCEX11_ROOT = "$ENV{'DANCEX11_ROOT'}";
$DANCEX11_BIN_FOLDER = $ENV{'DANCEX11_BIN_FOLDER'} || 'bin';

$sleep_time = 1;

$nr_daemon = 1;
@ports = ( 60001 );
@iorbases = ( "LocalityOne.ior" );
@locnames = ( "LocalityOne" );

$lm_app_ior = "app-deployment.ior";
$lm_am_ior = "appmgr-deployment.ior";

$daemons_running = 0;
$ns_running = 0;

@iorfiles = 0;

# ior files other than daemon
$ior_nsbase = "ns.ior";
$ior_nsfile = 0;

#  Processes
$E = 0;
$NS = 0;
@DEAMONS = 0;

# targets
@tg_daemons = 0;
$tg_executor = 0;

$status = 0;

$plan_fmt = "config";
$cdd_fmt = "cfg";

while ($i = shift) {
    if ($i eq '-fmt') {
        $plan_fmt = shift;
    }
}

$plan_file = "deployment." . $plan_fmt;

if ($plan_fmt eq "cdp") {
  $cdd_fmt = "cdd";
} elsif ($plan_fmt ne "config") {
  $cdd_fmt = $plan_fmt;
}

sub create_targets {
    #   daemon
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $tg_daemons[$i] = PerlACE::TestTarget::create_target ($i+2) || die "Create target for daemon $i failed\n";
        $tg_daemons[$i]->AddLibPath ('../lib');
    }
    #   executor (plan_launcher)
    $tg_executor = PerlACE::TestTarget::create_target (1) || die "Create target for executor failed\n";
    $tg_executor->AddLibPath ('.');
}

sub init_ior_files {
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
    $tg_executor->DeleteFile ($lm_app_ior);
    $tg_executor->DeleteFile ($lm_am_ior);
}

sub kill_locality_daemon {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $DEAMONS[$i]->Kill (); $DEAMONS[$i]->TimedWait (1);
    }
}

sub kill_open_processes {
    if ($daemons_running == 1) {
        kill_locality_daemon ();
    }

    if ($ns_running == 1) {
        $NS->Kill (); $NS->TimedWait (1);
    }
    # in case shutdown did not perform as expected
    for ($i = 0; $i < $nr_daemon; ++$i) {
      $tg_daemons[$i]->KillAll ('dancex11_deployment_manager');
    }
}

sub run_locality_daemons {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $iorbase = $iorbases[$i];
        $iorfile = $iorfiles[$i];
        $port = $ports[$i];
        $locname = $locnames[$i];
        $dancex11_root = $tg_daemons[$i]->LocalEnvDir("$DANCEX11_ROOT");

        $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
        $d_param = "--handler dancex11_locality_dm_handler -p $port -N -n $locname=$iorfile";

        print "Run dancex11_deployment_manager with $d_param\n";

        $DEAMONS[$i] = $tg_daemons[$i]->CreateProcess ($d_cmd, $d_param);
        $DEAMONS[$i]->Spawn ();

        if ($tg_daemons[$i]->WaitForFileTimed($iorbase,
                                        $tg_daemons[$i]->ProcessStartWaitInterval ()) == -1) {
            print STDERR
                "ERROR: The ior $iorfile file of locality daemon $i could not be found\n";
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

# Invoke locality daemon.
print "Invoking locality daemon\n";
$status = run_locality_daemons ();

if ($status != 0) {
    print STDERR "ERROR: Unable to execute the locality daemon\n";
    kill_open_processes ();
    exit 1;
}

$daemons_running = 1;

# copy NM IORs
for ($i = 0; $i < $nr_daemon; ++$i) {
  $tg_daemons[$i]->GetFile ($iorbases[$i]);
  $tg_executor->PutFile ($iorbases[$i]);
}

# Invoke executor - start the application -.
print "Invoking executor - launch the application -\n";

print "Start dancex11_plan_launcher.exe with -l $plan_file -r file://".$tg_executor->LocalFile($iorbases[0])."\n";
$E = $tg_executor->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_plan_launcher",
                        "-l " . $tg_executor->LocalFile($plan_file) .
                        " -r file://" . $tg_executor->LocalFile($iorbases[0]) .
                        " --app-ior " . $tg_executor->LocalFile($lm_app_ior) .
                        " --am-ior " . $tg_executor->LocalFile($lm_am_ior));

$pl_status = $E->SpawnWaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * $tg_executor->ProcessStartWaitInterval ());

if ($pl_status != 0) {
    print STDERR "ERROR: dancex11_plan_launcher returned $pl_status\n";
    kill_open_processes ();
    exit 1;
}

if ($tg_executor->WaitForFileTimed($lm_app_ior,
                                   $tg_executor->ProcessStartWaitInterval ()) == -1) {
    print STDERR
        "ERROR: The ior $lm_app_ior file of the LM Application could not be found\n";
    kill_open_processes ();
    exit 1;
}

$corrected_sleep_time = $sleep_time * $PerlACE::Process::WAIT_DELAY_FACTOR;
print "Sleeping $corrected_sleep_time seconds to allow task to complete\n";
sleep ($corrected_sleep_time);

# Invoke executor - stop the application -.
print "Invoking executor - stop the application -\n";
print "by running dancex11_plan_launcher with -x $plan_file -r file://".$tg_executor->LocalFile($iorbases[0])."\n";

$E = $tg_executor->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_plan_launcher",
                        "-x " . $tg_executor->LocalFile($plan_file) . " -r file://".$tg_executor->LocalFile($iorbases[0]));
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
