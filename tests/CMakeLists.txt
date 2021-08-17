
# https://qiita.com/mrk_21/items/25ee7f00cebb9934b472
add_executable(minecraftTest minecraftTest.c)
target_link_libraries(minecraftTest cunit study)
add_executable(aestest aestest.c)
add_test(NAME minecraftTest COMMAND minecraftTest)
add_test(NAME aestest COMMAND aestest)
add_test(NAME test.sh COMMAND test.sh )
add_test(xfail.sh COMMAND xfail.sh)
set_tests_properties(xfail.sh PROPERTIES WILL_FAIL true)
