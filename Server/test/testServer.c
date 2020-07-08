#include <CUnit/CUnit.h>
#include <stdlib.h>

static CU_TestInfo ServerTests[] = {
        // Name, functionThatRunsTest
        CU_TEST_INFO_NULL
};

CU_SuiteInfo ServerSuite = {
        "Server tests",
        NULL,
        NULL,
        NULL,
        NULL,
        ServerTests
};
