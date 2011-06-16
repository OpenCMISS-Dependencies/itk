/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNarrowBandImageFilterBase.txx,v $
  Language:  C++
  Date:      $Date: 2009-05-12 17:22:53 $
  Version:   $Revision: 1.20 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNarrowBandImageFilterBase_txx
#define __itkNarrowBandImageFilterBase_txx

#include "itkNarrowBandImageFilterBase.h"
#include "itkShiftScaleImageFilter.h"

namespace itk {

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::ClearNarrowBand () 
{
  while (!m_NarrowBand->Empty()) 
    {
    m_NarrowBand->Clear();
    }
}

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::CopyInputToOutput()
{
  //   First need to subtract the iso-surface value from the input image.
  typedef ShiftScaleImageFilter<InputImageType, OutputImageType> ShiftScaleFilterType;
  typename ShiftScaleFilterType::Pointer shiftScaleFilter = ShiftScaleFilterType::New();
  shiftScaleFilter->SetInput( this->GetInput()  );
  shiftScaleFilter->SetShift( - m_IsoSurfaceValue );
  shiftScaleFilter->Update();  
  this->GraftOutput(shiftScaleFilter->GetOutput());
}

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::GenerateData () 
{
  if (this->GetState() == Superclass::UNINITIALIZED)
    {
    // Allocate the output image
    typename TOutputImage::Pointer output = this->GetOutput();
    output->SetBufferedRegion(output->GetRequestedRegion());
    output->Allocate();
    
    //Set the number of threads before any other initialization happens
    this->GetMultiThreader()->SetNumberOfThreads(this->GetNumberOfThreads());
    
    // Copy the input image to the output image.  Algorithms will operate
    // directly on the output image and the update buffer.
    this->CopyInputToOutput();
    
    // Perform any other necessary pre-iteration initialization.
    this->Initialize();
    
    // Allocate the internal update buffer.  This takes place entirely within
    // the subclass, since this class cannot define an update buffer type.
    this->AllocateUpdateBuffer();
    
    // Iterative algorithm
    this->SetElapsedIterations (0);

    this->SetStateToInitialized();
    }
  
  //Swapn threads
  NarrowBandImageFilterBaseThreadStruct str;
  str.Filter = this;
  // Initialize the list of time step values that will be generated by the
  // various threads.  There is one distinct slot for each possible thread,
  // so this data structure is thread-safe.
  str.TimeStepList      = new TimeStepType[this->GetNumberOfThreads()];
  str.ValidTimeStepList = new bool        [this->GetNumberOfThreads()];
  
  for (int i =0; i < this->GetMultiThreader()->GetNumberOfThreads(); ++i)
    {
    str.ValidTimeStepList[i] = true;
    str.TimeStepList[i] = NumericTraits<TimeStepType>::Zero;
    }
  
  // Multithread the execution
  this->GetMultiThreader()->SetSingleMethod(this->IterateThreaderCallback, &str);
  
  // It is this method that will results in the creation of the threads
  this->GetMultiThreader()->SingleMethodExecute ();

  if (this->GetManualReinitialization() == false)
    {
    this->SetStateToUninitialized(); // Reset the state once execution is
                                     // completed
    }
  
  // Any further processing of the solution can be done here.
  this->PostProcessOutput();
  
  delete [] str.TimeStepList;
  delete [] str.ValidTimeStepList; 

}

template<class TInputImage, class TOutputImage>
ITK_THREAD_RETURN_TYPE
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::IterateThreaderCallback(void * arg)
{

#ifdef ITK_USE_SPROC
  // Every thread should 'usadd' itself to the arena as the very first thing so
  // as to detect errors (if any) early.
  if (MultiThreader::GetThreadArena() != 0)
    {
    int code= usadd (MultiThreader::GetThreadArena());
    if (code != 0)
      {
      throw ::itk::ExceptionObject(__FILE__, __LINE__, "Thread failed to join SGI arena: error", ITK_LOCATION);
      }
    }
#endif

  unsigned int threadId = ((MultiThreader::ThreadInfoStruct *)(arg))->ThreadID;
      
  NarrowBandImageFilterBaseThreadStruct * str
    = (NarrowBandImageFilterBaseThreadStruct *)
    (((MultiThreader::ThreadInfoStruct *)(arg))->UserData);

  str->Filter->ThreadedIterate(arg, threadId);
  
  return ITK_THREAD_RETURN_VALUE;
}


template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::ThreadedIterate(void * arg, int threadId)
{
  int threadCount;
  ThreadRegionType splitRegion;

  //Implement iterative loop in thread function
  //ThreadedApplyUpdate and ThreadedCalculateChanged
  // is called instead of ApplyUpdate and CalculateChange
  NarrowBandImageFilterBaseThreadStruct * str
    = (NarrowBandImageFilterBaseThreadStruct *)
    (((MultiThreader::ThreadInfoStruct *)(arg))->UserData);
  threadCount = ((MultiThreader::ThreadInfoStruct *)(arg))->NumberOfThreads;

  unsigned int iter = 0;  
  while ( ! (this->ThreadedHalt(arg)) )
    {
    if (threadId == 0)
      {
      this->InitializeIteration(); // An optional method for precalculating
                                 // global values, or otherwise setting up
                                 // for the next iteration
      }
    
    this->WaitForAll();

    //Update region to process for current thread
    // Execute the actual method with appropriate output region
    // first find out how many pieces extent can be split into.
    // Use GetSplitRegion to access partition previously computed by
    // the SplitRegions function in the itkNarrowBand class.

    this->GetSplitRegion(threadId, splitRegion);
     
    //Threaded Calculate Change
    str->ValidTimeStepList[threadId] = false;
    str->TimeStepList[threadId]
      = this->ThreadedCalculateChange(splitRegion, threadId);
    str->ValidTimeStepList[threadId]=true;
    
    this->WaitForAll();
    
    //Calculate the time step
     //Check how is done in itkParallell
    if (threadId == 0)
      {
      str->TimeStep = this->ResolveTimeStep(str->TimeStepList,
                                            str->ValidTimeStepList, threadCount);
      }
     
    this->WaitForAll(); 
        
    //Threaded Apply Update
    this->ThreadedApplyUpdate(str->TimeStep,splitRegion, threadId);

    //Reset ValidTimeStepList
    str->ValidTimeStepList[threadId] = false;
    
    this->WaitForAll();
    
    
    //Do this. Problems accesing data members.
    ++iter;
    if (threadId == 0)
      {
      ++m_Step;
      this->SetElapsedIterations (iter);

      // Invoke the iteration event.
      this->InvokeEvent( IterationEvent() );
      this->InvokeEvent( ProgressEvent() );
      if( this->GetAbortGenerateData() )
        {
        this->InvokeEvent( IterationEvent() );
        this->WaitForAll();
        this->ResetPipeline();
        ProcessAborted e(__FILE__,__LINE__);
        e.SetDescription("Process aborted.");
        e.SetLocation(ITK_LOCATION);
        throw e;
        }
      }
    this->WaitForAll();
    }
}   
  

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::Initialize () 
{
  
  m_Step = 0;
  
  ClearNarrowBand(); 
  CreateNarrowBand();
  
  // SetNarrowBand is expected to be defined in a subclass.
  // It should use the InsertNarrowBandNode function, which takes care of 
  // memory management issues, to create the desired narrow band. 
  
  m_RegionList=m_NarrowBand->SplitBand(this->GetMultiThreader()->GetNumberOfThreads());
  
  // The narrow band is split into multi-threading regions once here for
  // computationally efficiency. Later GetSplitRegions is used to access these
  // partitions. This assumes that the band will not be changed until another
  // call to Initialize(). Any reinitialization function also must call the
  // SplitRegions function.
  
  
  // Allocation of flag variable to check if a given thread touch the outer part
  // of the narrowband. If this part is touched, band should be reinitialized.
  m_TouchedForThread = new bool[this->GetMultiThreader()->GetNumberOfThreads()];
  for (int i = 0; i < this->GetMultiThreader()->GetNumberOfThreads(); i++)
    {
    m_TouchedForThread[i] = false;
    } 

  // A global barrier for all threads.
  m_Barrier->Initialize(this->GetMultiThreader()->GetNumberOfThreads());

}

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::InitializeIteration()
{
  //Set m_Touched flag from threads information
  for (int i = 0; i < this->GetMultiThreader()->GetNumberOfThreads(); i++)
    {
    m_Touched = (m_Touched || m_TouchedForThread[i]);
    m_TouchedForThread[i] = false;
    }
  //Check if we have to reinitialize the narrowband
  if (m_Touched || ((this->GetElapsedIterations() >0)
                    && (this->m_Step == m_ReinitializationFrequency )))
    {
    //Reinitialize the narrowband properly
    CreateNarrowBand();

    // Rebuild the narrow band splits used in multithreading
    m_RegionList=m_NarrowBand->SplitBand(this->GetMultiThreader()->GetNumberOfThreads());
    
    m_Step=0;
    m_Touched = false;
    }
}


template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::ThreadedApplyUpdate(TimeStepType dt,
                      const ThreadRegionType &regionToProcess,
                      int threadId)
{
  //const int INNER_MASK = 2;
  const signed char INNER_MASK = 2;
  typename NarrowBandType::ConstIterator it;
  typename OutputImageType::Pointer image=this->GetOutput();
  typename OutputImageType::PixelType oldvalue;
  typename OutputImageType::PixelType newvalue;
  for (it=regionToProcess.first; it != regionToProcess.last; ++it)
    {
    oldvalue = image->GetPixel(it->m_Index);
    newvalue = oldvalue + dt * it->m_Data;
    //Check whether solution is out the inner band or not
    m_TouchedForThread[threadId] = ( m_TouchedForThread[threadId] ||
                                     ( !(it->m_NodeState & INNER_MASK)
                                       && ( (oldvalue>0) != (newvalue>0))));
    image->SetPixel(it->m_Index, newvalue);
    
    }
}

template <class TInputImage, class TOutputImage>
typename
NarrowBandImageFilterBase<TInputImage, TOutputImage>::TimeStepType
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::ThreadedCalculateChange(const ThreadRegionType &regionToProcess,
                          int itkNotUsed(threadId))
{
  typedef typename OutputImageType::SizeType        OutputSizeType;

  typedef typename FiniteDifferenceFunctionType::NeighborhoodType
    NeighborhoodIteratorType;
  
  typename OutputImageType::Pointer output = this->GetOutput();
  TimeStepType timeStep;
  void *globalData;
  
  // Get the FiniteDifferenceFunction to use in calculations.
  const typename FiniteDifferenceFunctionType::Pointer df
    = this->GetDifferenceFunction();
  const OutputSizeType  radius = df->GetRadius();
  
  // Ask the function object for a pointer to a data structure it will use to
  // manage any global values it needs.  We'll pass this back to the function
  // object at each calculation so that the function object can use it to
  // determine a time step for this iteration.
  globalData = df->GetGlobalDataPointer();
  
  typename NarrowBandType::Iterator bandIt;
  NeighborhoodIteratorType outputIt(radius, output, output->GetRequestedRegion());
  
  for (bandIt = regionToProcess.first; bandIt != regionToProcess.last; ++bandIt)
    {
    outputIt.SetLocation(bandIt->m_Index);
    bandIt->m_Data = df->ComputeUpdate(outputIt, globalData);
    }
  
  // Ask the finite difference function to compute the time step for
  // this iteration.  We give it the global data pointer to use, then
  // ask it to free the global data memory.
  timeStep = df->ComputeGlobalTimeStep(globalData);
  df->ReleaseGlobalDataPointer(globalData);
  
  return timeStep;
}

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::PostProcessOutput( ) 
{
  delete [] m_TouchedForThread;
}  

template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::GetSplitRegion (int i, ThreadRegionType &splitRegion)
{
  splitRegion.first = m_RegionList[i].Begin;
  splitRegion.last = m_RegionList[i].End;
}

template<class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::WaitForAll ()
{
  m_Barrier->Wait();
}


template <class TInputImage, class TOutputImage>
void
NarrowBandImageFilterBase<TInputImage, TOutputImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "IsoSurfaceValue: "
     << static_cast<typename NumericTraits<ValueType>::PrintType>(m_IsoSurfaceValue)
     << std::endl;
}

}// end namespace itk

#endif
