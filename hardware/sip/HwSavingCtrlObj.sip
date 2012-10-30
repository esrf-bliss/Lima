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
class HwSavingCtrlObj
{
%TypeHeaderCode
#include <HwSavingCtrlObj.h>
using namespace lima;
%End
public:
  static const int COMMON_HEADER;
  static const int MANUAL_WRITE;
  static const int MANUAL_READ;
  
  //Basic Managed format
  static const char *RAW_FORMAT_STR;
  static const char *EDF_FORMAT_STR;
  static const char *CBF_FORMAT_STR;

  explicit HwSavingCtrlObj(int capabilities = 0);
  virtual ~HwSavingCtrlObj();
    
  virtual void setActive(bool);
  bool isActive() const;

  void setDirectory(const std::string&);
  void setPrefix(const std::string&);
  void setSuffix(const std::string&);
  void setNextNumber(long number);
  void setIndexFormat(const std::string&);
  
  void setSaveFormat(const std::string &format);
  virtual void getPossibleSaveFormat(std::list<std::string> &format_list) const = 0;

  int getCapabilities() const;

  virtual void writeFrame(int frame_nr = -1,int nb_frames = 1);
  virtual void readFrame(HwFrameInfoType&,int frame_nr);

  //virtual void setCommonHeader(const std::map<std::string,std::string>&);
  virtual void resetCommonHeader();

  class Callback
  {
  public:
    virtual ~Callback();
    virtual void prepare(const DirectoryEvent::Parameters &);
    virtual bool newFrameWritten(int frame_id) = 0;
  };
    
  void registerCallback(Callback *cbk);
  void unregisterCallback(Callback *cbk);

protected:
  virtual void _setActive(bool);
  virtual void _prepare();
  virtual void _start();
  /** @brief return the full path of acquired image
   */
  std::string _getFullPath(int image_number) const;


};
