
# Project source code list
SET( SHADOWDEMO_SOURCES
  # Add all the cpp source files here
  main.cpp
  ShadowMapBuilder.cpp
  ShadowMapRenderer.cpp
  ShadowMapSetup.cpp
  ShadowMapRenderingView.cpp
)

# todo get rid of this!@#!
# Include if using SDL under Mac OS X
IF(APPLE)
  SET(SHADOWDEMO_SOURCES ${SHADOWDEMO_SOURCES} ${SDL_MAIN_FOR_MAC})
ENDIF(APPLE)

# Project executable
ADD_EXECUTABLE(ShadowDemo ${SHADOWDEMO_SOURCES})

# Project dependencies
TARGET_LINK_LIBRARIES(ShadowDemo
  OpenEngine_Utils
  # Extensions
  OpenEngine_Geometry
  Extensions_SetupHelpers
  Extensions_FixedTimeStepPhysics
  Extensions_GenericHandlers
  ${BOOST_SERIALIZATION_LIB}
)
