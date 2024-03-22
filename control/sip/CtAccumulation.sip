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

class CtAccumulation
{
%TypeHeaderCode
#include "lima/CtAccumulation.h"
using namespace lima;
%End
public:
  typedef std::list<std::list<long long> > saturatedCounterResult;

  enum Filter { FILTER_NONE, FILTER_THRESHOLD_MIN, FILTER_OFFSET_THEN_THRESHOLD_MIN };
  enum Operation { ACC_SUM, ACC_MEAN, ACC_MEDIAN };                        

  struct Parameters
  {
    enum Mode {STANDARD,THRESHOLD_BEFORE, OFFSET_THEN_THRESHOLD_BEFORE};
    Parameters();
    void reset();
    
    bool        active;	///< if true do the calculation
    long long   pixelThresholdValue; ///< value which determine the threshold of the calculation
    ImageType   pixelOutputType; ///< pixel ouptut type

    bool	  	savingFlag; ///< saving flag if true save saturatedImageCounter
    std::string 	savePrefix; ///< prefix filename of saturatedImageCounter (default is saturated_image_counter)
    Mode		mode;
    long long		thresholdB4Acc; ///< value used in mode THRESHOLD_BEFORE
    SIP_PYOBJECT __repr__() const;
%MethodCode
    LIMA_REPR_CODE
%End
  };
  
  class ThresholdCallback
  {
   public:
    ThresholdCallback();
    virtual ~ThresholdCallback();

    int m_max;

   protected:
    virtual void aboveMax(Data&,long long value) = 0;
  };

  CtAccumulation(CtControl&);
  ~CtAccumulation();

  // --- accumulation adn concatenation parameters

  void setParameters(const Parameters &pars);
  void getParameters(Parameters& pars /Out/) const;
  
  void setActive(bool activeFlag);
  void getActive(bool &activeFlag /Out/);

  void setPixelThresholdValue(long long pixelThresholdValue);
  void getPixelThresholdValue(long long &pixelThresholdValue /Out/) const;

  void setOutputType(ImageType pixelOutputType);
  void getOutputType(ImageType &pixelOutputType /Out/) const;

  void getMaxNbBuffers(int &max_nb_buffers /Out/);

  void setSavingFlag(bool savingFlag);
  void getSavingFlag(bool &savingFlag /Out/) const;

  void setSavePrefix(const std::string &savePrefix);
  void getSavePrefix(std::string &savePrefix /Out/) const;

  void getMode(Parameters::Mode& mode) const;
  void setMode(Parameters::Mode mode);

  void getFilter(Filter& filter) const;
  void setFilter(Filter filter);

  void getOperation(Operation& acc) const;
  void setOperation(Operation acc);

  void getThresholdBefore(long long&) const;
  void setThresholdBefore(const long long&);

  void getOffsetBefore(long long&) const;
  void setOffsetBefore(const long long&);


  // --- variable and data result of Concatenation or Accumulation

  void readSaturatedImageCounter(Data& /Out/,long frameNumber = -1);
  void readSaturatedSumCounter(saturatedCounterResult &result /Out/,int from = -1);

  // --- Mask image to calculate sum counter
  void setMask(Data&);


  // --- Callback to monitor detector saturation

  void registerThresholdCallback(ThresholdCallback &cb);
  void unregisterThresholdCallback(ThresholdCallback &cb);
  
private:
  CtAccumulation(const CtAccumulation&);
};
