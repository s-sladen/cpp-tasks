project(tests)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(bigint-test bigint.cpp)
add_executable(smart_ptr-test smart_ptr.cpp)
add_executable(graph-test graph.cpp)

target_link_libraries(bigint-test tasks ${Boost_LIBRARIES})
target_link_libraries(smart_ptr-test tasks ${Boost_LIBRARIES})
target_link_libraries(graph-test tasks ${Boost_LIBRARIES})

add_test(NAME BigInt COMMAND bigint-test)
add_test(NAME SmartPtr COMMAND smart_ptr-test)
add_test(NAME Graph COMMAND graph-test)