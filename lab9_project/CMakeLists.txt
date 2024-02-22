add_executable(lab9.elf main.c eggmanControl.c eggman.c)
target_link_libraries(lab9.elf ${330_LIBS} touchscreen interrupts intervalTimer)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)