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
#ifndef __itkStatisticsAlgorithm_txx
#define __itkStatisticsAlgorithm_txx

#include "itkStatisticsAlgorithm.h"
#include "itkNumericTraits.h"

namespace itk
{
namespace Statistics
{
namespace Algorithm
{
template< class TSize >
inline TSize FloorLog(TSize size)
{
  TSize k;

  for ( k = 0; size != 1; size >>= 1 )
    {
    ++k;
    }

  return k;
}

/** The endIndex should points one point after the last elements if multiple
 * partitionValue exist in the sample the return index will points the middle
 * of such values. Implemented following the description of the partition
 * algorithm in the QuickSelect entry of the Wikipedia.
 * http://en.wikipedia.org/wiki/Selection_algorithm. */
template< class TSubsample >
inline int Partition(TSubsample *sample,
                     unsigned int activeDimension,
                     int beginIndex, int endIndex,
                     const typename TSubsample::MeasurementType partitionValue)
{
  typedef typename TSubsample::MeasurementType MeasurementType;

  int moveToFrontIndex = beginIndex;
  int moveToBackIndex = endIndex - 1;

  //
  // Move to the back all entries whose activeDimension component is equal to
  // the partitionValue.
  //
  while ( moveToFrontIndex < moveToBackIndex )
    {
    //
    //  Find the first element (from the back) that is in the wrong side of the
    // partition.
    //
    while ( moveToBackIndex >= beginIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToBackIndex)[activeDimension] != partitionValue )
        {
        break;
        }
      moveToBackIndex--;
      }

    //
    //  Find the first element (from the front) that is in the wrong side of the
    // partition.
    //
    while ( moveToFrontIndex < endIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToFrontIndex)[activeDimension] == partitionValue )
        {
        break;
        }
      moveToFrontIndex++;
      }

    if ( moveToFrontIndex < moveToBackIndex )
      {
      //
      // Swap them to place them in the correct side of the partition
      //
      sample->Swap(moveToBackIndex, moveToFrontIndex);
      }
    }

  //
  // Now, ignore the section at the end with all the values equal to the
  // partition value,
  // and start swapping entries that are in the wrong side of the partition.
  //
  moveToFrontIndex = beginIndex;
  moveToBackIndex  = endIndex - 1;
  while ( moveToFrontIndex < moveToBackIndex )
    {
    //
    //  Find the first element (from the back) that is in the wrong side of the
    // partition.
    //
    while ( moveToBackIndex >= beginIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToBackIndex)[activeDimension] < partitionValue )
        {
        break;
        }
      moveToBackIndex--;
      }

    //
    //  Find the first element (from the front) that is in the wrong side of the
    // partition.
    //
    while ( moveToFrontIndex < endIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToFrontIndex)[activeDimension] > partitionValue )
        {
        break;
        }
      moveToFrontIndex++;
      }

    if ( moveToFrontIndex < moveToBackIndex )
      {
      //
      // Swap them to place them in the correct side of the partition
      //
      sample->Swap(moveToBackIndex, moveToFrontIndex);
      }
    }

  //
  // Take all the entries with activeDimension component equal to
  // partitionValue, that were placed at the end of the list, and move them to
  // the interface between smaller and larger values.
  //
  int beginOfSectionEqualToPartition = moveToFrontIndex;
  moveToBackIndex = endIndex - 1;
  while ( moveToFrontIndex < moveToBackIndex )
    {
    //
    //  Find the first element (from the back) that is in the wrong side of the
    // partition.
    //
    while ( moveToBackIndex >= beginIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToBackIndex)[activeDimension] == partitionValue )
        {
        break;
        }
      moveToBackIndex--;
      }

    //
    //  Find the first element (from the front) that is in the wrong side of the
    // partition.
    //
    while ( moveToFrontIndex < endIndex )
      {
      if ( sample->GetMeasurementVectorByIndex(moveToFrontIndex)[activeDimension] != partitionValue )
        {
        break;
        }
      moveToFrontIndex++;
      }

    if ( moveToFrontIndex < moveToBackIndex )
      {
      //
      // Swap them to place them in the correct side of the partition
      //
      sample->Swap(moveToBackIndex, moveToFrontIndex);
      }
    }
  int endOfSectionEqualToPartition = moveToFrontIndex - 1;

  int storeIndex = ( beginOfSectionEqualToPartition + endOfSectionEqualToPartition ) / 2;

  const MeasurementType pivotValue  = sample->GetMeasurementVectorByIndex(storeIndex)[activeDimension];
  if ( pivotValue != partitionValue )
    {
    // The partition was done using a value that is not present in the sample.
    // Therefore we must now find the largest value of the left section and
    // swap it to the boundary between smaller and larger than the
    // partitionValue.
    for ( int kk = beginIndex; kk < storeIndex; kk++ )
      {
      MeasurementType nodeValue      = sample->GetMeasurementVectorByIndex(kk)[activeDimension];
      MeasurementType boundaryValue  = sample->GetMeasurementVectorByIndex(storeIndex)[activeDimension];
      if ( nodeValue > boundaryValue )
        {
        sample->Swap(kk, storeIndex);
        }
      }
    }

  return storeIndex;
}

