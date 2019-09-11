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

#include "itkCompensatedSummation.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

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
  using InputRegionType = typename InputImageType::RegionType;

  input->Update();
  m_ImageToHistogramFilter->SetInput(input);
  m_ImageToHistogramFilter->SetNumberOfBins(this->m_NumberOfBins);
  m_ImageToHistogramFilter->SetAutoHistogramMinimumMaximum(true);
  m_ImageToHistogramFilter->SetMarginalScale(10.0);
  m_ImageToHistogramFilter->Compute();
  m_ImageToHistogramFilter->Print(std::cout);
  m_ImageToHistogramFilter->GetOutput()->Print(std::cout);

  using HistogramType = typename ImageToHistogramFilterType::HistogramType;
  typename HistogramType::ConstPointer histogram = m_ImageToHistogramFilter->GetOutput();
  for (SizeValueType ii = 0; ii < histogram->Size(); ++ii)
  {
    std::cout << ii << " " << histogram->GetFrequency(ii, 0) << std::endl;
  }

  const double                          totalFrequency = histogram->GetTotalFrequency();
  CompensatedSummation<double>          entropy;
  typename HistogramType::ConstIterator itr = histogram->Begin();
  typename HistogramType::ConstIterator end = histogram->End();
  const double                          log2 = std::log(2.0);
  while (itr != end)
  {
    const double probability = itr.GetFrequency() / totalFrequency;

    if (probability > 0.99 / totalFrequency)
    {
      entropy += -probability * std::log(probability) / log2;
    }
    ++itr;
  }
  std::cout << "Entropy: " << entropy.GetSum() << std::endl;


  // InputRegionType inputRegion = InputRegionType(outputRegion.GetSize());

  // itk::ImageRegionConstIterator<InputImageType> in(input, inputRegion);
  // itk::ImageRegionIterator<OutputImageType> out(output, outputRegion);

  // for (in.GoToBegin(), out.GoToBegin(); !in.IsAtEnd() && !out.IsAtEnd(); ++in, ++out)
  //{
  // out.Set( in.Get() );
  //}
}

} // end namespace itk

#endif // itkEntropyBasedIntensityCorrectionImageFilter_hxx
