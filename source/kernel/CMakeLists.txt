project(kernel LANGUAGES C)
 
# 使用自定义的链接器生成elf 文件 PROJECT_SOURCE_DIR 是项目目录
set(CMAKE_EXE_LINKER_FLAGS "-m elf_i386  -T ${PROJECT_SOURCE_DIR}/kernel.lds")
set(CMAKE_C_LINK_EXECUTABLE "${LINKER_TOOL} <OBJECTS> ${CMAKE_EXE_LINKER_FLAGS} -o ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf")

# 将所有的汇编、C文件加入工程
# 注意保证start.asm在最前头，这样对应的程序会放在bin文件开头，从而在加载到0x7c00时能在开头处
file(GLOB_RECURSE C_LIST "*.c" "*.h")
add_executable(${PROJECT_NAME} init/start.S ${C_LIST})

# 这里直接使用都elf文件，bin 文件虽然生成但是没有使用
# bin文件生成，写入到image目录下 并进行反汇编  反编译可以使用 i386 32位模式
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND cp ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/image/${PROJECT_NAME}.elf
                   COMMAND ${OBJCOPY_TOOL} -O binary ${PROJECT_NAME}.elf ${CMAKE_SOURCE_DIR}/image/${PROJECT_NAME}.bin
                   COMMAND ${OBJDUMP_TOOL} -x -d -S -m i386 ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_dis.txt
                   COMMAND ${READELF_TOOL} -a ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf > ${PROJECT_NAME}_elf.txt
)