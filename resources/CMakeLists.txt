set( CF3_RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Dir for resources" )

list( APPEND resources_files
   hextet.neu
   quadtriag.neu
   rotation-tg-p1.neu
   rotation-qd-p1.neu
   rectangle-tg-p1.msh
   rectangle-tg-p2.msh
   rectangle-qd-p1.msh
   rectangle-qd-p2.msh
   rectangle-mix-p1.msh
   rectangle-mix-p2.msh
   sinusbump-tg-p1.msh
)

set( CF3_RESOURCES_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR} CACHE INTERNAL "build dir" )

# copy files to test dir in build tree
foreach( rfile ${resources_files} )
  add_custom_command( OUTPUT  ${rfile}
                      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${rfile} ${CF3_RESOURCES_BINARY_DIR} )
                      #COMMENT "Copying file ${rfile} to ${CF3_RESOURCES_BINARY_DIR}")
endforeach()

add_custom_target( copy-resources ALL DEPENDS ${resources_files} )
