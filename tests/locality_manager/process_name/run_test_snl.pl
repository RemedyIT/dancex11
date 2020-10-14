#---------------------------------------------------------------------
# @file   run_test_snl.pl
# @author Martin Corino
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

$daemons_running = 0;

$sleep_time = 3;

$nr_daemon = 1;
@ports = ( 60001);
@nodenames = ( "LocalityOne" );

#  Processes
$SP = 0;
@DEAMONS = 0;

# targets
@tg_daemons = 0;
$tg_splitter = 0;

$plan_fmt = "config";
$plan_base = "deployment.";
$status = 0;

while ($i = shift) {
    if ($i eq '-fmt') {
        $plan_fmt = shift;
    }
}

$plan_file = $plan_base . $plan_fmt;

sub create_targets {
    #   daemon
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $tg_daemons[$i] = PerlACE::TestTarget::create_target ($i+2) || die "Create target for daemon $i failed\n";
        $tg_daemons[$i]->AddLibPath ('../lib');
    }
    #   splitter (split_plan)
    $tg_splitter = PerlACE::TestTarget::create_target (1) || die "Create target for splitter failed\n";
}

# Delete if there are any .ior files.
sub delete_ior_files {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $tg_daemons[$i]->DeleteFile ("inst_one.ior");
    }
    $tg_splitter->DeleteFile("LocalityOne-deployment.cdr");
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

    # in case shutdown did not perform as expected
    for ($i = 0; $i < $nr_daemon; ++$i) {
      $tg_daemons[$i]->KillAll ('dancex11_deployment_manager');
      $tg_daemons[$i]->KillAll ('newlocality_test');
    }
}

sub run_node_daemons {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $port = $ports[$i];
        $nodename = $nodenames[$i];
        $dancex11_root = $tg_daemons[$i]->LocalEnvDir("$DANCEX11_ROOT");

        $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
        $d_param = "--handler dancex11_node_dm_handler -p $port -f cdr -l $nodename" . "-" . $plan_base . 'cdr';

        print "Run dancex11_deployment_manager with $d_param\n";

        $tg_daemons[$i]->PutFile ($nodename . "-" . $plan_base . '.cdr');

        $DEAMONS[$i] = $tg_daemons[$i]->CreateProcess ($d_cmd, $d_param);
        $tg_status = $DEAMONS[$i]->Spawn ();

        if ($tg_status != 0) {
            print STDERR "ERROR: Unable to execute the dancex11_deployment_manager $d_param\n";
            return -1;
        }
    }
    return 0;
}

sub stop_node_daemons {
    for ($i = 0; $i < $nr_daemon; ++$i) {
        $port = $ports[$i];
        $nodename = $nodenames[$i];

        $d_cmd = "$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_deployment_manager";
        $d_param = "-p $port -x";

        print "Run dancex11_deployment_manager with $d_param\n";

        $node_stopper = $tg_daemons[$i]->CreateProcess ($d_cmd, $d_param);
        $tg_status = $node_stopper->SpawnWaitKill (
                $PerlACE::Process::WAIT_DELAY_FACTOR * $tg_daemons[$i]->ProcessStartWaitInterval ());

        if ($tg_status != 0) {
            print STDERR "ERROR: Unable to execute the dancex11_deployment_manager $d_param\n";
            return -1;
        }
    }
    return 0;
}

create_targets ();
delete_ior_files ();

# Invoke plan splitter.
print "Invoking bin/dancex11_split_plan with -i $plan_file\n";

$SP = $tg_splitter->CreateProcess ("$DANCEX11_ROOT/$DANCEX11_BIN_FOLDER/dancex11_split_plan", "-i ". $tg_splitter->LocalFile ($plan_file));
$sp_status = $SP->SpawnWaitKill ($PerlACE::Process::WAIT_DELAY_FACTOR * $tg_splitter->ProcessStartWaitInterval ());

if ($sp_status != 0) {
    print STDERR "ERROR: Unable to execute dancex11_split_plan\n";
    kill_open_processes ();
    exit 1;
}

if ($tg_splitter->WaitForFileTimed ($nodenames[0]."-".$plan_base . 'cdr',
                                    $tg_splitter->ProcessStartWaitInterval ()) == -1) {
    print STDERR
      "ERROR: The split node plan could not be found\n";
    kill_open_processes ();
    exit 1;
}

# Launch nodes.
print "Launching nodes\n";
$status = run_node_daemons ();

if ($status != 0) {
    print STDERR "ERROR: Unable to execute the node daemon\n";
    kill_open_processes ();
    exit 1;
}

$daemons_running = 1;

$corrected_sleep_time = $sleep_time * $PerlACE::Process::WAIT_DELAY_FACTOR;
print "Sleeping $corrected_sleep_time seconds to allow task to complete\n";
sleep ($corrected_sleep_time);

# Run an external script on the target which checks on the process name of the locality
# is running.
$CHECKER = $tg_daemons[0]->CreateProcess ("check_process_name.sh");
$outcome = $CHECKER->SpawnWaitKill ($tg_daemons[0]->ProcessStartWaitInterval ());
if ($outcome == 1) {
    print STDERR "ERROR: Process name checker returned $outcome\n";
}

# Stop nodes.
print "Stopping nodes\n";
$status = stop_node_daemons ();

print "Shutting down rest of the processes.\n";

kill_open_processes ();

delete_ior_files ();

exit $status;
