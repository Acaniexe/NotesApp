cmake_minimum_required(VERSION 3.10)

project(MySDLApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SDL_ARCH x86_64-w64-mingw32)

file(GLOB SRC_FILES "src/*.cpp")

add_executable(app ${SRC_FILES})

target_include_directories(app PRIVATE
    SDL3/include
    ${CMAKE_SOURCE_DIR}/include
    SDL3_ttf/${SDL_ARCH}/include
    SDL3_image/${SDL_ARCH}/include
)

target_link_directories(app PRIVATE
    SDL3/lib
    SDL3_ttf/${SDL_ARCH}/lib
    SDL3_image/${SDL_ARCH}/lib
)

target_link_libraries(app
    SDL3
    SDL3_ttf
    SDL3_image
)

#add_custom_command(TARGET app POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#        ${CMAKE_SOURCE_DIR}/SDL3/lib/../SDL3.dll
#        ${CMAKE_SOURCE_DIR}/SDL3_ttf/${SDL_ARCH}/bin/SDL3_ttf.dll
#        ${CMAKE_SOURCE_DIR}/SDL3_image/${SDL_ARCH}/bin/SDL3_image.dll
#        $<TARGET_FILE_DIR:app>
#)