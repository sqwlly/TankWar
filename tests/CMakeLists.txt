# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# Find or fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Collect test source files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Collect necessary source files for testing (excluding main.cpp)
set(GAME_SOURCES
    ${SRC_DIR}/entities/base/Entity.cpp
    ${SRC_DIR}/entities/tanks/Tank.cpp
    ${SRC_DIR}/entities/tanks/PlayerTank.cpp
    ${SRC_DIR}/entities/tanks/EnemyTank.cpp
    ${SRC_DIR}/entities/projectiles/Bullet.cpp
    ${SRC_DIR}/entities/terrain/Terrain.cpp
    ${SRC_DIR}/entities/terrain/BrickWall.cpp
    ${SRC_DIR}/entities/terrain/SteelWall.cpp
    ${SRC_DIR}/entities/terrain/Water.cpp
    ${SRC_DIR}/entities/terrain/Grass.cpp
    ${SRC_DIR}/entities/terrain/Base.cpp
    ${SRC_DIR}/collision/CollisionManager.cpp
    ${SRC_DIR}/collision/handlers/BulletTankHandler.cpp
    ${SRC_DIR}/collision/handlers/BulletTerrainHandler.cpp
    ${SRC_DIR}/collision/handlers/BulletBulletHandler.cpp
    ${SRC_DIR}/collision/handlers/TankTankHandler.cpp
    ${SRC_DIR}/collision/handlers/TankTerrainHandler.cpp
    ${SRC_DIR}/level/Level.cpp
    ${SRC_DIR}/level/LevelLoader.cpp
    ${SRC_DIR}/states/GameStateManager.cpp
    ${SRC_DIR}/states/PlayingState.cpp
    ${SRC_DIR}/states/MenuState.cpp
    ${SRC_DIR}/states/StageState.cpp
    ${SRC_DIR}/states/ScoreState.cpp
    ${SRC_DIR}/states/ConstructionState.cpp
    ${SRC_DIR}/input/InputManager.cpp
    ${SRC_DIR}/ui/GameHUD.cpp
    ${SRC_DIR}/ai/AIBehavior.cpp
)

# Create test executable
add_executable(TankGameTests ${TEST_SOURCES} ${GAME_SOURCES})

target_include_directories(TankGameTests PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/tests
)

target_link_libraries(TankGameTests PRIVATE
    GTest::gtest_main
    GTest::gmock
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,$<IF:$<TARGET_EXISTS:SDL2::SDL2-static>,SDL2::SDL2-static,SDL2>>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image-static>,SDL2_image::SDL2_image-static,SDL2_image>>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,$<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer-static>,SDL2_mixer::SDL2_mixer-static,SDL2_mixer>>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf-static>,SDL2_ttf::SDL2_ttf-static,SDL2_ttf>>
)

# Enable testing
include(GoogleTest)
gtest_discover_tests(TankGameTests)
