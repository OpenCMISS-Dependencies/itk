set(ITKIOJPEG_SRC
itkJPEGImageIOFactory.cxx
itkJPEGImageIO.cxx
)

add_library(ITKIOJPEG ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOJPEG_SRC})

if (ITK_USE_SYSTEM_JPEG)
  find_package(JPEG REQUIRED)
  if (TARGET jpeg)
    target_link_libraries(ITKIOJPEG PUBLIC jpeg)
    message(STATUS "ITKIOJPEG PUBLIC: jpeg")
  endif ()
endif ()

itk_module_link_dependencies()
itk_module_target(ITKIOJPEG)
