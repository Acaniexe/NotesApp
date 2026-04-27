cmake_minimum_required(VERSION 3.10)

project(MySDLApp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(SDL_ARCH x86_64-w64-mingw32)

file(GLOB SRC_FILES
     "src/*.cpp"
     "systems/*.cpp"
)

add_executable(app ${SRC_FILES})

# INCLUDE PATHS
target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/SDL3/${SDL_ARCH}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/SDL3_ttf/${SDL_ARCH}/include
    ${CMAKE_SOURCE_DIR}/SDL3_image/${SDL_ARCH}/include
)

# LINK PATHS
target_link_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/SDL3/${SDL_ARCH}/lib
    ${CMAKE_SOURCE_DIR}/SDL3_ttf/${SDL_ARCH}/lib
    ${CMAKE_SOURCE_DIR}/SDL3_image/${SDL_ARCH}/lib
)

target_link_libraries(app
    SDL3
    SDL3_ttf
    SDL3_image
)

# COPY DLLs
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_SOURCE_DIR}/bin

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/SDL3/${SDL_ARCH}/bin/SDL3.dll
        ${CMAKE_SOURCE_DIR}/bin/SDL3.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/SDL3_ttf/${SDL_ARCH}/bin/SDL3_ttf.dll
        ${CMAKE_SOURCE_DIR}/bin/SDL3_ttf.dll

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/SDL3_image/${SDL_ARCH}/bin/SDL3_image.dll
        ${CMAKE_SOURCE_DIR}/bin/SDL3_image.dll
)

# COPY ASSETS
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:app>/assets
)