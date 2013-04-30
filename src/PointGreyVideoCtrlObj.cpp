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

#include "PointGreyVideoCtrlObj.h"
#include "PointGreyCamera.h"

using namespace lima;
using namespace lima::PointGrey;

/*******************************************************************
 * \brief VideoCtrlObj constructor
 *******************************************************************/
VideoCtrlObj::VideoCtrlObj(Camera &cam)
	: m_cam(cam)
	, m_live(false)
{
	DEB_CONSTRUCTOR();
	m_cam.m_video = this;
}


void VideoCtrlObj::getSupportedVideoMode(std::list<VideoMode> &mode_list) const
{
	DEB_MEMBER_FUNCT();
	mode_list.push_back(Y8);
	mode_list.push_back(Y16);
}

void VideoCtrlObj::getVideoMode(VideoMode& mode) const
{
	DEB_MEMBER_FUNCT();
	m_cam.getVideoMode(mode);
	DEB_RETURN() << DEB_VAR1(mode);
}

void VideoCtrlObj::setVideoMode(VideoMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);
	m_cam.setVideoMode(mode);
}

void VideoCtrlObj::getLive(bool &live) const
{
	DEB_MEMBER_FUNCT();
	live = m_live;
	DEB_RETURN() << DEB_VAR1(live);
}

void VideoCtrlObj::setLive(bool live)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(live);
	m_live = live;
	if (live){
		m_cam.setNbFrames(0);
		m_cam.startAcq();
	}
	else
		m_cam.stopAcq();
}

void VideoCtrlObj::getGain(double &gain) const
{
	DEB_MEMBER_FUNCT();
	m_cam.getGain(gain);
	DEB_RETURN() << DEB_VAR1(gain);
}

void VideoCtrlObj::setGain(double gain)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(gain);
	m_cam.setGain(gain);
}
