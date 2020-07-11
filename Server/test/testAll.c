#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int
main(void)
{
    extern CU_SuiteInfo DatabaseSuite;
    //extern CU_SuiteInfo ServerSuite;

    CU_SuiteInfo suites[] = {
        // Name, suiteSetup,suiteTeardown,testSetup,testTeardown,arrayOfTests
            DatabaseSuite,
            CU_SUITE_INFO_NULL
    };

    CU_initialize_registry();

    // Add all suites listed to registry
    CU_register_suites(suites);

    CU_basic_run_tests();

    CU_cleanup_registry();
}
