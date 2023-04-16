option(BUILD_CALA_DEMO "Quick guide on how to use Cala and show its features" ON)

if (${BUILD_CALA_DEMO})
    add_executable(Demo
        Demo.cpp
        DemoApplication.h DemoApplication.cpp
    )

    target_link_libraries(Demo
        PRIVATE
            Cala
    )
endif()