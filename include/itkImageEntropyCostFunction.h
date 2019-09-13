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

#ifndef itkImageEntropyCostFunction_h
#define itkImageEntropyCostFunction_h

#include "itkImage.h"
#include "itkObjectToObjectMetricBase.h"
#include "itkBSplineTransform.h"

#include "itkTransformFileWriter.h" // debug
#include "itkTransformFactory.h"    // debug

namespace itk
{

/** \class ImageEntropyCostFunction
 *
 * \brief Calculate total entropy if the image transformed by BSpline coefficients.
 *
 * TODO: write more detailed description
 *
 * \ingroup EntropyBasedIntensityCorrection
 */

template <typename TInputImage, typename TMaskImage, typename TOutputImage, unsigned VSplineOrder = 3>
class ITK_TEMPLATE_EXPORT ImageEntropyCostFunction
  : public ObjectToObjectMetricBaseTemplate<typename NumericTraits<typename TInputImage::PixelType>::RealType>
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(ImageEntropyCostFunction);

  using InputImageType = TInputImage;
  using MaskImageType = TMaskImage;
  using OutputImageType = TOutputImage;
  using InputPixelType = typename InputImageType::PixelType;
  using OutputPixelType = typename OutputImageType::PixelType;

  using InputRealType = typename NumericTraits<InputPixelType>::RealType;

  /** Standard class type alias. */
  using Self = ImageEntropyCostFunction;
  using Superclass = ObjectToObjectMetricBaseTemplate<InputRealType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageEntropyCostFunction, SingleValuedCostFunction);
  itkNewMacro(Self);

  using MeasureType = typename Superclass::MeasureType;
  using DerivativeType = typename Superclass::DerivativeType;
  using ParametersType = typename Superclass::ParametersType;
  using ParametersValueType = typename Superclass::ParametersValueType;
  using NumberOfParametersType = typename Superclass::NumberOfParametersType;
  using CoordinateRepresentationType = typename Superclass::ParametersValueType;

  /** Set the original image. */
  virtual void
  SetImage(const InputImageType * image)
  {
    if (this->m_Image != image)
    {
      this->m_Image = image;
      this->Initialize();
      this->Modified();
    }
  }

  /** Get the original image. */
  itkGetConstObjectMacro(Image, InputImageType);

  /** Set the mask image. */
  virtual void
  SetMask(const InputImageType * mask)
  {
    if (this->m_Mask != mask)
    {
      if (!m_Image)
      {
        itkExceptionMacro(<< "Input image needs to be set first");
      }
      if (mask->GetBufferedRegion() != m_Image->GetBufferedRegion())
      {
        itkExceptionMacro(<< "Mask image's buffered region must match input image");
      }
      this->m_Mask = mask;
      this->Modified();
    }
  }

  /** Get the mask image. */
  itkGetConstObjectMacro(Mask, MaskImageType);

  /** Get the corrected image, using current parameters. */
  void
  GetCorrectedImage(OutputImageType * output) const;

  /** Initialize the Metric by making sure that all the components
   *  are present and plugged together correctly. */
  void
  Initialize() override;


  void
  SetParameters(ParametersType & params) override
  {
    const NumberOfParametersType nParam = m_AddTransform->GetNumberOfParametersPerDimension();
    const NumberOfParametersType nTotal = m_AddTransform->GetNumberOfParameters();
    const NumberOfParametersType nStart = nTotal - nParam;
    // for (unsigned i = 0; i < nParam; i++)
    //{
    //  m_TransformParameters[nStart + i] = m_Parameters[i];
    //}
    // m_AddTransform->SetParameters(m_TransformParameters);
    this->Modified();
  }

  const ParametersType &
  GetParameters() const override
  {
    const NumberOfParametersType nParam = m_AddTransform->GetNumberOfParametersPerDimension();
    const NumberOfParametersType nTotal = m_AddTransform->GetNumberOfParameters();
    const NumberOfParametersType nStart = nTotal - nParam;
    // for (unsigned i = 0; i < nParam; i++)
    //{
    //  m_Parameters[i] = m_TransformParameters[nStart + i];
    //}
    return m_Parameters;
  }

  MeasureType
  GetValue() const override;

  NumberOfParametersType
  GetNumberOfParameters() const override
  {
    return m_AddTransform->GetNumberOfParametersPerDimension();
  }

  NumberOfParametersType
  GetNumberOfLocalParameters() const override
  {
    return GetNumberOfParameters();
  }

  /** This method returns the derivative of the cost function corresponding
   * to the specified parameters. This cost function does not compute derivatives
   * and must be used by an optimizer that does not require them.  */
  void
  GetDerivative(DerivativeType &) const override
  {
    itkExceptionMacro(<< "Derivatives not computed (1).");
  }
  void
  GetValueAndDerivative(MeasureType & value, DerivativeType & derivative) const override
  {
    itkExceptionMacro(<< "Derivatives not computed (2).");
  }
  void
  UpdateTransformParameters(
    const DerivativeType &       derivative,
    CoordinateRepresentationType factor = NumericTraits<CoordinateRepresentationType>::OneValue()) override
  {
    itkExceptionMacro(<< "Derivatives not computed (3).");
  }

  bool
  HasLocalSupport() const override
  {
    return false;
  }

  itkSetMacro(NormalizeIntensities, bool);
  itkGetMacro(NormalizeIntensities, bool);
  itkBooleanMacro(NormalizeIntensities);

protected:
  /** Constants for the image dimensions */
  static constexpr unsigned int Dimension = TInputImage::ImageDimension;
  static_assert(Dimension == TOutputImage::ImageDimension, "Input and output images must have the same dimension");


  ImageEntropyCostFunction()
  {
    m_AddTransform = TransformType::New();
    m_MulTransform = TransformType::New();
    m_Parameters = ParametersType(this->GetNumberOfParameters());
    // m_Parameters.Fill(0.0);

#ifndef NDEBUG
    TransformFactory<TransformType>::RegisterTransform();
    typename TransformFileWriterTemplate<InputRealType>::Pointer writer =
      TransformFileWriterTemplate<InputRealType>::New();
    writer->SetInput(m_AddTransform);
    writer->SetFileName("aImageEntropyCostFunction.tfm");
    writer->Update();
#endif // !NDEBUG
  }

  ~ImageEntropyCostFunction() override
  {
    m_Image = nullptr;
    m_Mask = nullptr;
    m_AddTransform = nullptr;
    m_MulTransform = nullptr;
  }

  void
  PrintSelf(std::ostream & os, Indent indent) const;

  using TransformType = BSplineTransform<InputRealType, Dimension + 1, VSplineOrder>;

private:
  typename InputImageType::ConstPointer         m_Image;
  typename MaskImageType::ConstPointer          m_Mask;
  typename TransformType::Pointer               m_AddTransform;
  typename TransformType::Pointer               m_MulTransform;
  typename TransformType::CoefficientImageArray m_AdditiveCoefficients;
  typename TransformType::CoefficientImageArray m_MultiplicativeCoefficients;

  bool                   m_NormalizeIntensities = true;
  mutable unsigned long  m_CurrentIteration = 0;
  mutable ParametersType m_Parameters;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkImageEntropyCostFunction.hxx"
#endif

#endif // itkImageEntropyCostFunction_h
