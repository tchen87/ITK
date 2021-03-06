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
#ifndef __itkColorTable_h
#define __itkColorTable_h

#include "itkObject.h"
#include "itkRGBPixel.h"
#include "itkObjectFactory.h"

#include <string>
#include <vector>
namespace itk
{
/** \class ColorTable
 *  \brief Define a Color table for image visualisation
 *
 * Generates color lookup tables of various types. The lookup table
 * value range differs for integral and continuous data types. Consult
 * the documentation for each lookup table generator.
 * \ingroup DataRepresentation
 * \ingroup ITK-Common
 */

template< class TPixel >
class ITK_EXPORT ColorTable:public Object
{
public:
  /** Standard class typedefs. */
  typedef ColorTable                 Self;
  typedef Object                     Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ColorTable, Object);

  /** Generate a lookup table of 8 discrete colors. The colors are Red,
    * Purple, Aqua, Yellow, Green, Blue, Grey0.70, White. For integral
    * pixel types, the color range is bewteen NonpositiveMin() and max(). For
    * continuous types, the range is 0.0 to 1.0.
    */
  void    UseDiscreteColors(void);

  /** Generate a lookuptable of n grayscale values. For integral pixel
    * types, a ramp is generated from NonpositiveMin() to max() of the
    * pixel type. For continuous pixel types, the range is 0.0 to 1.0.
    */
  void    UseGrayColors(unsigned int n = 256);

  /** Generate a lookup table of n values good for showing
    * "temperatures".  For integral pixel types, the color range is
    * bewteen NonpositiveMin() and max(). For continuous types, the
    * range is 0.0 to 1.0.
    */
  void    UseHeatColors(unsigned int n = 256);

  /** Generate a lookup table of n random values. For integral pixel
    * types, the color range is between NonpositiveMin() and
    * max(). For continous types, the range is 0.0 to 1.0.
    */
  void    UseRandomColors(unsigned int n = 256);

  /** Get the number of colors in the lookup table. */
  itkGetConstMacro(NumberOfColors, unsigned int);

  /** Get the color stored at a given index. */
  RGBPixel< TPixel > GetColor(unsigned int colorId);

  /** Set the color at a given index. Optionally provide a name for
    * the color. If a name is not provided, the name "UserDefined" is
    * used.
    */
  bool    SetColor(unsigned int c, TPixel r, TPixel g, TPixel b,
                   const char *name = "UserDefined");
  bool    SetColor(unsigned int c, RGBPixel<TPixel> pixel,
                   const char *name = "UserDefined");

  /** Given the position in the table and the color
    * returns the value.
    */
  TPixel  GetColorComponent(unsigned int colorId, char rgb);

  /** Get the name of the color at a given index. */
  std::string  GetColorName(unsigned int colorId);

  /** Find the color closest to a given pixel. Uses a L2 distance
    * metric.
     */
  unsigned int GetClosestColorTableId(TPixel r, TPixel g, TPixel b);

protected:
  ColorTable();
  void PrintSelf(std::ostream & os, Indent indent) const;

  unsigned int m_NumberOfColors;

  std::vector<std::string>         m_ColorName;
  std::vector<RGBPixel< TPixel > > m_Color;
private:
  ColorTable(const Self &);     //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  void DeleteColors();
};
} // namespace itk

// Define instantiation macro for this template.
#define ITK_TEMPLATE_ColorTable(_, EXPORT, TypeX, TypeY)     \
  namespace itk                                              \
  {                                                          \
  _( 1 ( class EXPORT ColorTable< ITK_TEMPLATE_1 TypeX > ) ) \
  namespace Templates                                        \
  {                                                          \
  typedef ColorTable< ITK_TEMPLATE_1 TypeX >                 \
  ColorTable##TypeY;                                       \
  }                                                          \
  }

#if ITK_TEMPLATE_EXPLICIT
#include "Templates/itkColorTable+-.h"
#endif

#if ITK_TEMPLATE_TXX
#include "itkColorTable.txx"
#endif

#endif
