
add_executable (rakconvert rakconvert.C)
add_executable (rakverb rakverb.C)
add_executable (rakverb2 rakverb2.C)
add_executable (rakgit2new rakgit2new.C)

target_include_directories (rakverb PRIVATE  ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(rakverb PRIVATE ${SNDFILE_LIBRARIES} )
target_include_directories (rakverb2 PRIVATE  ${SNDFILE_INCLUDE_DIRS})
target_link_libraries(rakverb2 PRIVATE ${SNDFILE_LIBRARIES} )

install (TARGETS rakconvert rakverb rakverb2 rakgit2new RUNTIME DESTINATION bin)

install (FILES InsertPresets.rkis
    DESTINATION share/rakarrack-plus)
