if(EXISTS "D:/Code/OS/Lab2/out/build/x64-Debug/tests98[1]_tests.cmake")
  include("D:/Code/OS/Lab2/out/build/x64-Debug/tests98[1]_tests.cmake")
else()
  add_test(tests98_NOT_BUILT tests98_NOT_BUILT)
endif()
