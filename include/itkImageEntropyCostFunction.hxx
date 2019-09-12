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

  if (m_Image->GetBufferedRegion().GetNumberOfPixels() == 0)
  {
    itkExceptionMacro(<< "Image's BufferedRegion is empty");
  }

  // If the image is provided by a source, update the source.
  if (m_Image->GetSource())
  {
    m_Image->GetSource()->Update();
  }

  if (m_Mask && m_Mask->GetBufferedRegion() != m_Image->GetBufferedRegion())
  {
    itkExceptionMacro(<< "Mask image's buffered region must match input image");
  }

  using CoefficientImageType = typename TransformType::ImageType;
  typename CoefficientImageType::Pointer zeroImage = CoefficientImageType::New();
  //zeroImage->SetDirection(m_Image->GetDirection());

  //using InitializerType = itk::BSplineTransformInitializer<BSplineTransformType, FixedImageType>;
  //typename InitializerType::Pointer transformInitializer = InitializerType::New();

  //transformInitializer->SetTransform(bsplineTx);
  //transformInitializer->SetImage(initializationImage);
  //transformInitializer->SetTransformDomainMeshSize(meshSize);
  //transformInitializer->InitializeTransform();

  for (unsigned d = 0; d < InputImageDimension; d++)
  {
    m_AdditiveCoefficients[d] = zeroImage;
  }

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
  itkDebugMacro("GetValue( " << m_Parameters << " ) ");

  m_CurrentIteration++;
  return std::sqrt(m_CurrentIteration);
}

} // end namespace itk

#endif // itkImageEntropyCostFunction_hxx
