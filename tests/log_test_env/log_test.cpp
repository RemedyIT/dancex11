/**
 * @file    log_test.cpp
 * @author  Marijke Hengstmengel
 *
 * @brief   CORBA C++11 logging test with env. var.
 *
 * @copyright Copyright (c) Remedy IT Expertise BV
 */

#include "dancex11/logger/log.h"

// X11_FUZZ: disable check_test_log_macros

void
test_log ()
{
  // Testcase priority 1
  // First use priority mask LP_INFO|LP_WARNING  set via env.var. DANCEX11_LOG_MASK:
  dancex11_info << "**************** Use priority mask LP_INFO|LP_WARNING"
              << std::endl;

  dancex11_info << " begin priority testcase: 1 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_INFO" << std::endl;
  DANCEX11_LOG_INFO ("Test of the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_WARNING" << std::endl;
  DANCEX11_LOG_WARNING ("Test of the macro <DANCEX11_LOG_WARNING>");

  dancex11_info << "not expected priority testcase: 1 " << std::endl;
  DANCEX11_LOG_DEBUG ("If you see this message from < DANCEX11_LOG_DEBUG>, it is an ERROR.") ;
  dancex11_info << "end priority testcase: 1 " << std::endl;


  // Testcase priority 2
  // Overwrite priority mask LP_INFO|LP_WARNING  set via env.var.  with LP_PANIC:
  dancex11_info << "**************** Use priority mask LP_PANIC"
               << std::endl;
  DANCEX11_LOGGER::priority_mask (x11_logger::LP_PANIC );
  dancex11_info << " begin priority testcase: 2 " << std::endl;
  dancex11_info << "DANCEX11_LOG expected: DANCEX11_LOG_PANIC" << std::endl;
  DANCEX11_LOG_PANIC ("Test of the macro <DANCEX11_LOG_PANIC>");
  dancex11_info << "not expected priority testcase: 2 " << std::endl;
  DANCEX11_LOG_DEBUG ("If you see this message from < DANCEX11_LOG_DEBUG>, it is an ERROR.") ;
  DANCEX11_LOG_INFO ("If you see this message from < DANCEX11_LOG_INFO>, it is an ERROR.") ;
  dancex11_info << "end priority testcase: 2 " << std::endl;

  // Testcase verbose 1
  // Use verbose mask V_CATEGORY|V_PRIO set via env. var. DANCEX11_VERBOSE
  dancex11_info << "************* uses priority mask LP_INFO"
              << " and verbose options :"
              << "V_CATEGORY, V_PRIO."
              << std::endl;

  DANCEX11_LOGGER::priority_mask (x11_logger::LP_INFO );
  dancex11_info << " begin verbose testcase: 1 " << std::endl;
  dancex11_info << "DANCEX11_LOG verbosity expected: V_CATEGORY,V_PRIO" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG verbosity unexpected: V_TIME,V_PROCESS_ID" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << " end verbose testcase: 1 " << std::endl;

  // Testcase verbose 2
  // Overwrite verbose mask V_CATEGORY|V_PRIO set via env. var. with V_TIME
  dancex11_info << "************* uses priority mask LP_INFO"
              << " and verbose options :"
              << "V_TIME."
              << std::endl;

  DANCEX11_LOGGER::priority_mask (x11_logger::LP_INFO );
  DANCEX11_LOGGER::verbosity_mask (x11_logger::V_TIME );
  dancex11_info << " begin verbose testcase: 2 " << std::endl;
  dancex11_info << "DANCEX11_LOG verbosity expected: V_TIME" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << "DANCEX11_LOG verbosity unexpected: CATEGORY,V_PRIO,V_PROCESS_ID" << std::endl;
  DANCEX11_LOG_INFO ("Test of verbosity with the macro <DANCEX11_LOG_INFO>");
  dancex11_info << " end verbose testcase: 2 " << std::endl;
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
