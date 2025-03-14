//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################
#include "lima/SoftOpInternalMgr.h"
using namespace lima;

#include "processlib/Flip.h"
#include "processlib/Binning.h"
#include "processlib/SoftRoi.h"
#include "processlib/Rotation.h"

SoftOpInternalMgr::SoftOpInternalMgr() :
  m_reconstruction_task(NULL),m_end_callback(NULL),
  m_first_processing_in_place(true)
{
}

SoftOpInternalMgr::~SoftOpInternalMgr()
{
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
  if(m_end_callback)
    m_end_callback->unref();
}


void SoftOpInternalMgr::setBin(const Bin &aBin)
{
  m_bin = aBin;
}

void SoftOpInternalMgr::getBin(Bin &aBin) const
{
  aBin = m_bin;
}

void SoftOpInternalMgr::setRoi(const Roi &aRoi)
{
  m_roi = aRoi;
}

void SoftOpInternalMgr::getRoi(Roi &aRoi) const
{
  aRoi = m_roi;
}

void SoftOpInternalMgr::setFlip(const Flip &aFlip)
{
  m_flip = aFlip;
}

void SoftOpInternalMgr::getFlip(Flip &aFlip) const
{
  aFlip = m_flip;
}

void SoftOpInternalMgr::setRotation(RotationMode mode)
{
  m_rotation = mode;
}

void SoftOpInternalMgr::getRotation(RotationMode &mode) const
{
  mode = m_rotation;
}
/** @brief set the reconstruction task.
 *  reconstruction task will be the first task
 */
void SoftOpInternalMgr::setReconstructionTask(LinkTask *aTask)
{
  if(aTask)
    aTask->ref();
  if(m_reconstruction_task)
    m_reconstruction_task->unref();
      
  m_reconstruction_task = aTask;
}

bool SoftOpInternalMgr::hasReconstructionTask()
{
  return (m_reconstruction_task!=NULL);
}

void SoftOpInternalMgr::addTo(TaskMgr &aTaskMgr,
			      int &aLastStage,
			      bool registerCallback,
			      bool skipReconstruction) const
{
  bool processingInPlace = m_first_processing_in_place;
  aLastStage = 0;
  if(m_reconstruction_task && !skipReconstruction)
    {
      aTaskMgr.setLinkTask(aLastStage,m_reconstruction_task);
      ++aLastStage;
    }

  Tasks::Binning *aBinTaskPt = NULL;
  if(m_bin.getX() > 1 || m_bin.getY() > 1)
    {
      aBinTaskPt = new Tasks::Binning();
      aBinTaskPt->setProcessingInPlace(processingInPlace);
      processingInPlace = true;
      if(m_rotation == Rotation_90 || m_rotation == Rotation_270)
        {
          aBinTaskPt->mXFactor = m_bin.getY();
          aBinTaskPt->mYFactor = m_bin.getX();
        }
      else
        {
          aBinTaskPt->mXFactor = m_bin.getX();
          aBinTaskPt->mYFactor = m_bin.getY();
        }
      aTaskMgr.setLinkTask(aLastStage,aBinTaskPt);
      aBinTaskPt->unref();
      ++aLastStage;
    }

  Tasks::Flip *aFlipTaskPt = NULL;
  if(m_flip.x || m_flip.y)
    {
      Tasks::Flip::FLIP_MODE aMode = Tasks::Flip::FLIP_NONE;
      if(m_flip.x && m_flip.y)
        aMode = Tasks::Flip::FLIP_ALL;
      else if(m_flip.x)
        aMode = Tasks::Flip::FLIP_X;
      else
        aMode = Tasks::Flip::FLIP_Y;

      aFlipTaskPt = new Tasks::Flip();
      aFlipTaskPt->setProcessingInPlace(processingInPlace);
      processingInPlace = true;

      aFlipTaskPt->setFlip(aMode);
      aTaskMgr.setLinkTask(aLastStage,aFlipTaskPt);
      aFlipTaskPt->unref();
      ++aLastStage;
    }

  Tasks::Rotation *aRotationTaskPt = NULL;
  if(m_rotation != Rotation_0)
    {
      Tasks::Rotation::Type aMode;
      switch(m_rotation)
      {
        case Rotation_180: aMode = Tasks::Rotation::R_180;break;
        case Rotation_270: aMode = Tasks::Rotation::R_270;break;
        default: aMode = Tasks::Rotation::R_90;break;
      }
      aRotationTaskPt = new Tasks::Rotation();
      aRotationTaskPt->setProcessingInPlace(processingInPlace);
      processingInPlace = true;

      aRotationTaskPt->setType(aMode);
      aTaskMgr.setLinkTask(aLastStage,aRotationTaskPt);
      aRotationTaskPt->unref();
      ++aLastStage;
    }
  Tasks::SoftRoi *aSoftRoiTaskPt = NULL;
  if(m_roi.isActive())
    {
      Point topl= m_roi.getTopLeft();
      Point botr= m_roi.getBottomRight();
      aSoftRoiTaskPt = new Tasks::SoftRoi();
      aSoftRoiTaskPt->setProcessingInPlace(processingInPlace);
      processingInPlace = true;

      aSoftRoiTaskPt->setRoi(topl.x, botr.x, topl.y, botr.y);
      aTaskMgr.setLinkTask(aLastStage,aSoftRoiTaskPt);
      aSoftRoiTaskPt->unref();
      ++aLastStage;
    }
  bool removeReconstructionTaskCallback = true;
  //Check now what is the last task to add a callback
  if(registerCallback && aLastStage)
    {
      if(aSoftRoiTaskPt)
        aSoftRoiTaskPt->setEventCallback(m_end_callback);
      else if(aRotationTaskPt)
        aRotationTaskPt->setEventCallback(m_end_callback);
      else if(aFlipTaskPt)
        aFlipTaskPt->setEventCallback(m_end_callback);
      else if(aBinTaskPt)
        aBinTaskPt->setEventCallback(m_end_callback);
      else if(m_reconstruction_task)
        m_reconstruction_task->setEventCallback(m_end_callback),removeReconstructionTaskCallback = false;

      //Clear eventCallback for reconstruction task
      if(m_reconstruction_task && removeReconstructionTaskCallback)
        m_reconstruction_task->setEventCallback(NULL);
    }
}

