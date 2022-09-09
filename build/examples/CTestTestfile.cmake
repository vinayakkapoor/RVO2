# CMake generated Testfile for 
# Source directory: /home/vinayak/thesis/RVO2/examples
# Build directory: /home/vinayak/thesis/RVO2/build/examples
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Blocks "/home/vinayak/thesis/RVO2/build/examples/Blocks")
set_tests_properties(Blocks PROPERTIES  LABELS "medium" TIMEOUT "60")
add_test(Circle "/home/vinayak/thesis/RVO2/build/examples/Circle")
set_tests_properties(Circle PROPERTIES  LABELS "medium" TIMEOUT "60")
add_test(Roadmap "/home/vinayak/thesis/RVO2/build/examples/Roadmap")
set_tests_properties(Roadmap PROPERTIES  LABELS "medium" TIMEOUT "60")
