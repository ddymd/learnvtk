cmake_minimum_required(VERSION 3.30)
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
project(VTKStructuredGrid VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_definitions(/utf-8)
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(VTK REQUIRED COMPONENTS
  CommonColor
  CommonCore
  CommonDataModel
  FiltersGeometry
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2)

add_executable(${PROJECT_NAME} main.cpp
  BlankPoint.cpp
  # DistancePointToLine.cpp
  # FloatingPointExceptions.cpp
  # GaussianRandomNumber.cpp
  # PerspectiveTransform.cpp
  # ProjectPointPlane.cpp
  # RandomSequence.cpp
  # UniformRandomNumber.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES} spdlog::spdlog)

vtk_module_autoinit(TARGETS ${PROJECT_NAME} MODULES ${VTK_LIBRARIES})
