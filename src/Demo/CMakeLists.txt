option(BUILD_DEMO "Quick guide on how to use Cala" ON)

if (${BUILD_DEMO})
    add_executable(Demo
        Demo.cpp
        DemoApplication.h DemoApplication.cpp
    )

    target_link_libraries(Demo
        PRIVATE
            Cala
    )

    set(texturesZip ${CMAKE_CURRENT_SOURCE_DIR}/textures.zip)

    if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/textures)
        execute_process(
            COMMAND curl --output ${texturesZip} https://opengameart.org/sites/default/files/50_free_textures_5.zip
        )
        if (WIN32)
            execute_process(
                COMMAND PowerShell Expand-Archive -Path ${texturesZip} -DestinationPath ${CMAKE_CURRENT_SOURCE_DIR}/textures
                COMMAND del ${texturesZip}
            )
        elseif(UNIX)
            execute_process(
                COMMAND unzip ${texturesZip} -d ${CMAKE_CURRENT_SOURCE_DIR}/textures
                COMMAND rm ${texturesZip}
            )
        endif()
    endif()
endif()