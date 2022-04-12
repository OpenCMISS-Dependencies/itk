set(ITKIOGDCM_SRC
  itkGDCMImageIO.cxx
  itkGDCMImageIOFactory.cxx
  itkGDCMSeriesFileNames.cxx
)

add_library(ITKIOGDCM  ${ITK_LIBRARY_BUILD_TYPE} ${ITKIOGDCM_SRC})

if (ITK_USE_SYSTEM_GDCM)
  find_package(GDCM-ABI REQUIRED CONFIG)
  if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(ITKIOGDCM PUBLIC -Wl,--start-group gdcmDICT gdcmMSFF -Wl,--end-group)
  else ()
    target_link_libraries(ITKIOGDCM PUBLIC gdcmDICT gdcmMSFF)
  endif ()
endif ()

itk_module_link_dependencies()
itk_module_target(ITKIOGDCM)
