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
#ifndef HWSAVINGCTRLOBJ_H
#define HWSAVINGCTRLOBJ_H
#include <string>
#include <map>
#include <list>
#include "LimaCompatibility.h"
#include "Debug.h"

namespace lima
{
  class LIMACORE_API HwSavingCtrlObj
  {
    DEB_CLASS(DebModHardware,"HwSavingCtrlObj");
  public:
    typedef std::map<std::string,std::string> HeaderMap;

    static const int COMMON_HEADER = 0x1; ///< if support common header @see setCommonHeader
    static const int MANUAL_WRITE = 0x2; ///< if support manual write @see writeFrame

    explicit HwSavingCtrlObj(int capabilities = 0);
    virtual ~HwSavingCtrlObj();
    
    virtual void setActive(bool) = 0;
    virtual void setDirectory(const std::string&) = 0;
    virtual void setPrefix(const std::string&) = 0;
    virtual void setSuffix(const std::string&) = 0;

    virtual void getSaveFormat(std::list<std::string> &format_list) const = 0;
    virtual void setSaveFormat(const std::string &format) = 0;

    int getCapabilities() const;

    virtual void writeFrame(int frame_nr = -1,int nb_frames = 1);

    virtual void setCommonHeader(const HeaderMap&);
    virtual void resetCommonHeader();

    class Callback
    {
    public:
      virtual ~Callback() {}
      virtual bool newFrameWrite(int frame_id) = 0;
    };
    
    void registerCallback(Callback *cbk);
    void unregisterCallback(Callback *cbk);

  protected:
    /** @brief this methode should be call we a new frame is written
	@param frame_id the frame id of the saved frame
     */
    bool newFrameWrite(int frame_id);

    int		m_caps;
  private:
    Callback*	m_callback;
  };
}
#endif
