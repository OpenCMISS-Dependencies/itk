set(ITKIOGIPL_SRC
itkGiplImageIOFactory.cxx
itkGiplImageIO.cxx
)

add_library(ITKIOGIPL ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOGIPL_SRC})
if (ITK_USE_SYSTEM_ZLIB)
  find_package(ZLIB REQUIRED)
  if (TARGET zlib)
    target_link_libraries(ITKIOGIPL PUBLIC zlib)
  endif ()
endif ()

itk_module_link_dependencies()
itk_module_target(ITKIOGIPL)
