get_filename_component(MY_CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(READ "${MY_CURRENT_DIR}/README.rst" DOCUMENTATION)

# define the dependencies of the include module and the tests
itk_module(EntropyBasedIntensityCorrection
  DEPENDS
    ITKCommon
    ITKStatistics
    ITKOptimizersv4
    ITKIOTransformBase # debugging
  COMPILE_DEPENDS
    ITKImageSources
    ITKImageGrid
  TEST_DEPENDS
    ITKTestKernel
    ITKIOTransformInsightLegacy
  DESCRIPTION
    "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
)
