// Generated by gencpp from file CameraVideoTopic/NavSatFix.msg
// DO NOT EDIT!


#ifndef CAMERAVIDEOTOPIC_MESSAGE_NAVSATFIX_H
#define CAMERAVIDEOTOPIC_MESSAGE_NAVSATFIX_H


#include <string>
#include <vector>
#include <map>

#include <ros/types.h>
#include <ros/serialization.h>
#include <ros/builtin_message_traits.h>
#include <ros/message_operations.h>


namespace CameraVideoTopic
{
template <class ContainerAllocator>
struct NavSatFix_
{
  typedef NavSatFix_<ContainerAllocator> Type;

  NavSatFix_()
    : latitude(0.0)
    , longitude(0.0)
    , altitude(0.0)
    , position_covariance()
    , position_covariance_type(0)  {
      position_covariance.assign(0.0);
  }
  NavSatFix_(const ContainerAllocator& _alloc)
    : latitude(0.0)
    , longitude(0.0)
    , altitude(0.0)
    , position_covariance()
    , position_covariance_type(0)  {
  (void)_alloc;
      position_covariance.assign(0.0);
  }



   typedef double _latitude_type;
  _latitude_type latitude;

   typedef double _longitude_type;
  _longitude_type longitude;

   typedef double _altitude_type;
  _altitude_type altitude;

   typedef boost::array<double, 9>  _position_covariance_type;
  _position_covariance_type position_covariance;

   typedef uint8_t _position_covariance_type_type;
  _position_covariance_type_type position_covariance_type;





  typedef boost::shared_ptr< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> const> ConstPtr;

}; // struct NavSatFix_

typedef ::CameraVideoTopic::NavSatFix_<std::allocator<void> > NavSatFix;

typedef boost::shared_ptr< ::CameraVideoTopic::NavSatFix > NavSatFixPtr;
typedef boost::shared_ptr< ::CameraVideoTopic::NavSatFix const> NavSatFixConstPtr;

// constants requiring out of line definition



template<typename ContainerAllocator>
std::ostream& operator<<(std::ostream& s, const ::CameraVideoTopic::NavSatFix_<ContainerAllocator> & v)
{
ros::message_operations::Printer< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >::stream(s, "", v);
return s;
}


template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator==(const ::CameraVideoTopic::NavSatFix_<ContainerAllocator1> & lhs, const ::CameraVideoTopic::NavSatFix_<ContainerAllocator2> & rhs)
{
  return lhs.latitude == rhs.latitude &&
    lhs.longitude == rhs.longitude &&
    lhs.altitude == rhs.altitude &&
    lhs.position_covariance == rhs.position_covariance &&
    lhs.position_covariance_type == rhs.position_covariance_type;
}

template<typename ContainerAllocator1, typename ContainerAllocator2>
bool operator!=(const ::CameraVideoTopic::NavSatFix_<ContainerAllocator1> & lhs, const ::CameraVideoTopic::NavSatFix_<ContainerAllocator2> & rhs)
{
  return !(lhs == rhs);
}


} // namespace CameraVideoTopic

namespace ros
{
namespace message_traits
{





template <class ContainerAllocator>
struct IsMessage< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsMessage< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
  : TrueType
  { };

template <class ContainerAllocator>
struct IsFixedSize< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> const>
  : TrueType
  { };

template <class ContainerAllocator>
struct HasHeader< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
  : FalseType
  { };

template <class ContainerAllocator>
struct HasHeader< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> const>
  : FalseType
  { };


template<class ContainerAllocator>
struct MD5Sum< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
{
  static const char* value()
  {
    return "d34c1978afe13c3c14c4416e63843b4f";
  }

  static const char* value(const ::CameraVideoTopic::NavSatFix_<ContainerAllocator>&) { return value(); }
  static const uint64_t static_value1 = 0xd34c1978afe13c3cULL;
  static const uint64_t static_value2 = 0x14c4416e63843b4fULL;
};

template<class ContainerAllocator>
struct DataType< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
{
  static const char* value()
  {
    return "CameraVideoTopic/NavSatFix";
  }

  static const char* value(const ::CameraVideoTopic::NavSatFix_<ContainerAllocator>&) { return value(); }
};

template<class ContainerAllocator>
struct Definition< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
{
  static const char* value()
  {
    return "float64 latitude\n"
"float64 longitude\n"
"float64 altitude\n"
"float64[9] position_covariance\n"
"uint8 position_covariance_type\n"
;
  }

  static const char* value(const ::CameraVideoTopic::NavSatFix_<ContainerAllocator>&) { return value(); }
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

  template<class ContainerAllocator> struct Serializer< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
  {
    template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
    {
      stream.next(m.latitude);
      stream.next(m.longitude);
      stream.next(m.altitude);
      stream.next(m.position_covariance);
      stream.next(m.position_covariance_type);
    }

    ROS_DECLARE_ALLINONE_SERIALIZER
  }; // struct NavSatFix_

} // namespace serialization
} // namespace ros

namespace ros
{
namespace message_operations
{

template<class ContainerAllocator>
struct Printer< ::CameraVideoTopic::NavSatFix_<ContainerAllocator> >
{
  template<typename Stream> static void stream(Stream& s, const std::string& indent, const ::CameraVideoTopic::NavSatFix_<ContainerAllocator>& v)
  {
    s << indent << "latitude: ";
    Printer<double>::stream(s, indent + "  ", v.latitude);
    s << indent << "longitude: ";
    Printer<double>::stream(s, indent + "  ", v.longitude);
    s << indent << "altitude: ";
    Printer<double>::stream(s, indent + "  ", v.altitude);
    s << indent << "position_covariance[]" << std::endl;
    for (size_t i = 0; i < v.position_covariance.size(); ++i)
    {
      s << indent << "  position_covariance[" << i << "]: ";
      Printer<double>::stream(s, indent + "  ", v.position_covariance[i]);
    }
    s << indent << "position_covariance_type: ";
    Printer<uint8_t>::stream(s, indent + "  ", v.position_covariance_type);
  }
};

} // namespace message_operations
} // namespace ros

#endif // CAMERAVIDEOTOPIC_MESSAGE_NAVSATFIX_H
