add_test([=[MainTest.DisplayBinaryFileNonexistent]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.DisplayBinaryFileNonexistent]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.DisplayBinaryFileNonexistent]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[MainTest.DisplayBinaryFileValid]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.DisplayBinaryFileValid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.DisplayBinaryFileValid]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[MainTest.DisplayTextFileNonexistent]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.DisplayTextFileNonexistent]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.DisplayTextFileNonexistent]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[MainTest.DisplayTextFileValid]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.DisplayTextFileValid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.DisplayTextFileValid]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[MainTest.StartProcessInvalid]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.StartProcessInvalid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.StartProcessInvalid]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test([=[MainTest.StartProcessValid]=]  D:/Code/OS/Lab1_1/out/build/x64-Debug/main_test.exe [==[--gtest_filter=MainTest.StartProcessValid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[MainTest.StartProcessValid]=]  PROPERTIES WORKING_DIRECTORY D:/Code/OS/Lab1_1/out/build/x64-Debug SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  main_test_TESTS MainTest.DisplayBinaryFileNonexistent MainTest.DisplayBinaryFileValid MainTest.DisplayTextFileNonexistent MainTest.DisplayTextFileValid MainTest.StartProcessInvalid MainTest.StartProcessValid)
