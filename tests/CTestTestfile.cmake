# CMake generated Testfile for 
# Source directory: /root/varcoin_lib/tests
# Build directory: /root/varcoin_lib/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(coretests "coretests" "--generate_and_play_test_data")
add_test(crypto "crypto-tests" "/root/varcoin_lib/tests/crypto/tests.txt")
add_test(difficulty "difficulty-tests" "/root/varcoin_lib/tests/difficulty/data.txt")
add_test(hash-fast "hash-tests" "fast" "/root/varcoin_lib/tests/hash/tests-fast.txt")
add_test(hash-slow "hash-tests" "slow" "/root/varcoin_lib/tests/hash/tests-slow.txt")
add_test(hash-tree "hash-tests" "tree" "/root/varcoin_lib/tests/hash/tests-tree.txt")
add_test(hash-extra-blake "hash-tests" "extra-blake" "/root/varcoin_lib/tests/hash/tests-extra-blake.txt")
add_test(hash-extra-groestl "hash-tests" "extra-groestl" "/root/varcoin_lib/tests/hash/tests-extra-groestl.txt")
add_test(hash-extra-jh "hash-tests" "extra-jh" "/root/varcoin_lib/tests/hash/tests-extra-jh.txt")
add_test(hash-extra-skein "hash-tests" "extra-skein" "/root/varcoin_lib/tests/hash/tests-extra-skein.txt")
add_test(hash-target "hash-target-tests")
subdirs(gtest)
