itk_module_test()

set(EntropyBasedIntensityCorrectionTests
  itkMinimalStandardRandomVariateGeneratorTest.cxx
  itkNormalDistributionImageSourceTest.cxx
  itkEntropyBasedIntensityCorrectionTest.cxx
  )

CreateTestDriver(EntropyBasedIntensityCorrection "${EntropyBasedIntensityCorrection-Test_LIBRARIES}" "${EntropyBasedIntensityCorrectionTests}")

itk_add_test(NAME itkMinimalStandardRandomVariateGeneratorTest
  COMMAND EntropyBasedIntensityCorrectionTestDriver itkMinimalStandardRandomVariateGeneratorTest
  )

itk_add_test(NAME itkEntropyBasedIntensityCorrectionTest
  COMMAND EntropyBasedIntensityCorrectionTestDriver
    --compare
    ${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionTestOutput.mha
    DATA{Baseline/itkEntropyBasedIntensityCorrectionTestOutput.mha}
  itkEntropyBasedIntensityCorrectionTest
    ${ITK_TEST_OUTPUT_DIR}/itkEntropyBasedIntensityCorrectionTestOutput.mha
  )

itk_add_test(NAME itkNormalDistributionImageSourceTest
  COMMAND EntropyBasedIntensityCorrectionTestDriver --without-threads
  --compare
    ${ITK_TEST_OUTPUT_DIR}/itkNormalDistributionImageSourceTestOutput.mha
    DATA{Baseline/itkNormalDistributionImageSourceTestOutput.mha}
  itkNormalDistributionImageSourceTest
    ${ITK_TEST_OUTPUT_DIR}/itkNormalDistributionImageSourceTestOutput.mha
  )
