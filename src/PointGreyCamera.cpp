#include "PointGreyCamera.h"
#include "PointGreyInterface.h"

using namespace lima;
using namespace lima::PointGrey;
using namespace std;

//---------------------------
//- utility functions
//---------------------------

//---------------------------
//
//---------------------------
Camera::Camera(const std::string& camera_ip, const int camera_serial_no)
    : m_nb_frames(1)
    , m_status(Ready)
    , m_image_number(0)
    , m_started(false)
    , m_continue_acq(false)
    , m_video(NULL)
    , m_sync(NULL)
    // camera properties
    , m_frame_rate_property(FlyCapture2::FRAME_RATE)
    , m_exp_time_property(FlyCapture2::SHUTTER)
    , m_gain_property(FlyCapture2::GAIN)
    // property info
    , m_frame_rate_property_info(FlyCapture2::FRAME_RATE)
    , m_exp_time_property_info(FlyCapture2::SHUTTER)
    , m_gain_property_info(FlyCapture2::GAIN)
{
    DEB_CONSTRUCTOR();

    FlyCapture2::BusManager busmgr;
    FlyCapture2::PGRGuid pgrguid;

    unsigned int nb_cameras;
    m_camera = new FlyCapture2::Camera();

    m_error = busmgr.GetNumOfCameras(&nb_cameras);
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Failed to create bus manager: " << m_error.GetDescription();

    if (nb_cameras < 1)
    	THROW_HW_ERROR(Error) << "No cameras found";

    if (camera_serial_no)
    {
    	m_error = busmgr.GetCameraFromSerialNumber(camera_serial_no, &pgrguid);
    	if (m_error != FlyCapture2::PGRERROR_OK)
	    THROW_HW_ERROR(Error) << "Camera not found: " << m_error.GetDescription();
    }
    else
        // TODO: lookup via ip address
    	THROW_HW_ERROR(Error) << "Camera lookup via IP address has not been implemented";

    m_error = m_camera->Connect(&pgrguid);
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Failed to connect to camera: " << m_error.GetDescription();

    m_error = m_camera->GetCameraInfo(&m_camera_info);
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Failed to get camera info: "<<  m_error.GetDescription();

    bool fmt7_supported;
    m_fmt7_info.mode = FlyCapture2::MODE_0;
    m_error = m_camera->GetFormat7Info( &m_fmt7_info, &fmt7_supported );

    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Failed to get Format7 info: "<<  m_error.GetDescription();

    if (!fmt7_supported)
    	THROW_HW_ERROR(Error) << "Format7 is not supported: ";

    // query for supported pixel formats
    static const FlyCapture2::PixelFormat mono_formats[] = {
        FlyCapture2::PIXEL_FORMAT_MONO16,
	FlyCapture2::PIXEL_FORMAT_MONO8,
	FlyCapture2::UNSPECIFIED_PIXEL_FORMAT // == 0
    };

    const FlyCapture2::PixelFormat *pixel_format;

    for (pixel_format=mono_formats; *pixel_format; pixel_format++)
        if ((*pixel_format & m_fmt7_info.pixelFormatBitField) > 0)
	    break;

    if (!*pixel_format)
    	THROW_HW_ERROR(Error) << "Unable to set pixel format for the camera!";

    // Set image format settings
    m_fmt7_image_settings.mode = m_fmt7_info.mode;
    m_fmt7_image_settings.offsetX = 0;
    m_fmt7_image_settings.offsetY = 0;
    m_fmt7_image_settings.width = m_fmt7_info.maxWidth;
    m_fmt7_image_settings.height = m_fmt7_info.maxHeight;
    m_fmt7_image_settings.pixelFormat = *pixel_format;

    bool valid;

    m_error = m_camera->ValidateFormat7Settings(&m_fmt7_image_settings,
						&valid,
						&m_fmt7_packet_info);
    if ( m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to validate image format settings:" << m_error.GetDescription();
    if ( !valid )
    	THROW_HW_ERROR(Error) << "Unsupported image format settings";

    m_error = m_camera->SetFormat7Configuration(&m_fmt7_image_settings,
						m_fmt7_packet_info.recommendedBytesPerPacket);

    if ( m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to apply image format settings:" << m_error.GetDescription();


    // get frame rate property
    _getPropertyInfo(&m_frame_rate_property_info);
    _getProperty(&m_frame_rate_property);

    // set auto frame rate, if supported
    // control off means that the current frame rate will not limit
    // the range of valid values for the exposure time property. the
    // frame rate will be adjusted to allow the requested exposure time.
    m_frame_rate_property.onOff = false;
    m_frame_rate_property.autoManualMode = m_frame_rate_property_info.autoSupported;

    _setProperty(&m_frame_rate_property);

    // get exp time and property
    _getPropertyInfo(&m_exp_time_property_info);
    _getProperty(&m_exp_time_property);

    // set auto exp time, if supported
    m_exp_time_property.onOff = true;
    m_exp_time_property.autoManualMode = m_exp_time_property_info.autoSupported;

    _setProperty(&m_exp_time_property);

    // Get gain property
    _getPropertyInfo(&m_gain_property_info);
    _getProperty(&m_gain_property);

    // set auto gain, if supported
    m_gain_property.onOff = true;
    m_gain_property.autoManualMode = m_gain_property_info.autoSupported;

    _setProperty(&m_gain_property);
}

//---------------------------
//
//---------------------------
Camera::~Camera()
{
    DEB_DESTRUCTOR();
    // disconnect from camera
    m_camera->Disconnect();
    delete m_camera;
}

void Camera::prepareAcq()
{
    DEB_MEMBER_FUNCT();
    m_image_number = 0;
}

//---------------------------
//
//---------------------------
void Camera::startAcq()
{
    DEB_MEMBER_FUNCT();

    DEB_TRACE() << "Start acquisition";
    m_continue_acq = true;
    m_video->getBuffer().setStartTimestamp(Timestamp::now());

    m_error = m_camera->StartCapture(_newFrameCBK, (void *)this);
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to start image capture" << m_error.GetDescription();

    m_status = Camera::Readout;
    m_started = true;
}

//---------------------------
//
//---------------------------
void Camera::stopAcq()
{
    DEB_MEMBER_FUNCT();

    DEB_TRACE() << "Stop acquisition";

    if (!m_started)
        return;

    m_continue_acq = false;

    m_error = m_camera->StopCapture();
    if (m_error != FlyCapture2::PGRERROR_OK)
        THROW_HW_ERROR(Error) << "Unable to stop image capture" << m_error.GetDescription();

    m_status = Camera::Ready;
    m_started = false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorImageSize(Size& size)
{
    DEB_MEMBER_FUNCT();
    size = Size(m_fmt7_info.maxWidth, m_fmt7_info.maxHeight);
    DEB_RETURN() << DEB_VAR1(size);
}


//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorType(string& type)
{
    DEB_MEMBER_FUNCT();
    type = m_camera_info.vendorName;;
    DEB_RETURN() << DEB_VAR1(type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getDetectorModel(string& model)
{
    DEB_MEMBER_FUNCT();
    model = m_camera_info.modelName;
    DEB_RETURN() << DEB_VAR1(model);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getTrigMode(TrigMode& mode)
{
    DEB_MEMBER_FUNCT();

    cout << "getTrigMode" << endl;

    // Get current trigger settings
    FlyCapture2::TriggerMode triggerMode;
    m_error = m_camera->GetTriggerMode( &triggerMode );
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to get trigger mode settings" << m_error.GetDescription();

    mode = triggerMode.onOff ? ExtTrigSingle : IntTrig;

    DEB_RETURN() << DEB_VAR1(mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setTrigMode(TrigMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);

    // Check for external trigger support
    FlyCapture2::TriggerModeInfo triggerModeInfo;
    m_error = m_camera->GetTriggerModeInfo( &triggerModeInfo );
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to get trigger mode info from camera" << m_error.GetDescription();

    if ( not triggerModeInfo.present)
    {
    	DEB_ERROR() << "Camera does not support external trigger";
        return;
    }

    // Get current trigger settings
    FlyCapture2::TriggerMode triggerMode;
    m_error = m_camera->GetTriggerMode( &triggerMode );
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to get trigger mode settings" << m_error.GetDescription();

    switch(mode)
    {
    case IntTrig:
    	triggerMode.onOff = false;
    	break;
    case ExtTrigSingle:
    	triggerMode.onOff = true;
    	triggerMode.mode = 0;
    	triggerMode.source = 0;
    	break;
    default:
    	THROW_HW_ERROR(Error) << "Trigger mode " << mode << " is not supported";
    }

    m_error = m_camera->SetTriggerMode( &triggerMode );
    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to set trigger mode settings" << m_error.GetDescription();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExpTime(double& exp_time)
{
    DEB_MEMBER_FUNCT();

    if (m_exp_time_property.autoManualMode)
        // value might have changed
        _getProperty(&m_exp_time_property);

    exp_time = 1.0E-3 * m_exp_time_property.absValue;
    DEB_RETURN() << DEB_VAR1(exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setExpTime(double exp_time)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(exp_time);

    m_exp_time_property.autoManualMode = false;
    m_exp_time_property.absValue = 1000 * exp_time;

    _setProperty(&m_exp_time_property);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getAutoExpTime(bool &auto_exp_time) const
{
    DEB_MEMBER_FUNCT();

    auto_exp_time = m_exp_time_property.autoManualMode;

    DEB_RETURN() << DEB_VAR1(auto_exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setAutoExpTime(bool auto_exp_time)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(auto_exp_time);

    if (!m_exp_time_property_info.autoSupported )
    {
        DEB_WARNING() << "Auto exposure is not supported";
	return;
    }

    m_exp_time_property.autoManualMode = auto_exp_time;

    // do a "one push" adjustment, if supported
    if (m_exp_time_property_info.onePushSupported)
    	m_exp_time_property.onePush = auto_exp_time;

    _setProperty(&m_exp_time_property);

    m_exp_time_property.onePush = false;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTime(double& lat_time)
{
    // latency is not managed
    DEB_MEMBER_FUNCT();
    lat_time = 0;
    DEB_RETURN() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setLatTime(double lat_time)
{
    // latency is not managed
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(lat_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getExpTimeRange(double& min_exp_time, double& max_exp_time) const
{
    DEB_MEMBER_FUNCT();
    min_exp_time = 1E-3 * m_exp_time_property_info.absMin;
    max_exp_time = 1E-3 * m_exp_time_property_info.absMax;
    DEB_RETURN() << DEB_VAR2(min_exp_time, max_exp_time);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getLatTimeRange(double& min_lat_time, double& max_lat_time) const
{
    DEB_MEMBER_FUNCT();
    min_lat_time = 0;
    max_lat_time = 0;
    DEB_RETURN() << DEB_VAR2(min_lat_time, max_lat_time);
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
void Camera::setNbFrames(int nb_frames)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(nb_frames);
    m_nb_frames = nb_frames;
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
    DEB_RETURN() << DEB_VAR1(status);
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
void Camera::getRoi(Roi& hw_roi)
{
    DEB_MEMBER_FUNCT();
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
void Camera::checkBin(Bin &aBin)
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
void Camera::setBin(const Bin &aBin)
{
    DEB_MEMBER_FUNCT();
    DEB_RETURN() << DEB_VAR1(aBin);
}

//---------------------------
//
//---------------------------
void Camera::reset(void)
{
    DEB_MEMBER_FUNCT();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getVideoMode(VideoMode& mode) const
{
    DEB_MEMBER_FUNCT();
    switch(m_fmt7_image_settings.pixelFormat)
    {
    case FlyCapture2::PIXEL_FORMAT_MONO8:
    	mode = Y8;
    	break;
    case FlyCapture2::PIXEL_FORMAT_MONO16:
    	mode = Y16;
    	break;
    default:
    	THROW_HW_ERROR(Error) << "Unable to determine the video mode";
    }
    DEB_RETURN() << DEB_VAR1(mode);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setVideoMode(VideoMode mode)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(mode);

    FlyCapture2::Format7ImageSettings fmt7_image_settings(m_fmt7_image_settings);
    FlyCapture2::Format7PacketInfo fmt7_packet_info;
    ImageType image_type;
    bool valid;

    switch(mode)
    {
    case Y8:
    	fmt7_image_settings.pixelFormat = FlyCapture2::PIXEL_FORMAT_MONO8;
    	image_type = Bpp8;
    	break;
    case Y16:
    	fmt7_image_settings.pixelFormat = FlyCapture2::PIXEL_FORMAT_MONO16;
    	image_type = Bpp16;
    	break;
    default:
    	THROW_HW_ERROR(Error) << "Unsupported video mode";
    }

    if (fmt7_image_settings.pixelFormat == m_fmt7_image_settings.pixelFormat)
    	// no changes
    	return;

    bool pause_acq = m_started;
    if (pause_acq)
    	stopAcq();

    m_error = m_camera->ValidateFormat7Settings(&fmt7_image_settings,
						&valid,
						&fmt7_packet_info);

    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to validate image format settings:" << m_error.GetDescription();

    if (!valid)
    	THROW_HW_ERROR(Error) << "Unsupported image format settings";

    m_error = m_camera->SetFormat7Configuration(&fmt7_image_settings,
						fmt7_packet_info.recommendedBytesPerPacket);

    if (m_error != FlyCapture2::PGRERROR_OK)
    	THROW_HW_ERROR(Error) << "Unable to apply image format settings:" << m_error.GetDescription();

    if (pause_acq)
    	startAcq();

    m_fmt7_image_settings = fmt7_image_settings;

    maxImageSizeChanged(Size(m_fmt7_info.maxWidth, m_fmt7_info.maxHeight), image_type);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getGain(double& gain)
{
    DEB_MEMBER_FUNCT();

    if (m_gain_property.autoManualMode)
        // value might have changed
        _getProperty(&m_gain_property);

    gain = m_gain_property.absValue / m_gain_property_info.absMax;

    DEB_RETURN() << DEB_VAR1(gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setGain(double gain)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(gain);

    m_gain_property.autoManualMode = false;
    m_gain_property.absValue = gain * m_gain_property_info.absMax;

    _setProperty(&m_gain_property);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::getAutoGain(bool& auto_gain) const
{
    DEB_MEMBER_FUNCT();
    auto_gain = m_gain_property.autoManualMode;
    DEB_RETURN() << DEB_VAR1(auto_gain);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Camera::setAutoGain(bool auto_gain)
{
    DEB_MEMBER_FUNCT();
    DEB_PARAM() << DEB_VAR1(auto_gain);

    if (!m_gain_property_info.autoSupported)
    {
    	DEB_WARNING() << "Auto gain is not supported";
    	return;
    }

    m_gain_property.autoManualMode = auto_gain;

    // do a "one push" adjustment, if supported
    if (m_gain_property_info.onePushSupported)
        m_gain_property.onePush = auto_gain;

    _setProperty(&m_gain_property);

    m_gain_property.onePush = false;
}

void Camera::_getPropertyInfo(FlyCapture2::PropertyInfo *property_info)
{
    DEB_MEMBER_FUNCT();
    m_error = m_camera->GetPropertyInfo(property_info);
    if (m_error != FlyCapture2::PGRERROR_OK)
        THROW_HW_ERROR(Error) << "Failed to get camera property info: " << m_error.GetDescription();
}

void Camera::_getProperty(FlyCapture2::Property *property)
{
    DEB_MEMBER_FUNCT();
    m_error = m_camera->GetProperty(property);
    if (m_error != FlyCapture2::PGRERROR_OK)
        THROW_HW_ERROR(Error) << "Failed to get camera property: " << m_error.GetDescription();
}

void Camera::_setProperty(FlyCapture2::Property *property)
{
    DEB_MEMBER_FUNCT();
    m_error = m_camera->SetProperty(property);
    if (m_error != FlyCapture2::PGRERROR_OK)
        THROW_HW_ERROR(Error) << "Failed to set camera property: " << m_error.GetDescription();
}

//---------------------------
//
//---------------------------
void Camera::_newFrameCBK(FlyCapture2::Image* image, const void *data)
{
    DEB_STATIC_FUNCT();
    Camera *camera = (Camera*)data;
    camera->_newFrame(image);
}

//---------------------------
//
//---------------------------
void Camera::_newFrame(FlyCapture2::Image* image)
{
    DEB_MEMBER_FUNCT();
    if(!m_continue_acq) return;

    // TODO: do we need mutex protection? probably.
    unsigned int rows, cols, stride;
    FlyCapture2::PixelFormat pixel_format;
    FlyCapture2::BayerTileFormat bayer_format;

    image->GetDimensions(&rows, &cols, &stride, &pixel_format, &bayer_format);

    VideoMode mode;
    switch(pixel_format)
    {
    case FlyCapture2::PIXEL_FORMAT_MONO8:
        mode = Y8;
	break;
    case FlyCapture2::PIXEL_FORMAT_MONO16:
        mode = Y16;
	break;
    default:
        DEB_ERROR() << "Format not supported: " << DEB_VAR1(pixel_format);
	stopAcq();
	return;
    }

    m_continue_acq =  m_video->callNewImage((char *)image->GetData(), cols, rows, mode);
    m_image_number++;

    if (m_nb_frames && m_image_number == m_nb_frames)
        stopAcq();
}
