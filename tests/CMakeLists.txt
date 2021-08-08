
# https://qiita.com/mrk_21/items/25ee7f00cebb9934b472
add_executable(minecraftTest minecraftTest.c)
target_link_libraries(minecraftTest cunit study)
add_executable(aestest aestest.c)
add_test(NAME minecraftTest COMMAND minecraftTest)
add_test(NAME aestest COMMAND aestest)
