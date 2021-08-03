
# https://qiita.com/mrk_21/items/25ee7f00cebb9934b472
add_executable(minecraftTest minecraftTest.c)
add_test(minecraftTest COMMAND $<TARGET_FILE:minecraftTest>)
