/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkEntropyBasedIntensityCorrectionImageFilter_hxx
#define itkEntropyBasedIntensityCorrectionImageFilter_hxx

#include "itkEntropyBasedIntensityCorrectionImageFilter.h"

#include "itkImageEntropyCostFunction.h"
#include "itkPowellOptimizerv4.h"

namespace itk
{

template <typename TInputImage, typename TMaskImage, typename TOutputImage>
EntropyBasedIntensityCorrectionImageFilter<TInputImage, TMaskImage, TOutputImage>::
  EntropyBasedIntensityCorrectionImageFilter()
{
  this->m_BSplineFilter = BSplineFilterType::New();
  this->m_ImageToHistogramFilter = ImageToHistogramFilterType::New();
}


template <typename TInputImage, typename TMaskImage, typename TOutputImage>
void
EntropyBasedIntensityCorrectionImageFilter<TInputImage, TMaskImage, TOutputImage>::PrintSelf(std::ostream & os,
                                                                                             Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


template <typename TInputImage, typename TMaskImage, typename TOutputImage>
void
EntropyBasedIntensityCorrectionImageFilter<TInputImage, TMaskImage, TOutputImage>::EnlargeOutputRequestedRegion(
  DataObject * output)
{
  Superclass::EnlargeOutputRequestedRegion(output);

  output->SetRequestedRegionToLargestPossibleRegion();
}


template <typename TInputImage, typename TMaskImage, typename TOutputImage>
void
EntropyBasedIntensityCorrectionImageFilter<TInputImage, TMaskImage, TOutputImage>::GenerateData()
{
  this->AllocateOutputs();

  OutputImageType * output = this->GetOutput();
  InputImageType *  input = const_cast<InputImageType *>(this->GetInput());

  using EntropyCostType = ImageEntropyCostFunction<InputImageType, MaskImageType, OutputImageType>;
  typename EntropyCostType::Pointer entropyCost = EntropyCostType::New();
  entropyCost->SetImage(input);

  using OptimizerType = PowellOptimizerv4<typename EntropyCostType::ParametersValueType>;
  typename OptimizerType::Pointer optimizer = OptimizerType::New();

  optimizer->SetMetric(entropyCost);
  // optimizer->SetDoEstimateScales(false);
  optimizer->SetNumberOfIterations(10);
  optimizer->SetStepTolerance(0.1);
  optimizer->SetStepLength(0.5);

  optimizer->StartOptimization();
  // try
  //{
  //  optimizer->StartOptimization();
  //}
  // catch (itk::ExceptionObject e)
  //{
  //  std::cerr << e << std::endl;
  //  return;
  //}

  typename EntropyCostType::ParametersType params = optimizer->GetCurrentPosition();
  std::cout << '\n' << optimizer->GetStopConditionDescription() << std::endl << std::endl;
  std::cout << "\nParams: " << params;
  std::cout << "\nCost: " << optimizer->GetCurrentMetricValue();
  std::cout << "\nIterations: " << optimizer->GetCurrentIteration();

  entropyCost->SetParameters(params); // in case the last evaluation was not done with the best parameters

  entropyCost->GetCorrectedImage(output);
}

} // end namespace itk

#endif // itkEntropyBasedIntensityCorrectionImageFilter_hxx