template< class TValue >
inline TValue MedianOfThree(const TValue a,
                            const TValue b,
                            const TValue c)
{
  if ( a < b )
    {
    if ( b < c )
      {
      return b;
      }
    else if ( a < c )
      {
      return c;
      }
    else
      {
      return a;
      }
    }
  else if ( a < c )
    {
    return a;
    }
  else if ( b < c )
    {
    return c;
    }
  else
    {
    return b;
    }
}

template< class TSample >
inline void FindSampleBound(const TSample *sample,
                            typename TSample::ConstIterator begin,
                            typename TSample::ConstIterator end,
                            typename TSample::MeasurementVectorType & min,
                            typename TSample::MeasurementVectorType & max)
{
  typedef typename TSample::MeasurementVectorSizeType MeasurementVectorSizeType;

  const MeasurementVectorSizeType Dimension = sample->GetMeasurementVectorSize();
  if ( Dimension == 0 )
    {
    itkGenericExceptionMacro(
      << "Length of a sample's measurement vector hasn't been set.");
    }
  // Sanity check
  MeasurementVectorTraits::Assert(max, Dimension,
                                  "Length mismatch StatisticsAlgorithm::FindSampleBound");
  MeasurementVectorTraits::Assert(min, Dimension,
                                  "Length mismatch StatisticsAlgorithm::FindSampleBound");

  if ( sample->Size() == 0 )
    {
    itkGenericExceptionMacro(
      << "Attempting to compute bounds of a sample list containing no\
       measurement vectors");
    }

  unsigned int dimension;
  typename TSample::MeasurementVectorType temp;

  min = max = temp = begin.GetMeasurementVector();
  while ( true )
    {
    for ( dimension = 0; dimension < Dimension; dimension++ )
      {
      if ( temp[dimension] < min[dimension] )
        {
        min[dimension] = temp[dimension];
        }
      else if ( temp[dimension] > max[dimension] )
        {
        max[dimension] = temp[dimension];
        }
      }
    ++begin;
    if ( begin == end )
      {
      break;
      }
    temp = begin.GetMeasurementVector();
    }   // end of while
}

