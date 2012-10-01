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
#ifndef RAYONIXHS_H
#define RAYONIXHS_H

//#include <ostream>

#include "HwInterface.h"
#include "HwBufferMgr.h"
#include "SizeUtils.h"

#include "FrameStatusCb.h"
#include "RayonixHsSyncCtrlObj.h"

namespace lima {
namespace RayonixHs {

enum DETECTOR_STATUS {
   DETECTOR_STATUS_IDLE,
   DETECTOR_STATUS_INTEGRATING
};

class Camera : public HwMaxImageSizeCallbackGen {
   friend class Interface;

	public:
		Camera();
		~Camera();

		void getDetectorModel(std::string &model);
		void getDetectorType(std::string &model);
		
		HwInterface::StatusType::Basic getStatus();

		void getMaxImageSize(Size& max_image_size);
	        void getPixelSize(double &x, double &y);

		void reset();

		bool acquiring() { return m_acquiring; }
		void prepareAcq();
		void startAcq();
		void stopAcq();

		void getTrigMode(TrigMode &mode);
		void setTrigMode(TrigMode mode);
		bool checkTrigMode(TrigMode mode);

		void getImageType(ImageType &type);
		void setImageType(ImageType type);

		void setNbFrames(int nb_frames);
		void getNbFrames(int& nb_frames);
		int getNbAcquiredFrames();

		void setExpTime(double exp_time);
		void getExpTime(double& exp_time);

		void setLatTime(double lat_time);
		void getLatTime(double& lat_time);

		void setBin(const Bin& bin);
		void getBin(Bin& bin);
		void checkBin(Bin& bin);

		void setRoi(const Roi& roi);
		void getRoi(Roi& roi);
		void checkRoi(Roi& roi);

		void setFrameDim(const FrameDim& frame_dim);
		void getFrameDim(FrameDim& frame_dim);

		SoftBufferCtrlObj* getBufferCtrlObj();

	private:
		void init();

		BufferCtrlObj *m_buffer_ctrl_obj;

		SyncCtrlObj* m_sync;

		FrameDim m_frame_dim;

		double m_exp_time;
		double m_lat_time;
		int m_nb_frames;

		Size m_max_image_size;

		DETECTOR_STATUS m_detector_status;

		FrameStatusCb *m_frame_status_cb;

		craydl::RxDetector *m_rx_detector;

		volatile bool m_acquiring;

		//void acquisitionComplete();

};

//LIBRAYONIXHS_API std::ostream& operator <<(std::ostream& os, Camera& simu);

} //namespace rayonixhs
} // namespace lima

#endif // RAYONIXHS_H
