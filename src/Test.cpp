#include <sick_safetyscanners_base/SickSafetyscannersBase.h>
#include <sick_safetyscanners_base/datastructure/CommSettings.h>


class SickScanner
{
public:
    SickScanner();

    sick::datastructure::CommSettings m_communication_settings;
    sick::datastructure::Data data;
    sick::datastructure::PacketBuffer buffer;
    std::shared_ptr<sick::SickSafetyscannersBase> m_device;
    void receivedPacket(const sick::datastructure::Data& data);
    void readTypeCodeSettings();

};

SickScanner::SickScanner()
{
    std::string sensor_ip_adress = "192.168.0.10";
    m_communication_settings.setSensorIp(sensor_ip_adress);
    std::string host_ip_adress = "192.168.0.9";
    m_communication_settings.setHostIp(host_ip_adress);
    m_communication_settings.setSensorTcpPort(2122);

    m_device = std::make_shared<sick::SickSafetyscannersBase>(boost::bind(&SickScanner::receivedPacket, this, _1), &m_communication_settings);
    m_device->run();
    readTypeCodeSettings();
    m_device->changeSensorSettings(m_communication_settings);
}

void SickScanner::receivedPacket(const sick::datastructure::Data& data)
{
        if (!data.getMeasurementDataPtr()->isEmpty() && !data.getDerivedValuesPtr()->isEmpty())
        {


           uint16_t num_scan_points = data.getDerivedValuesPtr()->getNumberOfBeams();
           std::vector<sick::datastructure::ScanPoint> scan_points =
           data.getMeasurementDataPtr()->getScanPointsVector();

           std::cout << num_scan_points << std::endl;

           for (uint16_t i = 0; i < num_scan_points; ++i)
           {
                const sick::datastructure::ScanPoint scan_point = scan_points.at(i);
                // uint16_t a = scan_point.getDistance();
                // uint8_t b = scan_point.getReflectivity();
                float a,b;
                

                a = static_cast<float>(scan_point.getDistance()) *
                data.getDerivedValuesPtr()->getMultiplicationFactor() * 1e-3; // mm -> m
                b = static_cast<float>(scan_point.getReflectivity());

                std::cout << "nDist: " << a <<  ", " << "nIntensity: " << b << std::endl;
           }
        }
}

void SickScanner::readTypeCodeSettings()
{
//   ROS_INFO("Reading Type code settings");
    std::cout << "Reading Type code setting" << std::endl;
    sick::datastructure::TypeCode type_code;
    m_device->requestTypeCode(m_communication_settings, type_code);
    m_communication_settings.setEInterfaceType(type_code.getInterfaceType());
    
}
 

int main(int argc, char** argv)
{
    SickScanner sick_test;

    while(1)
    {
        sleep(0.5);
        
    }

    return 0;
}
