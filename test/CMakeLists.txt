project(tests)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(bigint-test bigint.cpp)
add_executable(smart_ptr-test smart_ptr.cpp)

target_link_libraries(bigint-test ${Boost_LIBRARIES})
target_link_libraries(smart_ptr-test ${Boost_LIBRARIES})

add_test(NAME BigInt COMMAND bigint-test)
add_test(NAME SmartPtr COMMAND smart_ptr-test)