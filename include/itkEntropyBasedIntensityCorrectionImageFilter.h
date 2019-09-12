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
#ifndef itkEntropyBasedIntensityCorrectionImageFilter_h
#define itkEntropyBasedIntensityCorrectionImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkBSplineScatteredDataPointSetToImageFilter.h"
#include "itkPointSet.h"
#include "itkVector.h"
#include "itkScalarImageToHistogramGenerator.h"

namespace itk
{

/** \class EntropyBasedIntensityCorrectionImageFilter
 *
 * \brief Create a b-spline intensity correction function by minimizing
 * entropy.
 *
 * todo
 *
 * \ingroup EntropyBasedIntensityCorrection
 *
 */
template <typename TInputImage, typename TMaskImage, typename TOutputImage=TInputImage>
class ITK_TEMPLATE_EXPORT EntropyBasedIntensityCorrectionImageFilter
  : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(EntropyBasedIntensityCorrectionImageFilter);

  static constexpr unsigned int InputImageDimension = TInputImage::ImageDimension;
  static constexpr unsigned int OutputImageDimension = TOutputImage::ImageDimension;

  using InputImageType = TInputImage;
  using MaskImageType = TMaskImage;
  using OutputImageType = TOutputImage;
  using InputPixelType = typename InputImageType::PixelType;
  using OutputPixelType = typename OutputImageType::PixelType;

  /** Standard class type alias. */
  using Self = EntropyBasedIntensityCorrectionImageFilter<InputImageType, MaskImageType, OutputImageType>;
  using Superclass = ImageToImageFilter<InputImageType, OutputImageType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;

  /** Run-time type information. */
  itkTypeMacro(EntropyBasedIntensityCorrectionImageFilter, ImageToImageFilter);

  /** Standard New macro. */
  itkNewMacro(Self);

  /** Number of histogram bins for entropy calculation. */
  itkSetMacro(NumberOfBins, unsigned int);
  itkGetConstMacro(NumberOfBins, unsigned int);

protected:
  /** B-sline filter type alias */
  using RealType = float;
  using ScalarType = Vector<RealType, 1>;
  using PointSetType = PointSet<ScalarType, ImageDimension>;
  using ScalarImageType = Image<ScalarType, ImageDimension>;
  using BSplineFilterType = BSplineScatteredDataPointSetToImageFilter<PointSetType, ScalarImageType>;
  using BiasFieldControlPointLatticeType = typename BSplineFilterType::PointDataImageType;
  using ArrayType = typename BSplineFilterType::ArrayType;

  using ImageToHistogramFilterType = Statistics::ScalarImageToHistogramGenerator<InputImageType>;

  EntropyBasedIntensityCorrectionImageFilter();
  virtual ~EntropyBasedIntensityCorrectionImageFilter() override {}

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  using OutputRegionType = typename OutputImageType::RegionType;

  /** Ensures that this filter can compute the entire output at once.  */
  void
  EnlargeOutputRequestedRegion(DataObject *) override;

  void
  GenerateData() override;

  typename BSplineFilterType::Pointer          m_BSplineFilter;
  typename ImageToHistogramFilterType::Pointer m_ImageToHistogramFilter;

  unsigned int m_NumberOfBins{ 32 };

private:
#ifdef ITK_USE_CONCEPT_CHECKING
  // Add concept checking such as
  // itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename InputImageType::PixelType > ) );
#endif
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkEntropyBasedIntensityCorrectionImageFilter.hxx"
#endif

#endif // itkEntropyBasedIntensityCorrectionImageFilter
