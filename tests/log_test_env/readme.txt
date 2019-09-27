Logging  test with environment variables.

In this test logging macro's of DANCEX11  are used.
The priority mask determine which macro's are enabled .
The verbosity mask  determine the format of the output of the macro's .


Different settings of the priority masks and verbosity masks of DANCEX11_LOGGER are used
after each other. 
For this the environment variables DANCEX11_LOG_MASK, X11_LOG_MASK, X11_VERBOSE and
DANCEX11_VERBOSE  are set in the script run_test.pl and the setter operations of the 
priority_mask and verbosity_mask are used in the application self.
If X11_LOG_MASK and DANCEX11_LOG_MASK are set, DANCEX11_LOG_MASK overwrite the priority-mask set with X11_LOG_MASK.
If X11_VERBOSE and DANCEX11_VERBOSE are set, DANCEX11_VERBOSE overwrite the verbosity_mask set with X11_VERBOSE.
If in the application the setter operation are used, these will in turn overwrite the priority-mask 
and verbosity-mask values again.

WWe check automatically the right output of the macro's in the script $TAOX11_ROOT/tao/x11/logger/log_check.pl
The script expected a blocks of code in a special order.  This is described in 
$TAOX11_ROOT/tao/x11/logger/readme.txt
