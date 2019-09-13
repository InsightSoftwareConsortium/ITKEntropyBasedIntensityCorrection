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

#ifndef itkImageEntropyCostFunction_hxx
#define itkImageEntropyCostFunction_hxx

#include "itkImageEntropyCostFunction.h"

#include "itkBSplineTransformInitializer.h"

namespace itk
{
template <typename TInputImage, typename TMaskImage, typename TOutputImage, unsigned VSplineOrder>
void
ImageEntropyCostFunction<TInputImage, TMaskImage, TOutputImage, VSplineOrder>::Initialize()
{
  if (!m_Image)
  {
    itkExceptionMacro(<< "Image is not present");
  }
  // If the image is provided by a source, update the source.
  if (m_Image->GetSource())
  {
    m_Image->GetSource()->Update();
  }

  using InRegionType = typename InputImageType::RegionType;
  InRegionType largestR = m_Image->GetLargestPossibleRegion();
  InRegionType reqR = m_Image->GetRequestedRegion();

  if (largestR.GetNumberOfPixels() == 0)
  {
    itkExceptionMacro(<< "Image's LargestPossibleRegion is empty");
  }
  if (reqR.GetNumberOfPixels() == 0)
  {
    itkExceptionMacro(<< "Image's RequestedRegion is empty");
  }
  if (!m_Image->GetBufferedRegion().IsInside(reqR))
  {
    itkExceptionMacro(<< "Image's RequestedRegion is not (completely) contained in its own BufferedRegion");
  }
  if (m_Mask && !m_Mask->GetBufferedRegion().IsInside(reqR))
  {
    itkExceptionMacro(<< "Image's RequestedRegion is not (completely) contained in mask's BufferedRegion");
  }

  using CoefficientImageType = typename TransformType::ImageType;
  typename CoefficientImageType::Pointer zeroImage = CoefficientImageType::New();

  using CRegionType = typename CoefficientImageType::RegionType;
  CRegionType reg;
  // reg = zeroImage->GetBufferedRegion();
  for (unsigned d = 0; d < Dimension + 1; d++)
  {
    reg.SetSize(d, 1 + VSplineOrder);
  }


  using InitializerType = BSplineTransformInitializer<TransformType, CoefficientImageType>;
  typename InitializerType::Pointer    transformInitializer = InitializerType::New();
  typename TransformType::MeshSizeType meshSize;
  meshSize.Fill(1);
  transformInitializer->SetTransformDomainMeshSize(meshSize);
  transformInitializer->SetImage(zeroImage);

  // now initialize both transforms
  transformInitializer->SetTransform(m_AddTransform);
  transformInitializer->InitializeTransform();
  transformInitializer->SetTransform(m_MulTransform);
  transformInitializer->InitializeTransform();

  for (unsigned d = 0; d < Dimension; d++)
  {
    m_AdditiveCoefficients[d] = zeroImage;
    m_MultiplicativeCoefficients[d] = zeroImage;
  }

  // m_AdditiveCoefficients[Dimension] = firstPartOfparameters;
  // m_MultiplicativeCoefficients[Dimension] = secondPartOfparameters;

  m_Parameters = ParametersType(GetNumberOfParameters());
  m_Parameters.Fill(0.0);

  // If there are any observers on the metric, call them to give the
  // user code a chance to set m_Parameters on the metric
  this->InvokeEvent(InitializeEvent());
  m_CurrentIteration = 0;
}


/**
 * PrintSelf
 */
template <typename TInputImage, typename TMaskImage, typename TOutputImage, unsigned VSplineOrder>
void
ImageEntropyCostFunction<TInputImage, TMaskImage, TOutputImage, VSplineOrder>::PrintSelf(std::ostream & os,
                                                                                         Indent         indent) const
{
  Superclass::PrintSelf(os, indent);
  // m_NormalizeIntensities etc
}

template <typename TInputImage, typename TMaskImage, typename TOutputImage, unsigned VSplineOrder>
void
ImageEntropyCostFunction<TInputImage, TMaskImage, TOutputImage, VSplineOrder>::GetCorrectedImage(
  OutputImageType * output) const
{
  itkAssertOrThrowMacro(m_Image->GetBufferedRegion() == output->GetBufferedRegion(),
                        "Input and output buffered regions do not match!");
  itk::ImageRegionConstIterator<InputImageType> in(m_Image, m_Image->GetBufferedRegion());
  itk::ImageRegionIterator<OutputImageType>     out(output, output->GetBufferedRegion());

  for (in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd(); ++in, ++out)
  {
    out.Set(in.Get());
  }
}

template <typename TInputImage, typename TMaskImage, typename TOutputImage, unsigned VSplineOrder>
typename ImageEntropyCostFunction<TInputImage, TMaskImage, TOutputImage, VSplineOrder>::MeasureType
ImageEntropyCostFunction<TInputImage, TMaskImage, TOutputImage, VSplineOrder>::GetValue() const
{
  // m_ImageToHistogramFilter->SetInput(m_Image);
  // m_ImageToHistogramFilter->SetNumberOfBins(this->m_NumberOfBins);
  // m_ImageToHistogramFilter->SetAutoHistogramMinimumMaximum(true);
  // m_ImageToHistogramFilter->SetMarginalScale(10.0);
  // m_ImageToHistogramFilter->Compute();
  // m_ImageToHistogramFilter->Print(std::cout);
  // m_ImageToHistogramFilter->GetOutput()->Print(std::cout);

  // using HistogramType = typename ImageToHistogramFilterType::HistogramType;
  // typename HistogramType::ConstPointer histogram = m_ImageToHistogramFilter->GetOutput();
  // for (SizeValueType ii = 0; ii < histogram->Size(); ++ii)
  //{
  //  std::cout << ii << " " << histogram->GetFrequency(ii, 0) << std::endl;
  //}

  // const double                          totalFrequency = histogram->GetTotalFrequency();
  // CompensatedSummation<double>          entropy;
  // typename HistogramType::ConstIterator itr = histogram->Begin();
  // typename HistogramType::ConstIterator end = histogram->End();
  // const double                          log2 = std::log(2.0);
  // while (itr != end)
  //{
  //  const double probability = itr.GetFrequency() / totalFrequency;

  //  if (probability > 0.99 / totalFrequency)
  //  {
  //    entropy += -probability * std::log(probability) / log2;
  //  }
  //  ++itr;
  //}
  // std::cout << "Entropy: " << entropy.GetSum() << std::endl;

  m_CurrentIteration++;
  return std::sqrt(m_CurrentIteration);
}

} // end namespace itk

#endif // itkImageEntropyCostFunction_hxx
