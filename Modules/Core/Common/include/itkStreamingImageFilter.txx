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
#ifndef __itkStreamingImageFilter_txx
#define __itkStreamingImageFilter_txx
#include "itkStreamingImageFilter.h"
#include "itkCommand.h"
#include "itkImageRegionIterator.h"
#include "itkImageAlgorithm.h"

namespace itk
{
/**
 *
 */
template< class TInputImage, class TOutputImage >
StreamingImageFilter< TInputImage, TOutputImage >
::StreamingImageFilter()
{
  // default to 10 divisions
  m_NumberOfStreamDivisions = 10;

  // create default region splitter
  m_RegionSplitter = ImageRegionSplitter< InputImageDimension >::New();
}

/**
 *
 */
template< class TInputImage, class TOutputImage >
StreamingImageFilter< TInputImage, TOutputImage >
::~StreamingImageFilter()
{}

/**
 *
 */
template< class TInputImage, class TOutputImage >
void
StreamingImageFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Number of stream divisions: " << m_NumberOfStreamDivisions
     << std::endl;
  if ( m_RegionSplitter )
    {
    os << indent << "Region splitter:" << m_RegionSplitter << std::endl;
    }
  else
    {
    os << indent << "Region splitter: (none)" << std::endl;
    }
}

/**
 *
 */
template< class TInputImage, class TOutputImage >
void
StreamingImageFilter< TInputImage, TOutputImage >
::PropagateRequestedRegion(DataObject *output)
{
  /**
   * check flag to avoid executing forever if there is a loop
   */
  if ( this->m_Updating )
    {
    return;
    }

  /**
   * Give the subclass a chance to indicate that it will provide
   * more data then required for the output. This can happen, for
   * example, when a source can only produce the whole output.
   * Although this is being called for a specific output, the source
   * may need to enlarge all outputs.
   */
  this->EnlargeOutputRequestedRegion(output);

  /**
   * Give the subclass a chance to define how to set the requested
   * regions for each of its outputs, given this output's requested
   * region.  The default implementation is to make all the output
   * requested regions the same.  A subclass may need to override this
   * method if each output is a different resolution.
   */
  this->GenerateOutputRequestedRegion(output);

  // we don't call GenerateInputRequestedRegion since the requested
  // regions are manage when the pipeline is execute

  // we don't call inputs PropagateRequestedRegion either
  // because the pipeline managed later
}

/**
 *
 */
template< class TInputImage, class TOutputImage >
void
StreamingImageFilter< TInputImage, TOutputImage >
::UpdateOutputData( DataObject *itkNotUsed(output) )
{
  unsigned int idx;

  /**
   * prevent chasing our tail
   */
  if ( this->m_Updating )
    {
    return;
    }

  /**
   * Prepare all the outputs. This may deallocate previous bulk data.
   */
  this->PrepareOutputs();

  /**
   * Make sure we have the necessary inputs
   */
  unsigned int ninputs = this->GetNumberOfValidRequiredInputs();
  if ( ninputs < this->GetNumberOfRequiredInputs() )
    {
    itkExceptionMacro(
      << "At least " << static_cast< unsigned int >( this->GetNumberOfRequiredInputs() )
      << " inputs are required but only " << ninputs << " are specified.");
    return;
    }
  this->SetAbortGenerateData(0);
  this->SetProgress(0.0);
  this->m_Updating = true;

  /**
   * Tell all Observers that the filter is starting
   */
  this->InvokeEvent( StartEvent() );

  /**
   * Allocate the output buffer.
   */
  OutputImageType      *outputPtr = this->GetOutput(0);
  OutputImageRegionType outputRegion = outputPtr->GetRequestedRegion();
  outputPtr->SetBufferedRegion(outputRegion);
  outputPtr->Allocate();

  /**
   * Grab the input
   */
  InputImageType * inputPtr =
    const_cast< InputImageType * >( this->GetInput(0) );

  /**
   * Determine of number of pieces to divide the input.  This will be the
   * minimum of what the user specified via SetNumberOfStreamDivisions()
   * and what the Splitter thinks is a reasonable value.
   */
  unsigned int numDivisions, numDivisionsFromSplitter;

  numDivisions = m_NumberOfStreamDivisions;
  numDivisionsFromSplitter =
    m_RegionSplitter
    ->GetNumberOfSplits(outputRegion, m_NumberOfStreamDivisions);
  if ( numDivisionsFromSplitter < numDivisions )
    {
    numDivisions = numDivisionsFromSplitter;
    }

  /**
   * Loop over the number of pieces, execute the upstream pipeline on each
   * piece, and copy the results into the output image.
   */
  unsigned int         piece;
  InputImageRegionType streamRegion;
  for ( piece = 0;
        piece < numDivisions && !this->GetAbortGenerateData();
        piece++ )
    {
    streamRegion = m_RegionSplitter->GetSplit(piece, numDivisions,
                                              outputRegion);

    inputPtr->SetRequestedRegion(streamRegion);
    inputPtr->PropagateRequestedRegion();
    inputPtr->UpdateOutputData();

    // copy the result to the proper place in the output. the input
    // requested region determined by the RegionSplitter (as opposed
    // to what the pipeline might have enlarged it to) is used to
    // copy the regions from the input to output
    ImageAlgorithm::Copy( inputPtr, outputPtr, streamRegion, streamRegion );


    this->UpdateProgress( (float)piece / numDivisions );
    }

  /**
   * If we ended due to aborting, push the progress up to 1.0 (since
   * it probably didn't end there)
   */
  if ( !this->GetAbortGenerateData() )
    {
    this->UpdateProgress(1.0);
    }

  // Notify end event observers
  this->InvokeEvent( EndEvent() );

  /**
   * Now we have to mark the data as up to data.
   */
  for ( idx = 0; idx < this->GetNumberOfOutputs(); ++idx )
    {
    if ( this->GetOutput(idx) )
      {
      this->GetOutput(idx)->DataHasBeenGenerated();
      }
    }

  /**
   * Release any inputs if marked for release
   */
  this->ReleaseInputs();

  // Mark that we are no longer updating the data in this filter
  this->m_Updating = false;
}
} // end namespace itk

#endif
