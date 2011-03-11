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
#ifndef __itkReflectiveImageRegionConstIterator_txx
#define __itkReflectiveImageRegionConstIterator_txx

#include "itkReflectiveImageRegionConstIterator.h"

namespace itk
{
template< class TImage >
ReflectiveImageRegionConstIterator< TImage >
::ReflectiveImageRegionConstIterator():ImageConstIteratorWithIndex< TImage >()
{
  m_BeginOffset.Fill(0);
  m_EndOffset.Fill(0);
  this->GoToBegin();
}

template< class TImage >
ReflectiveImageRegionConstIterator< TImage >
::ReflectiveImageRegionConstIterator(TImage *ptr, const RegionType & region):
  ImageConstIteratorWithIndex< TImage >(ptr, region)
{
  m_BeginOffset.Fill(0);
  m_EndOffset.Fill(0);
  this->GoToBegin();
}

template< class TImage >
ReflectiveImageRegionConstIterator< TImage >
::ReflectiveImageRegionConstIterator(const Self & it)
{
  this->Operator = ( it );
  this->GoToBegin();
}

template< class TImage >
ReflectiveImageRegionConstIterator< TImage >
::ReflectiveImageRegionConstIterator(
  const ImageConstIteratorWithIndex< TImage > & it)
{
  this->ImageConstIteratorWithIndex< TImage >::operator=(it);

  m_BeginOffset.Fill(0);
  m_EndOffset.Fill(0);
}

template< class TImage >
ReflectiveImageRegionConstIterator< TImage > &
ReflectiveImageRegionConstIterator< TImage >
::operator=(const Self & it)
{
  this->ImageConstIteratorWithIndex< TImage >::operator=(it);
  m_BeginOffset = it.m_BeginOffset;
  m_EndOffset = it.m_EndOffset;
  return *this;
}

template< class TImage >
void
ReflectiveImageRegionConstIterator< TImage >
::GoToBegin(void)
{
  this->m_PositionIndex = this->m_BeginIndex + this->m_BeginOffset;
  this->m_Position = this->m_Image->GetBufferPointer()
                     + this->m_Image->ComputeOffset(this->m_PositionIndex);

  this->m_Remaining = false;
  for ( unsigned int i = 0; i < TImage::ImageDimension; ++i )
    {
    SizeValueType size = this->m_Region.GetSize()[i];
    if ( size > 0 ) { this->m_Remaining = true; }
    }

  for ( unsigned int i = 0; i < TImage::ImageDimension; i++ )
    {
    m_IsFirstPass[i] = true;
    }
}

template< class TImage >
bool
ReflectiveImageRegionConstIterator< TImage >
::IsReflected(unsigned int dim) const
{
  return !m_IsFirstPass[dim];
}

template< class TImage >
void
ReflectiveImageRegionConstIterator< TImage >
::FillOffsets(const OffsetValueType & value)
{
  m_BeginOffset.Fill(value);
  m_EndOffset.Fill(value);
}

//----------------------------------------------------------------------
//  Advance along the line
//----------------------------------------------------------------------
template< class TImage >
ReflectiveImageRegionConstIterator< TImage > &
ReflectiveImageRegionConstIterator< TImage >
::operator++()
{
  this->m_Remaining = false;
  for ( unsigned int in = 0; in < TImage::ImageDimension; in++ )
    {
    if ( m_IsFirstPass[in] )
      {
      this->m_PositionIndex[in]++;
      if ( this->m_PositionIndex[in] < this->m_EndIndex[in] )
        {
        this->m_Position += this->m_OffsetTable[in];
        this->m_Remaining = true;
        break;
        }
      else
        {
        this->m_PositionIndex[in] = this->m_EndIndex[in] - m_EndOffset[in] - 1;
        m_IsFirstPass[in] = false;
        this->m_Remaining = true;
        break;
        }
      }
    else
      {
      this->m_PositionIndex[in]--;
      if ( this->m_PositionIndex[in] >= this->m_BeginIndex[in] )
        {
        this->m_Position -= this->m_OffsetTable[in];
        this->m_Remaining = true;
        break;
        }
      else
        {
        this->m_PositionIndex[in] = this->m_BeginIndex[in] + m_BeginOffset[in];
        m_IsFirstPass[in] = true;
        }
      }
    }

  if ( !this->m_Remaining ) // It will not advance here otherwise
    {
    this->m_Position = this->m_End;
    }

  return *this;
}
} // end namespace itk

#endif