/** The endIndex should points one point after the last elements. */
template< class TSubsample >
inline void
FindSampleBoundAndMean(const TSubsample *sample,
                       int beginIndex,
                       int endIndex,
                       typename TSubsample::MeasurementVectorType & min,
                       typename TSubsample::MeasurementVectorType & max,
                       typename TSubsample::MeasurementVectorType & mean)
{
  typedef typename TSubsample::MeasurementType       MeasurementType;
  typedef typename TSubsample::MeasurementVectorType MeasurementVectorType;

  typedef typename TSubsample::MeasurementVectorSizeType MeasurementVectorSizeType;
  const MeasurementVectorSizeType Dimension = sample->GetMeasurementVectorSize();
  if ( Dimension == 0 )
    {
    itkGenericExceptionMacro(
      << "Length of a sample's measurement vector hasn't been set.");
    }

  Array< double > sum(Dimension);

  MeasurementVectorSizeType dimension;
  MeasurementVectorType     temp;
  NumericTraits<MeasurementVectorType>::SetLength(temp, Dimension);
  NumericTraits<MeasurementVectorType>::SetLength(mean, Dimension);

  min = max = temp = sample->GetMeasurementVectorByIndex(beginIndex);
  double frequencySum = sample->GetFrequencyByIndex(beginIndex);
  sum.Fill(0.0);

  while ( true )
    {
    for ( dimension = 0; dimension < Dimension; dimension++ )
      {
      if ( temp[dimension] < min[dimension] )
        {
        min[dimension] = temp[dimension];
        }
      else if ( temp[dimension] > max[dimension] )
        {
        max[dimension] = temp[dimension];
        }
      sum[dimension] += temp[dimension];
      }
    ++beginIndex;
    if ( beginIndex == endIndex )
      {
      break;
      }
    temp = sample->GetMeasurementVectorByIndex(beginIndex);
    frequencySum += sample->GetFrequencyByIndex(beginIndex);
    }   // end of while

  for ( unsigned int i = 0; i < Dimension; i++ )
    {
    mean[i] = (MeasurementType)( sum[i] / frequencySum );
    }
}

/** The endIndex should point one point after the last elements.  Note that kth
 * is an index in a different scale than [beginIndex,endIndex].  For example,
 * it is possible to feed this function with beginIndex=15, endIndex=23, and
 * kth=3, since we can ask for the element 3rd in the range [15,23]. */

template< class TSubsample >
inline typename TSubsample::MeasurementType
QuickSelect(TSubsample *sample,
            unsigned int activeDimension,
            int beginIndex,
            int endIndex,
            int kth,
            typename TSubsample::MeasurementType medianGuess)
{
  typedef typename TSubsample::MeasurementType MeasurementType;

  int begin = beginIndex;
  int end = endIndex - 1;
  int kthIndex = kth + beginIndex;

  MeasurementType tempMedian;

  //
  // Select a pivot value
  //
  if ( medianGuess != NumericTraits< MeasurementType >::NonpositiveMin() )
    {
    tempMedian = medianGuess;
    }
  else
    {
    const int             length = end - begin;
    const int             middle = begin + length / 2;
    const MeasurementType v1 = sample->GetMeasurementVectorByIndex(begin)[activeDimension];
    const MeasurementType v2 = sample->GetMeasurementVectorByIndex(end)[activeDimension];
    const MeasurementType v3 = sample->GetMeasurementVectorByIndex(middle)[activeDimension];
    tempMedian = MedianOfThree< MeasurementType >(v1, v2, v3);
    }

  while ( true )
    {
    // Partition expects the end argument to be one past-the-end of the array.
    // The index pivotNewIndex returned by Partition is in the range
    // [begin,end].
    int pivotNewIndex =
      Partition< TSubsample >(sample, activeDimension, begin, end + 1, tempMedian);

    if ( kthIndex == pivotNewIndex )
      {
      break;
      }

    if ( kthIndex < pivotNewIndex )
      {
      end = pivotNewIndex - 1;
      }
    else
      {
      begin = pivotNewIndex + 1;
      }

    if ( begin > end )
      {
      break;
      }

    const int             length = end - begin;
    const MeasurementType v1 = sample->GetMeasurementVectorByIndex(begin)[activeDimension];
    const MeasurementType v2 = sample->GetMeasurementVectorByIndex(end)[activeDimension];

    // current partition has only 1 or 2 elements
    if ( length < 2 )
      {
      if ( v2 < v1 )
        {
        sample->Swap(begin, end);
        }
      break;
      }

    const int             middle = begin + length / 2;
    const MeasurementType v3 = sample->GetMeasurementVectorByIndex(middle)[activeDimension];
    tempMedian = MedianOfThree< MeasurementType >(v1, v2, v3);
    }

  return sample->GetMeasurementVectorByIndex(kthIndex)[activeDimension];
}

