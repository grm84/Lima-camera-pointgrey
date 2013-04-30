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

#include "PointGreySyncCtrlObj.h"
#include "PointGreyCamera.h"

using namespace lima;
using namespace lima::PointGrey;


/*******************************************************************
 * \brief SyncCtrlObj constructor
 *******************************************************************/
SyncCtrlObj::SyncCtrlObj(Camera& cam)
	: m_cam(cam)
{
	DEB_CONSTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	return true;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	if (!checkTrigMode(trig_mode))
		THROW_HW_ERROR(InvalidValue) << "Invalid " << DEB_VAR1(trig_mode);
	m_cam.setTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	m_cam.getTrigMode(trig_mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setExpTime(double exp_time)
{
	m_cam.setExpTime(exp_time);
	ValidRangesType valid_ranges;
	getValidRanges(valid_ranges);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getExpTime(double& exp_time)
{
	m_cam.getExpTime(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setLatTime(double lat_time)
{
	m_cam.setLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getLatTime(double& lat_time)
{
	m_cam.getLatTime(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	m_cam.setNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	m_cam.getNbFrames(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	DEB_MEMBER_FUNCT();
	double min_time;
	double max_time;

	m_cam.getExpTimeRange(min_time, max_time);
	valid_ranges.min_exp_time = min_time;
	valid_ranges.max_exp_time = max_time;

	m_cam.getLatTimeRange(min_time, max_time);
	valid_ranges.min_lat_time = min_time;
	valid_ranges.max_lat_time = max_time;
}
