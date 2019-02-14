itk_module_test()

set(EntropyBasedIntensityCorrectionImageFilterTests
  itkEntropyBasedIntensityCorrectionImageFilterTest.cxx
  )

CreateTestDriver(EntropyBasedIntensityCorrectionImageFilter "${EntropyBasedIntensityCorrectionImageFilter-Test_LIBRARIES}" "${EntropyBasedIntensityCorrectionImageFilterTests}")

itk_add_test(NAME itkEntropyBasedIntensityCorrectionImageFilterTest
  COMMAND EntropyBasedIntensityCorrectionImageFilterTestDriver
    --compare
    #${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha
    #DATA{Baseline/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha}
  itkEntropyBasedIntensityCorrectionImageFilterTest
    DATA{Input/tass.png}
    ${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionImageFilterTestOutput.mha
  )
