option(BUILD_CALA_DEMO "Quick guide on how to use Cala and show its features" ON)

if (${BUILD_CALA_DEMO})
    add_executable(CalaDemo
        main.cpp
        DemoApplication.h DemoApplication.cpp
    )

    target_link_libraries(CalaDemo
        PRIVATE
            Cala
    )
endif()