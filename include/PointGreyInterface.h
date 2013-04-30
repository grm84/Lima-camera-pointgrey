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
#ifndef POINTGREYINTERFACE_H
#define POINTGREYINTERFACE_H

#include "HwInterface.h"

namespace lima
{
namespace PointGrey
{

class Camera;
class DetInfoCtrlObj;
class SyncCtrlObj;
class VideoCtrlObj;

/*******************************************************************
 * \class Interface
 * \brief PointGrey hardware interface
 *******************************************************************/

class Interface : public HwInterface
{
	DEB_CLASS_NAMESPC(DebModCamera, "PointGreyInterface", "PointGrey");

public:
	Interface(Camera& cam);
	virtual ~Interface();

	//- From HwInterface
	virtual void getCapList(CapList&) const;
	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();

	//- PointGrey Specific
	void getAutoExpTime(bool &auto_frame_rate) const;
	void setAutoExpTime(bool auto_exp_time);

	void getAutoGain(bool& auto_gain) const;
	void setAutoGain(bool auto_gain);

private:
	Camera&         m_cam;
	CapList         m_cap_list;
	DetInfoCtrlObj *m_det_info;
	SyncCtrlObj    *m_sync;
	VideoCtrlObj   *m_video;
};

} // namespace PointGrey
} // namespace lima

#endif // POINTGREYINTERFACE_H