template< class TSubsample >
inline typename TSubsample::MeasurementType
QuickSelect(TSubsample *sample,
            unsigned int activeDimension,
            int beginIndex,
            int endIndex,
            int kth)
{
  typedef typename TSubsample::MeasurementType MeasurementType;
  MeasurementType medianGuess = NumericTraits< MeasurementType >::NonpositiveMin();
  return QuickSelect< TSubsample >(sample, activeDimension,
                                   beginIndex, endIndex, kth, medianGuess);
}


template< class TSubsample >
inline int UnguardedPartition(TSubsample *sample,
                              unsigned int activeDimension,
                              int beginIndex,
                              int endIndex,
                              typename TSubsample::MeasurementType pivotValue)
{
  typedef typename TSubsample::MeasurementType MeasurementType;
  while ( true )
    {
    MeasurementType beginValue =
      sample->GetMeasurementVectorByIndex(beginIndex)[activeDimension];

    while ( beginValue < pivotValue )
      {
      ++beginIndex;

      beginValue = sample->GetMeasurementVectorByIndex(beginIndex)[activeDimension];
      }

    --endIndex;

    MeasurementType endValue =
      sample->GetMeasurementVectorByIndex(endIndex)[activeDimension];

    while ( pivotValue < endValue )
      {
      --endIndex;
      endValue = sample->GetMeasurementVectorByIndex(endIndex)[activeDimension];
      }

    if ( !( beginIndex < endIndex ) )
      {
      return beginIndex;
      }

    sample->Swap(beginIndex, endIndex);

    ++beginIndex;
    }
}
template< class TSubsample >
inline typename TSubsample::MeasurementType
NthElement(TSubsample *sample,
           unsigned int activeDimension,
           int beginIndex,
           int endIndex,
           int nth)
{
  typedef typename TSubsample::MeasurementType MeasurementType;

  const int nthIndex = beginIndex + nth;

  int beginElement = beginIndex;
  int endElement   = endIndex;

  while ( endElement - beginElement > 3 )
    {
    const int begin = beginElement;
    const int end   = endElement - 1;
    const int length = endElement - beginElement;
    const int middle = beginElement + length / 2;

    const MeasurementType v1 = sample->GetMeasurementVectorByIndex(begin)[activeDimension];
    const MeasurementType v2 = sample->GetMeasurementVectorByIndex(end)[activeDimension];
    const MeasurementType v3 = sample->GetMeasurementVectorByIndex(middle)[activeDimension];

    const MeasurementType tempMedian = MedianOfThree< MeasurementType >(v1, v2, v3);

    int cut = UnguardedPartition(sample, activeDimension, beginElement,  endElement, tempMedian);

    if ( cut <= nthIndex )
      {
      beginElement = cut;
      }
    else
      {
      endElement = cut;
      }
    }

  InsertSort(sample, activeDimension, beginElement,  endElement);

  return sample->GetMeasurementVectorByIndex(nthIndex)[activeDimension];
}

template< class TSubsample >
inline void InsertSort(TSubsample *sample,
                       unsigned int activeDimension,
                       int beginIndex,
                       int endIndex)
{
  int backwardSearchBegin;
  int backwardIndex;

  for ( backwardSearchBegin = beginIndex + 1;
        backwardSearchBegin < endIndex;
        backwardSearchBegin++ )
    {
    backwardIndex = backwardSearchBegin;
    while ( backwardIndex > beginIndex )
      {
      typedef typename TSubsample::MeasurementType MeasurementType;
      const MeasurementType value1 = sample->GetMeasurementVectorByIndex(backwardIndex)[activeDimension];
      const MeasurementType value2 = sample->GetMeasurementVectorByIndex(backwardIndex - 1)[activeDimension];

      if ( value1 < value2 )
        {
        sample->Swap(backwardIndex, backwardIndex - 1);
        }
      else
        {
        break;
        }
      --backwardIndex;
      }
    }
}

