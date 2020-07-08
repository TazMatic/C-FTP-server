#!/usr/bin/env bash
echo "ALL MAKE STDOUT IS SUPPRESSED IN THIS RUN"
echo "*********************Test1: Standard make******************"
make 1>/dev/null
echo "*********************Test1: Complete***********************"
echo "*********************Test2: Make clean*********************"
make clean 1>/dev/null
echo "*********************Test2: Complete  *********************"
echo "*********************Test3: Make debug*********************"
make debug 1>/dev/null
make clean 1>/dev/null
echo "*********************Test3: Complete***********************"
echo "*********************Test4: Make profile*******************"
make profile 1>/dev/null
make clean 1>/dev/null
echo "*********************Test4: Complete***********************"
echo "*********************Test5: Make Clean*********************"
make clean 1>/dev/null
echo "*********************Test5: Complete***********************"
echo "*********************Test6: Start Dispatcher***************"
