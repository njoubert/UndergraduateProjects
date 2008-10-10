Find_Package(OpenGL)
Find_Package(GLUT)

set (CMAKE_CXX_FLAGS "-DNDEBUG -D_CONSOLE -DUSE_DOUBLE -DNOOMP -O3")
include_directories (${NEEDLESIM_SOURCE_DIR}/Src ${NEEDLESIM_SOURCE_DIR} ${NEEDLESIM_SOURCE_DIR}/WildMagic4/LibFoundation ${NEEDLESIM_SOURCE_DIR}/WildMagic4/LibFoundation/System ${NEEDLESIM_SOURCE_DIR}/WildMagic4/LibFoundation/Mathematics ${NEEDLESIM_SOURCE_DIR}/Eigen ${NEEDLESIM_SOURCE_DIR}/Src/Star ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
link_directories (${NEEDLESIM_BINARY_DIR}/Src)
add_executable (needlesimexe.exe main_nographics.cpp)
target_link_libraries ( needlesimexe.exe needlesimlib ${SADSDS} ${GLUT_glut_LIBRARY} ${OPENGL_LIBRARIES} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

