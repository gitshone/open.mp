find_program(MAKENSIS makensis)
if(NOT MAKENSIS)
	message(WARNING "makensis not found; skipping installer target.")
	return()
endif()

set(NSI ${CMAKE_CURRENT_SOURCE_DIR}/openmp-client.nsi)

add_custom_target(ClientInstaller
	COMMAND ${MAKENSIS}
		-DLOADER_EXE=$<TARGET_FILE:Loader>
		-DCLIENT_DLL=$<TARGET_FILE:Client>
		-DRESOURCES_DIR=${CMAKE_SOURCE_DIR}/Client/Resources
		${NSI}
	DEPENDS Loader Client
	COMMENT "Building open.mp client installer"
	VERBATIM)
