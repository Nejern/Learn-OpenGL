cmake_minimum_required(VERSION 3.15)

# Название проекта и версия
project(Learn_OpenGL VERSION 1.0 LANGUAGES C CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Генерация compile_commands.json для clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Настройки компиляции и линковки
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-ldl -lglfw -lassimp -Wall -Wextra -Wfatal-errors -Wpedantic -Wconversion -Wshadow")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ldl -lglfw -lassimp")

# Настройки для исполняемого файла
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Проверка зависимостей
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Настройки директорий
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

# Сбор исходных файлов
file(GLOB INCLUDES
    ${INC_DIR}
    ${INC_DIR}/imgui
)
file(GLOB SOURCES
    ${SRC_DIR}/*
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Линковка библиотек
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
