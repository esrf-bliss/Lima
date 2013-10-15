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
#include "HwFrameInfo.h"
#include "DirectoryEventUtils.h"

namespace lima
{
  class LIMACORE_API HwSavingCtrlObj
  {
    DEB_CLASS(DebModHardware,"HwSavingCtrlObj");
  public:
    typedef std::map<std::string,std::string> HeaderMap;

    //Capabilities
    static const int COMMON_HEADER = 0x1; ///< if support common header @see setCommonHeader
    static const int MANUAL_WRITE = 0x2; ///< if support manual write @see writeFrame
    static const int MANUAL_READ = 0x4;	///< if support manual read @see readFrame
    //Basic Managed format
    static const char *RAW_FORMAT_STR;
    static const char *EDF_FORMAT_STR;
    static const char *CBF_FORMAT_STR;
    static const char *TIFF_FORMAT_STR;

    explicit HwSavingCtrlObj(int capabilities = 0);
    virtual ~HwSavingCtrlObj();
    
    void setActive(bool);
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

    virtual void setCommonHeader(const HeaderMap&);
    virtual void resetCommonHeader();

    void prepare();
    void start();
    void stop();

    class Callback
    {
    public:
      virtual ~Callback() {}
#ifdef __linux__
      virtual void prepare(const DirectoryEvent::Parameters &) {};
      virtual bool newFrameWritten(int frame_id) = 0;
#endif
    };
    
    void registerCallback(Callback *cbk);
    void unregisterCallback(Callback *cbk);

  protected:
    virtual void _setActive(bool) {}
    virtual void _prepare() {}
    virtual void _start() {}
    /** @brief return the full path of acquired image
     */
    std::string _getFullPath(int image_number) const;

    int		m_caps;

    bool	m_active;
    std::string m_directory;
    std::string m_prefix;
    std::string m_suffix;
    long 	m_next_number;
    std::string m_file_format;
    std::string m_index_format;

  private:
    class DirectoryCallback;

    Callback*		m_callback;
#ifdef __linux__
    DirectoryCallback*  m_dir_cbk;
    DirectoryEvent 	m_dir_event;
#endif
  };
}
#endif
