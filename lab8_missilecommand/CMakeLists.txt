add_executable(lab8_m1.elf main_m1.c missile.c)
target_link_libraries(lab8_m1.elf ${330_LIBS} touchscreen interrupts intervalTimer)
set_target_properties(lab8_m1.elf PROPERTIES LINKER_LANGUAGE CXX)

add_executable(lab8_m2.elf main_m2.c gameControl.c missile.c)
target_link_libraries(lab8_m2.elf ${330_LIBS} touchscreen interrupts intervalTimer)
set_target_properties(lab8_m2.elf PROPERTIES LINKER_LANGUAGE CXX)

#add_executable(lab8_m3.elf main_m3.c minimax.c ticTacToeControl.c ticTacToeDisplay.c)
#target_link_libraries(lab8_m3.elf ${330_LIBS} touchscreen buttons_switches interrupts intervalTimer)
#set_target_properties(lab8_m3.elf PROPERTIES LINKER_LANGUAGE CXX)
