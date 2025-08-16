# CMake generated Testfile for 
# Source directory: /media/data/programming/v004beta/tests
# Build directory: /media/data/programming/v004beta/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(astro_tests_core "/media/data/programming/v004beta/build/tests/astro_tests_core")
set_tests_properties(astro_tests_core PROPERTIES  _BACKTRACE_TRIPLES "/media/data/programming/v004beta/tests/CMakeLists.txt;25;add_test;/media/data/programming/v004beta/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
