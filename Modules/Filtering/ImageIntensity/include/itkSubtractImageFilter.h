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
#ifndef __itkSubtractImageFilter_h
#define __itkSubtractImageFilter_h

#include "itkBinaryFunctorImageFilter.h"

namespace itk
{
/** \class SubtractImageFilter
 * \brief Implements an operator for pixel-wise subtraction of two images.
 *
 * The main functionality of this class is to subtract each pixel from image2
 * from its corresponding pixel in image1:
 *
 * Output = Input1 - Input2.
 *
 * This is done using
 *
 * SetInput1( image1 );
 * SetInput2( image2 );
 *
 * This class is parametrized over the types of the two
 * input images and the type of the output image.
 * Numeric conversions (castings) are done by the C++ defaults.
 *
 * Additionally, a constant can be subtracted from every pixel in an image using:
 *
 * SetInput1( image1 );
 * SetConstant2( constant );
 *
 * Note: The result of AddImageFilter with a negative constant is not
 * necessarily the same as SubtractImageFilter. This would be the case when
 * the PixelType defines an operator-() that is not the inverse of operator+()
 *
 * \ingroup IntensityImageFilters Multithreaded
 * \ingroup ITK-ImageIntensity
 *
 * \wiki
 * \wikiexample{ImageProcessing/SubtractImageFilter,Subtract two images}
 * \wikiexample{ImageProcessing/SubtractConstantFromImageFilter,Subtract a constant from every pixel in an image}
 * \endwiki
 */
namespace Functor
{
template< class TInput1, class TInput2 = TInput1, class TOutput = TInput1 >
class Sub2
{
public:
  Sub2() {}
  ~Sub2() {}
  bool operator!=(const Sub2 &) const
  {
    return false;
  }

  bool operator==(const Sub2 & other) const
  {
    return !( *this != other );
  }

  inline TOutput operator()(const TInput1 & A, const TInput2 & B) const
  { return (TOutput)( A - B ); }
};
}

template< class TInputImage1, class TInputImage2 = TInputImage1, class TOutputImage = TInputImage1 >
class ITK_EXPORT SubtractImageFilter:
  public
  BinaryFunctorImageFilter< TInputImage1, TInputImage2, TOutputImage,
                            Functor::Sub2<
                              typename TInputImage1::PixelType,
                              typename TInputImage2::PixelType,
                              typename TOutputImage::PixelType >   >
{
public:
  /** Standard class typedefs. */
  typedef SubtractImageFilter Self;
  typedef BinaryFunctorImageFilter<
    TInputImage1, TInputImage2, TOutputImage,
    Functor::Sub2< typename TInputImage1::PixelType,
                   typename TInputImage2::PixelType,
                   typename TOutputImage::PixelType > >  Superclass;

  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(SubtractImageFilter,
               BinaryFunctorImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( Input1Input2OutputAdditiveOperatorsCheck,
                   ( Concept::AdditiveOperators< typename TInputImage1::PixelType,
                                                 typename TInputImage2::PixelType,
                                                 typename TOutputImage::PixelType > ) );
  /** End concept checking */
#endif
protected:
  SubtractImageFilter() {}
  virtual ~SubtractImageFilter() {}
private:
  SubtractImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);      //purposely not implemented
};
} // end namespace itk

#endif
