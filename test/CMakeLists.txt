itk_module_test()

set(EntropyBasedIntensityCorrectionTests
  itkEntropyBasedIntensityCorrectionImageFilterTest.cxx
  )

CreateTestDriver(EntropyBasedIntensityCorrection "${EntropyBasedIntensityCorrection-Test_LIBRARIES}" "${EntropyBasedIntensityCorrectionTests}")

itk_add_test(NAME itkEntropyBasedIntensityCorrectionImageFilterTest
  COMMAND EntropyBasedIntensityCorrectionTestDriver
    # --compare DATA{Baseline/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha}
    # ${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha
  itkEntropyBasedIntensityCorrectionImageFilterTest
    DATA{Input/tass.png}
    ${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha
  )
