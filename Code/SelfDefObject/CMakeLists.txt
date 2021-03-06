CMAKE_MINIMUM_REQUIRED(VERSION 3.15.0 FATAL_ERROR)
SET(PROJECTNAME SelfDefObject)
PROJECT(${PROJECTNAME})
ADD_DEFINITIONS("-DSELFDEFINEOBJ_API")
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

IF(NOT DEFINED Qt5_DIR)
	SET(Qt5_DIR "Qt5_DIR-NOTFOUND" CACHE PATH "Qt5_DIR")
ENDIF()
IF(Qt5_DIR STREQUAL "Qt5_DIR-NOTFOUND")		
	MESSAGE(FATAL_ERROR "Qt5_DIR is null, please specified Qt5Config.cmake location!~")
ENDIF()

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin)
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin_d)    
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin) 
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin_d)   
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../../output/bin)
ENDIF()

SET(QF_QRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../qrc/qianfan.qrc")
SET(TRAN_QRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../qrc/translations.qrc")
#FILE(GLOB SelfDefObject_TS_FILE "${TOP_CMAKE_CURRENT_SOURCE_DIR}/qrc/translations/SelfDefObject_zh_CN.ts")
FILE(GLOB SRC_UI_FILES "*.ui")
FILE(GLOB SRC_H_FILES "*.h")
FILE(GLOB SRC_CPP_FILES "*.cpp")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/..)

#FIND_PACKAGE(Qt5 COMPONENTS Widgets Gui Core LinguistTools REQUIRED)
FIND_PACKAGE(Qt5 COMPONENTS Widgets Gui Core REQUIRED)
#SET_SOURCE_FILES_PROPERTIES(${SelfDefObject_TS_FILE} PROPERTIES OUTPUT_LOCATION "${TOP_CMAKE_CURRENT_SOURCE_DIR}/qrc/translations")
#QT5_CREATE_TRANSLATION(SelfDefObject_QM_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${SelfDefObject_TS_FILE} OPTIONS -noobsolete)
QT5_ADD_RESOURCES(QFTRAN_Generated_QRC_FILE ${QF_QRC_FILE} ${TRAN_QRC_FILE})
QT5_WRAP_UI(SRC_Generated_UI_FILES ${SRC_UI_FILES})
SET(CMAKE_AUTOMOC ON)
#SOURCE_GROUP("Translation" FILES ${SelfDefObject_QM_FILE} ${SelfDefObject_TS_FILE})
SOURCE_GROUP("rc" FILES ${QFTRAN_Generated_QRC_FILE} ${QF_QRC_FILE} ${TRAN_QRC_FILE})
SOURCE_GROUP("Ui" FILES ${SRC_Generated_UI_FILES} ${SRC_UI_FILES})
SOURCE_GROUP("Generated Files" FILES "${PROJECT_BINARY_DIR}/${PROJECTNAME}_autogen/mocs_compilation.cpp")

ADD_LIBRARY(${PROJECTNAME} SHARED #${SelfDefObject_TS_FILE}
                                  #${SelfDefObject_QM_FILE}
								  ${QFTRAN_Generated_QRC_FILE}
								  ${SRC_Generated_UI_FILES}								   
								  ${SRC_H_FILES}
								  ${SRC_CPP_FILES})								   
TARGET_LINK_LIBRARIES(${PROJECTNAME} Qt5::Widgets Qt5::Gui Qt5::Core)
IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
	TARGET_LINK_LIBRARIES(${PROJECTNAME} debug "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/libSettings${CMAKE_DEBUG_POSTFIX}.so"     optimized "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}/libSettings${CMAKE_RELEASE_POSTFIX}.so"
										 debug "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/libPythonModule${CMAKE_DEBUG_POSTFIX}.so" optimized "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}/libPythonModule${CMAKE_RELEASE_POSTFIX}.so"
										 debug "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}/libDataProperty${CMAKE_DEBUG_POSTFIX}.so" optimized "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}/libDataProperty${CMAKE_RELEASE_POSTFIX}.so")											 
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
	TARGET_LINK_LIBRARIES(${PROJECTNAME} debug "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}/Settings${CMAKE_DEBUG_POSTFIX}.lib"     optimized "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}/Settings${CMAKE_RELEASE_POSTFIX}.lib"
										 debug "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}/PythonModule${CMAKE_DEBUG_POSTFIX}.lib" optimized "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}/PythonModule${CMAKE_RELEASE_POSTFIX}.lib"
										 debug "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}/DataProperty${CMAKE_DEBUG_POSTFIX}.lib" optimized "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}/DataProperty${CMAKE_RELEASE_POSTFIX}.lib")	
ENDIF()			
							 
IF(TARGET SolutionDataIO)	
	ADD_DEPENDENCIES(${PROJECTNAME} Settings PythonModule DataProperty)
ENDIF()