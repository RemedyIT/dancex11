/**
 * @file    log_test.cpp
 * @author  Marijke Hengstmengel
 *
 * @brief   CORBA C++11 logging test
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"

// X11_FUZZ: disable check_test_log_macros

void
test_log ()
{
  //Testcase priority 1
  //First use defaults:
  dancex11_info << "**************** Use default no priority mask"
              << " and default verbose options :"
              << "V_CATEGORY, V_PRIO, V_THREAD_ID, V_PROCESSID and V_TIME ."
              << std::endl;

  dancex11_info << " begin priority testcase: 1 " << std::endl;

  dancex11_info << "not expected priority testcase: 1 " << std::endl;
  DANCEX11_LOG_DEBUG ("If you see this message from < DANCEX11_LOG_DEBUG>, it is an ERROR.") ;
  DANCEX11_LOG_WARNING ("If you see this message from < DANCEX11_LOG_WARNING>, it is an ERROR.") ;
  // Panic messages are always logged, don't log this otherwise an error will be logged.
  // DANCEX11_LOG_PANIC ("If you see this message from < DANCEX11_LOG_PANIC>, it is an ERROR.") ;
  DANCEX11_LOG_DEBUG_W ("If you see this message from < DANCEX11_LOG_DEBUG_W>, it is an ERROR.") ;
  DANCEX11_LOG_INFO ("If you see this message from < DANCEX11_LOG_INFO>, it is an ERROR.");
  DANCEX11_LOG_CRITICAL ("If you see this message from < DANCEX11_LOG_CRITICAL>, it is an ERROR.");
  DANCEX11_LOG_ERROR ("If you see this message from < DANCEX11_LOG_ERROR>, it is an ERROR.");

  dancex11_info << "end priority testcase: 1 " << std::endl;


  //Testcase priority 2
  dancex11_info << "**************** Use priority mask"
              << " LP_WARNING | LP_PANIC "
              << " and verbose options V_PRIO."
              << std::endl;

  DANCEX11_LOGGER::priority_mask (x11_logger::LP_WARNING |
                                x11_logger::LP_PANIC);
  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_PRIO );


  dancex11_info << " begin priority testcase: 2 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_PANIC" << std::endl;
  DANCEX11_LOG_PANIC ("Test of the macro <DANCEX11_LOG_PANIC>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_WARNING" << std::endl;
  DANCEX11_LOG_WARNING ("Test of the macro <DANCEX11_LOG_WARNING>");

  dancex11_info << " not expected priority testcase: 2 " << std::endl;
  DANCEX11_LOG_DEBUG ("If you see this message from < DANCEX11_LOG_DEBUG>, it is an ERROR.") ;
  DANCEX11_LOG_INFO ("If you see this message from < DANCEX11_LOG_INFO>, it is an ERROR.");
  DANCEX11_LOG_CRITICAL ("If you see this message from < DANCEX11_LOG_CRITICAL>, it is an ERROR.");
  DANCEX11_LOG_ERROR ("If you see this message from < DANCEX11_LOG_ERROR>, it is an ERROR.");

  dancex11_info << " end priority testcase: 2 " << std::endl;


  //Testcase priority 3
  dancex11_info << "**************** Use priority mask"
              << " LP_DEBUG | LP_INFO | LP_CRITICAL"
              << " and verbose options V_PRIO ."
              << std::endl;

  DANCEX11_LOGGER::priority_mask (x11_logger::LP_DEBUG |
                                x11_logger::LP_INFO |
                                x11_logger::LP_CRITICAL);

  dancex11_info << " begin priority testcase: 3 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_DEBUG" << std::endl;
  DANCEX11_LOG_DEBUG ("Test of the macro <DANCEX11_LOG_DEBUG>") ;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_INFO" << std::endl;
  DANCEX11_LOG_INFO ("Test of the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_CRITICAL" << std::endl;
  DANCEX11_LOG_CRITICAL ("Test of the macro <DANCEX11_LOG_CRITICAL>");

  dancex11_info << " not expected priority testcase: 3 " << std::endl;
  DANCEX11_LOG_PANIC ("If you see this message from < DANCEX11_LOG_PANIC>, it is an ERROR.");
  DANCEX11_LOG_ERROR ("If you see this message from < DANCEX11_LOG_ERROR>, it is an ERROR.");
  DANCEX11_LOG_WARNING ("If you see this message from < DANCEX11_LOG_WARNING>, it is an ERROR.");

  dancex11_info << " end priority testcase: 3 " << std::endl;


  //Testcase priority 4
  dancex11_info << "*************** uses priority mask"
              << " LP_ALL_ER-ROR"
              << " and verbose options V_CATEGORY."
              << std::endl;
  DANCEX11_LOGGER::priority_mask (x11_logger::LP_ALL_ERROR );
  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_CATEGORY );

  dancex11_info << " begin priority testcase: 4 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_PANIC" << std::endl;
  DANCEX11_LOG_PANIC ("Test of the macro <DANCEX11_LOG_PANIC>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_CRITICAL" << std::endl;
  DANCEX11_LOG_CRITICAL ("Test of the macro <DANCEX11_LOG_CRITICAL>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_ER-ROR" << std::endl;
  DANCEX11_LOG_ERROR ("Test of the macro <DANCEX11_LOG_ER-ROR>");

  dancex11_info << " not expected priority testcase: 4 " << std::endl;
  DANCEX11_LOG_DEBUG ("If you see this message from < DANCEX11_LOG_DEBUG>, it is an ERROR.") ;
  DANCEX11_LOG_INFO ("If you see this message from < DANCEX11_LOG_INFO>, it is an ERROR.");
  DANCEX11_LOG_WARNING ("If you see this message from < DANCEX11_LOG_WARNING>, it is an ERROR.");

  dancex11_info << " end priority testcase: 4 " << std::endl;

  //Testcase priority 5
  dancex11_info << "*************** uses priority mask"
               << " LP_ALL"
               << " and verbose options V_CATEGORY."
               << std::endl;
  DANCEX11_LOGGER::priority_mask (x11_logger::LP_ALL );
  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_CATEGORY );

  dancex11_info << " begin priority testcase: 5 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_DEBUG" << std::endl;
  DANCEX11_LOG_DEBUG ("Test of the macro <DANCEX11_LOG_DEBUG>") ;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_DEBUG_W" << std::endl;
  DANCEX11_LOG_DEBUG_W (L"Test of the macro" << "<DANCEX11_LOG_DEBUG_W>") ;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_INFO" << std::endl;
  DANCEX11_LOG_INFO ("Test of the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_PANIC" << std::endl;
  DANCEX11_LOG_PANIC ("Test of the macro <DANCEX11_LOG_PANIC>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_CRITICAL" << std::endl;
  DANCEX11_LOG_CRITICAL ("Test of the macro <DANCEX11_LOG_CRITICAL>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_WARNING" << std::endl;
  DANCEX11_LOG_WARNING ("Test of the macro <DANCEX11_LOG_WARNING>");

  dancex11_info << "not expected priority testcase: 5 " << std::endl;
  dancex11_info << "end priority testcase: 5 " << std::endl;

  //Testcase verbose 1
  dancex11_info << "************* uses priority mask"
              << " LP_INFO"
              << " and verbose options :"
              << "V_THREAD_ID, V_PROCESS_ID and V_DATETIME ."
              << std::endl;
  DANCEX11_LOGGER::priority_mask (x11_logger::LP_INFO );

  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_THREAD_ID |
                                 x11_logger::V_PROCESS_ID |
                                 x11_logger::V_DATETIME );

  dancex11_info << " begin verbose testcase: 1 " << std::endl;
  dancex11_info << "DANCEX11_LOG verbosity expected: V_THREAD_ID,V_PROCESS_ID,V_DATETIME" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG verbosity unexpected: V_CATEGORY,V_PRIO" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << " end verbose testcase: 1 " << std::endl;


  //Testcase verbose 2
  dancex11_info << "************* uses priority mask"
              << " LP_INFO"
              << " and verbose options :"
              << "V_CATEGORY, V_PRIO, V_PROCESS_ID and V_TIME ."
              << std::endl;

  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_PRIO |
                                 x11_logger::V_CATEGORY |
                                 x11_logger::V_PROCESS_ID |
                                 x11_logger::V_TIME );

  dancex11_info << " begin verbose testcase: 2 " << std::endl;
  dancex11_info << "DANCEX11_LOG verbosity expected: V_CATEGORY,V_PRIO,V_PROCESS_ID,V_TIME" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity  with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG verbosity unexpected: V_DATETIME" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");

  dancex11_info << " end verbose testcase: 2 " << std::endl;

  //Testcase same messages with DANCEX11_LOG and X11_LOG .
  dancex11_info << "************* uses priority mask"
              << " LP_INFO, and verbose options :"
              << "V_CATEGORY, V_PRIO."
              << " Expecting 2 equal messages:" << std::endl;
  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_PRIO |
                                 x11_logger::V_CATEGORY);

  dancex11_info << " begin same message testcase: 1 " << std::endl;
  dancex11_info << "same message expected: Result from ret_str is hello" << std::endl;
  std::string ret_str = "hello";
  std::string mess_str = "Result from ret_str is ";
  X11_LOG_INFO (DANCEX11_LOGGER, mess_str << ret_str);
  DANCEX11_LOG_INFO (mess_str << ret_str);
  dancex11_info << " end same message testcase: 1 " << std::endl;

}


int main(int /*argc*/, char** /*argv[]*/)
{
  try
  {
    //now test logging macro's
    test_log();

    dancex11_info << "shutting down...";

    dancex11_info << std::endl;

  }
  catch (const std::exception& e)
  {
    dancex11_error << "exception caught: " << e.what () << std::endl;
    return 1;
  }
  return 0;
}
