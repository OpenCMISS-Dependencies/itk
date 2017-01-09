set(ITKIOPNG_SRC
itkPNGImageIOFactory.cxx
itkPNGImageIO.cxx
)

add_library(ITKIOPNG ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOPNG_SRC})

# If we are using this with find module wrappers
if(ITK_USE_SYSTEM_PNG)
  find_package(PNG REQUIRED)
  target_link_libraries(ITKIOPNG PUBLIC png)
endif()

itk_module_link_dependencies()
itk_module_target(ITKIOPNG)
