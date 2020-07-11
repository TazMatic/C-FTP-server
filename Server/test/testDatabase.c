#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/Database.h"

static void
testDBCreation(void)
{
    int db = record_open((char *)"DBTest");
    CU_ASSERT(db > 0);
    close(db);
}

static CU_TestInfo DatabaseTests[] = {
        // Name, functionThatRunsTest
        { "record_open", testDBCreation },
        CU_TEST_INFO_NULL
};

CU_SuiteInfo DatabaseSuite = {
        "Database tests",
        NULL,
        NULL,
        NULL,
        NULL,
        DatabaseTests
};
