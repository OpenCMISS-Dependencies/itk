set(ITKIOTIFF_SRC
itkTIFFImageIO.cxx
itkTIFFReaderInternal.cxx
itkTIFFImageIOFactory.cxx
)

add_library(ITKIOTIFF ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOTIFF_SRC})

# If we are using this with find module wrappers
if(ITK_USE_SYSTEM_TIFF)
  find_package(TIFF REQUIRED)
  target_link_libraries(ITKIOTIFF PUBLIC tiff)
endif()

itk_module_link_dependencies()
itk_module_target(ITKIOTIFF)
