#include "PointGreyCamera.h"

using namespace lima;
using namespace lima::PointGrey;
using namespace std;

const static int DEFAULT_TIME_OUT = 600000; // 10 minutes

//---------------------------
//- utility functions
//---------------------------


//---------------------------
//- Ctor
//---------------------------
Camera::Camera(const string& camera_ip, int packet_size)
        : m_nb_frames(1),
          m_status(Ready),
          m_image_number(0),
          m_exp_time(1.),
          m_timeout(DEFAULT_TIME_OUT),
          m_latency_time(0.),
          m_camera_ip(camera_ip)
{
    DEB_CONSTRUCTOR();

    FlyCapture2::BusManager busmgr;
    FlyCapture2::CameraInfo	caminfo;
    FlyCapture2::PGRGuid pgrguid;
    FlyCapture2::Error error;

    unsigned int nCameras;
    m_pgcam = new FlyCapture2::Camera();

    error = busmgr.GetNumOfCameras(&nCameras);
    if (error != FlyCapture2::PGRERROR_OK)
    {
    	THROW_HW_ERROR(Error) << "Failed to create bus manager!";
    }

    if (nCameras > 0)
    {
    	THROW_HW_ERROR(Error) << "No cameras found!";
    }

	error = busmgr.GetCameraFromIndex(0, &pgrguid);
    if (error != FlyCapture2::PGRERROR_OK)
    {
    	THROW_HW_ERROR(Error) << "Camera not found!";
    }

    error = m_pgcam->Connect(&pgrguid);
    if (error != FlyCapture2::PGRERROR_OK)
    {
    	THROW_HW_ERROR(Error) << "Failed to connect to camera";
    }

    error = m_pgcam->GetCameraInfo(&caminfo);
    {
    	THROW_HW_ERROR(Error) << "Failed to get camera info";
    }

    printf(
        "\n*** CAMERA INFORMATION ***\n"
         "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        caminfo.serialNumber,
        caminfo.modelName,
        caminfo.vendorName,
        caminfo.sensorInfo,
        caminfo.sensorResolution,
        caminfo.firmwareVersion,
        caminfo.firmwareBuildTime);

}

//---------------------------
//- Dtor
//---------------------------
Camera::~Camera()
{
    DEB_DESTRUCTOR();

    delete m_pgcam;
}

void Camera::prepareAcq()
{
    DEB_MEMBER_FUNCT();
}

//---------------------------
//- Camera::start()
//---------------------------
void Camera::startAcq()
{
    DEB_MEMBER_FUNCT();
}

//---------------------------
//- Camera::stopAcq()
//---------------------------
void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(size);
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getImageType(ImageType& type)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setImageType(ImageType type)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(type);
}
//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorType(string& type)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorModel(string& model)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(model);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
    return &m_buffer_ctrl_obj;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setTrigMode(TrigMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getTrigMode(TrigMode& mode)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(mode);
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setExpTime(double exp_time)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExpTime(double& exp_time)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setLatTime(double lat_time)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTime(double& lat_time)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExposureTimeRange(double& min_expo, double& max_expo) const
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR2(min_expo, max_expo);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTimeRange(double& min_lat, double& max_lat) const
{   
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR2(min_lat, max_lat);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setNbFrames(int nb_frames)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(nb_frames);
    m_nb_frames = nb_frames;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getNbFrames(int& nb_frames)
{
    DEB_MEMBER_FUNCT();
    nb_frames = m_nb_frames;
    DEB_RETURN() << DEB_VAR1(nb_frames);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getNbHwAcquiredFrames(int &nb_acq_frames)
{ 
    DEB_MEMBER_FUNCT();    
    nb_acq_frames = m_image_number;
}
  
//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getStatus(Camera::Status& status)
{
    DEB_MEMBER_FUNCT();
    status = m_status;
    DEB_RETURN() << DEB_VAR1(DEB_HEX(status));
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getFrameRate(double& frame_rate)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(frame_rate);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setTimeout(int timeout)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(timeout);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(set_roi);
    DEB_RETURN() << DEB_VAR1(hw_roi);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setRoi(const Roi& ask_roi)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(ask_roi);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getRoi(Roi& hw_roi)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(hw_roi);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::checkBin(Bin &aBin)
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(aBin);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setBin(const Bin &aBin)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(aBin);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getBin(Bin &aBin)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(aBin);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
bool Camera::isBinnigAvailable(void)
{
    DEB_MEMBER_FUNCT();
    return false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setInterPacketDelay(int ipd)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(ipd);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setAutoGain(bool auto_gain)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(auto_gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getAutoGain(bool& auto_gain) const
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(auto_gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setGain(double gain)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getGain(double& gain) const
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setFrameTransmissionDelay(int ftd)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(ftd);
}

//---------------------------
//- Camera::reset()
//---------------------------
void Camera::reset(void)
{
    DEB_MEMBER_FUNCT();
}
//---------------------------
