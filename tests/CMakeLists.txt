
# https://qiita.com/mrk_21/items/25ee7f00cebb9934b472
add_executable(minecraftTest minecraftTest.c simpletestcases.c)
target_link_libraries(minecraftTest cunit study)
add_test(NAME mc_seed COMMAND $<TARGET_FILE:minecraftTest>)

#add_custom_target(test.sh)
#add_custom_command(TARGET test.sh POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tests/test.sh ${CMAKE_SOURCE_DIR}/test.sh )
#add_test(NAME test.sh COMMAND ./test.sh)

#add_custom_target(xfail.sh)
#add_custom_command(TARGET xfail.sh POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/tests/xfail.sh ${CMAKE_SOURCE_DIR}/xfail.sh )
#add_test(NAME xfail.sh COMMAND ./xfail.sh)
#set_tests_properties(xfail.sh PROPERTIES WILL_FAIL true)

add_executable(epsp_parser_test epsp-parser-test.c simpletestcases.c)
target_link_libraries(epsp_parser_test cunit study)
add_test(NAME hoge_epsp_parser_test COMMAND $<TARGET_FILE:epsp_parser_test>)

add_executable(string_list_test string_list_test.c)
target_link_libraries(string_list_test cunit study)
add_test(NAME string_list_test COMMAND $<TARGET_FILE:string_list_test>)

add_executable(changebasetest changebasetest.c)
target_link_libraries(changebasetest cunit study)
add_test(NAME changebasetest COMMAND $<TARGET_FILE:changebasetest>)

add_executable(ripeencode ripeencode.c)
target_link_libraries(ripeencode cunit study)
add_test(NAME ripeencode COMMAND $<TARGET_FILE:ripeencode>)

add_executable(base58test base58test.c)
target_link_libraries(base58test cunit study)
add_test(NAME base58test COMMAND $<TARGET_FILE:base58test>)

add_executable(testChainCookies testChainCookies.c testChainCookies2.c)
target_link_libraries(testChainCookies cunit study m)
#target_compile_options(testChainCookies PRIVATE -fprofile-arcs -ftest-coverage --coverage)
add_test(NAME hoge_testChainCookies COMMAND $<TARGET_FILE:testChainCookies>)
