
set(LIBNAME CBPM-TSHARC)
cmake_minimum_required(VERSION $ENV{ACC_CMAKE_VERSION})

set(INC_DIRS 
  ${GTK2_INCLUDE_DIRS}
  ../include
  ../c_utils/include
  ../mpmnet/include
  ../cbi_net/include
  ../cbpmfio/include
  ../BeamInstSupport/hardware_include
  ../BeamInstSupport/include
  ../BeamInstSupport/comm_include
  ../BeamInstSupport/cesr_intf_include
  include
  comm_include
)


set(SRC_DIRS
  actions
  comm_code
  commstruct_output
  data_publishing
  file_io
  shared
  system
  type_convert
  user_io
  utils
)

include($ENV{ACC_BUILD_SYSTEM}/Master.cmake)