template< class TSubsample >
inline void DownHeap(TSubsample *sample,
                     unsigned int activeDimension,
                     int beginIndex, int endIndex, int node)
{
  int currentNode = node;
  int leftChild;
  int rightChild;
  int largerChild;

  typedef typename TSubsample::MeasurementType MeasurementType;
  MeasurementType currentNodeValue =
    sample->GetMeasurementVectorByIndex(currentNode)[activeDimension];
  MeasurementType leftChildValue;
  MeasurementType rightChildValue;
  MeasurementType largerChildValue;

  while ( true )
    {
    // location of first child
    largerChild = leftChild =
                    beginIndex + 2 * ( currentNode - beginIndex ) + 1;
    rightChild = leftChild + 1;
    if ( leftChild > endIndex - 1 )
      {
      // leaf node
      return;
      }

    largerChildValue = rightChildValue = leftChildValue =
                                           sample->GetMeasurementVectorByIndex(leftChild)[activeDimension];

    if ( rightChild < endIndex )
      {
      rightChildValue =
        sample->GetMeasurementVectorByIndex(rightChild)[activeDimension];
      }

    if ( leftChildValue < rightChildValue )
      {
      largerChild = rightChild;
      largerChildValue = rightChildValue;
      }

    if ( largerChildValue <= currentNodeValue )
      {
      // the node satisfies heap property
      return;
      }
    // move down current node value to the larger child
    sample->Swap(currentNode, largerChild);
    currentNode = largerChild;
    }
}

template< class TSubsample >
inline void HeapSort(TSubsample *sample,
                     unsigned int activeDimension,
                     int beginIndex,
                     int endIndex)
{
  // construct a heap
  int node;

  for ( node = beginIndex + ( endIndex - beginIndex ) / 2 - 1;
        node >= beginIndex; node-- )
    {
    DownHeap< TSubsample >(sample, activeDimension,
                           beginIndex, endIndex, node);
    }

  // sort
  int newEndIndex;
  for ( newEndIndex = endIndex - 1; newEndIndex >= beginIndex;
        --newEndIndex )
    {
    sample->Swap(beginIndex, newEndIndex);
    DownHeap< TSubsample >(sample, activeDimension,
                           beginIndex, newEndIndex, beginIndex);
    }
}

template< class TSubsample >
inline void IntrospectiveSortLoop(TSubsample *sample,
                                  unsigned int activeDimension,
                                  int beginIndex,
                                  int endIndex,
                                  int depthLimit,
                                  int sizeThreshold)
{
  typedef typename TSubsample::MeasurementType MeasurementType;

  int length = endIndex - beginIndex;
  int cut;
  while ( length > sizeThreshold )
    {
    if ( depthLimit == 0 )
      {
      HeapSort< TSubsample >(sample, activeDimension,
                             beginIndex, endIndex);
      return;
      }

    --depthLimit;
    cut = Partition< TSubsample >( sample, activeDimension,
                                   beginIndex, endIndex,
                                   MedianOfThree< MeasurementType >
                                     (sample->GetMeasurementVectorByIndex(beginIndex)[activeDimension],
                                     sample->GetMeasurementVectorByIndex(beginIndex + length / 2)[activeDimension],
                                     sample->GetMeasurementVectorByIndex(endIndex - 1)[activeDimension]) );
    IntrospectiveSortLoop< TSubsample >(sample, activeDimension,
                                        cut, endIndex,
                                        depthLimit, sizeThreshold);
    endIndex = cut;
    length = endIndex - beginIndex;
    }
}

template< class TSubsample >
inline void IntrospectiveSort(TSubsample *sample,
                              unsigned int activeDimension,
                              int beginIndex,
                              int endIndex,
                              int sizeThreshold)
{
  typedef typename TSubsample::MeasurementType MeasurementType;
  IntrospectiveSortLoop< TSubsample >(sample, activeDimension, beginIndex, endIndex,
                                      2 * FloorLog(endIndex - beginIndex), sizeThreshold);
  InsertSort< TSubsample >(sample, activeDimension, beginIndex, endIndex);
}
} // end of namespace Algorithm
} // end of namespace Statistics
} // end of namespace itk

#endif // #ifndef __itkStatisticsAlgorithm_txx
