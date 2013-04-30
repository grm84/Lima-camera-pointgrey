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
#ifndef POINTGREYVIDEOCTRLOBJ_H
#define POINTGREYVIDEOCTRLOBJ_H

#include "HwVideoCtrlObj.h"

namespace lima
{
namespace PointGrey
{

class Camera;

/*******************************************************************
 * \class VideoCtrlObj
 * \brief Control object providing PointGrey Video interface
 *******************************************************************/
class VideoCtrlObj : public HwVideoCtrlObj
{
	DEB_CLASS_NAMESPC(DebModCamera, "VideoCtrlObj", "PointGrey");
public:
	VideoCtrlObj(Camera& cam);
	virtual ~VideoCtrlObj() {};

	virtual void getSupportedVideoMode(std::list<VideoMode> &aList) const;
	virtual void getVideoMode(VideoMode&) const;
	virtual void setVideoMode(VideoMode);

	virtual void getLive(bool&) const;
	virtual void setLive(bool);

	virtual void getGain(double&) const;
	virtual void setGain(double);

	virtual void checkBin(Bin& bin){};
	virtual void setBin(const Bin&){};

	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi){};
	virtual void setRoi(const Roi&){};

private:
	Camera& m_cam;
	bool m_live;
};

} // namespace PointGrey
} // namespace lima

#endif // POINTGREYVIDEOCTRLOBJ_H
