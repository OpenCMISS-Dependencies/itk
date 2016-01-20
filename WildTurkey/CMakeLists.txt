IF(NOT ITK_INSTALL_NO_DEVELOPMENT)
  INSTALL(EXPORT itk-config DESTINATION ${ITK_INSTALL_PACKAGE_DIR_CM24})
  SET(_TARGET_LIST itksys itkjpeg8 itkjpeg12 itkjpeg16 itkopenjpeg ITKMetaIO ITKNrrdIO ITKEXPAT
    ITKznz ITKniftiio ITKDICOMParser itkvcl itkv3p_netlib itkv3p_lsqr itktestlib itkvnl itkvnl_algo
    itkNetlibSlatec itkgdcm itkvnl_inst ITKCommon ITKNumerics ITKBasicFilters ITKStatistics 
    ITKAlgorithms ITKSpatialObject ITKIO ITKFEM ITKIOReview ITKTransformIOReview ITKIOMINC2 ITKQuadEdgeMesh)
  ADD_LIBRARY(itk INTERFACE)
  FOREACH(_TARGET ${_TARGET_LIST})
    IF(TARGET ${_TARGET})
      LIST(APPEND _ACTIVE_TARGETS ${_TARGET})
    ENDIF()
  ENDFOREACH()
  TARGET_LINK_LIBRARIES(itk INTERFACE ${_ACTIVE_TARGETS})
  # The relative include directories need to be set for the install interface only
  foreach(_DIR ${ITK_INCLUDE_RELATIVE_DIRS})
      target_include_directories(itk INTERFACE $<INSTALL_INTERFACE:include/InsightToolkit/${_DIR}>)
  endforeach()
  INSTALL(TARGETS itk
    EXPORT itk-config
    INCLUDES DESTINATION include/InsightToolkit)
  include(CMakePackageConfigHelpers)
  WRITE_BASIC_PACKAGE_VERSION_FILE(${CMAKE_CURRENT_BINARY_DIR}/itk-config-version.cmake
        COMPATIBILITY AnyNewerVersion)
  install(FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/itk-config-version.cmake
    DESTINATION ${ITK_INSTALL_PACKAGE_DIR_CM24})
ENDIF(NOT ITK_INSTALL_NO_DEVELOPMENT)