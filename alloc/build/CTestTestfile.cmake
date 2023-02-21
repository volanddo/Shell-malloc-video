# CMake generated Testfile for 
# Source directory: /user/7/volandd/Documents/2A/SEPC/tp1alloc
# Build directory: /user/7/volandd/Documents/2A/SEPC/tp1alloc/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(AllTestsAllocator "alloctest")
set_tests_properties(AllTestsAllocator PROPERTIES  _BACKTRACE_TRIPLES "/user/7/volandd/Documents/2A/SEPC/tp1alloc/CMakeLists.txt;67;add_test;/user/7/volandd/Documents/2A/SEPC/tp1alloc/CMakeLists.txt;0;")
subdirs("gtest")
