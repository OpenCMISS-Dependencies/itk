set(ITKSpatialObjects_SRC
itkMetaEvent.cxx
itkCylinderSpatialObject.cxx
itkSpatialObjectFactoryBase.cxx
itkArrowSpatialObject.cxx
)

add_library(ITKSpatialObjects ${ITKSpatialObjects_SRC})
if (ITK_USE_SYSTEM_ZLIB)
  find_package(ZLIB REQUIRED)
  if (TARGET zlib)
    target_link_libraries(ITKSpatialObjects PUBLIC zlib)
  endif ()
endif ()

itk_module_link_dependencies()
itk_module_target(ITKSpatialObjects)
