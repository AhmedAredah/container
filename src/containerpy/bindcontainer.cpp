#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "containerext.h"
#include "containermapext.h"
#include "packageext.h"

namespace py = pybind11;
using namespace ContainerCore;

PYBIND11_MODULE(container_module, m) {
    m.doc() = "Pybind11 plugin for Container library";

    py::class_<PackageExt>(m, "Package")
        .def(py::init<const std::string &>(), "Constructor taking a std::string")  // Explicit std::string constructor
        .def("get_package_id", &PackageExt::packageID, "Get the package ID as std::string")
        .def("set_package_id", &PackageExt::setPackageID, "Set the package ID using std::string")
        .def("to_json", &Package::toJson, "Convert to JSON");

    py::class_<ContainerExt>(m, "Container")
        .def(py::init<const std::string &, Container::ContainerSize>(),
             py::arg("id"),
             py::arg("size") = nullptr,
             "Constructor that initializes a Container with a specified size.")
        .def("get_container_id", &ContainerExt::getContainerID)
        .def("set_container_id", &ContainerExt::setContainerID)
        .def("add_package", &ContainerExt::addPackage)
        .def("add_custom_variable", static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, const std::string&)>(&ContainerExt::addCustomVariable), py::arg("hauler"), py::arg("key"), py::arg("value"))
        .def("add_custom_variable", static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, int)>(&ContainerExt::addCustomVariable), py::arg("hauler"), py::arg("key"), py::arg("value"))
        .def("add_custom_variable", static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, double)>(&ContainerExt::addCustomVariable), py::arg("hauler"), py::arg("key"), py::arg("value"))
        .def("removeCustomVariable", static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&)>(&ContainerExt::removeCustomVariable), py::arg("hauler"), py::arg("key"))
        .def("get_custom_variable", static_cast<std::string (ContainerExt::*)(ContainerExt::HaulerType, const std::string&) const>(&ContainerExt::getCustomVariable), py::arg("hauler"), py::arg("key"))
        .def("set_container_current_location", &ContainerExt::setContainerCurrentLocation)
        .def("get_container_current_location", &ContainerExt::getContainerCurrentLocation)
        .def("set_container_next_destinations", &ContainerExt::setContainerNextDestinations)
        .def("get_container_next_destinations", &ContainerExt::getContainerNextDestinations)
        .def("set_container_movement_history", &ContainerExt::setContainerMovementHistory)
        .def("get_container_movement_history", &ContainerExt::getContainerMovementHistory);

    // Binding the ContainerSize enum
    py::enum_<ContainerExt::ContainerSize>(m, "ContainerSize")
        .value("TwentyFT", ContainerExt::ContainerSize::twentyFT)
        .value("TwentyFT_HighCube", ContainerExt::ContainerSize::twentyFT_HighCube)
        .value("FourtyFT", ContainerExt::ContainerSize::fourtyFT)
        .value("FourtyFT_HighCube", ContainerExt::ContainerSize::fourtyFT_HighCube)
        .value("FortyFiveFT", ContainerExt::ContainerSize::fortyFiveFT)
        .value("FortyFiveFT_HighCube", ContainerExt::ContainerSize::fortyFiveFT_HighCube)
        .value("TenFT", ContainerExt::ContainerSize::tenFT)
        .value("ThirtyFT", ContainerExt::ContainerSize::thirtyFT)
        .value("FortyEightFT", ContainerExt::ContainerSize::fortyEightFT)
        .value("FiftyThreeFT", ContainerExt::ContainerSize::fiftyThreeFT)
        .value("SixtyFT", ContainerExt::ContainerSize::sixtyFT)
        .export_values();

    // Binding the HaulerType enum
    py::enum_<ContainerExt::HaulerType>(m, "HaulerType")
        .value("Truck", ContainerExt::HaulerType::truck)
        .value("Train", ContainerExt::HaulerType::train)
        .value("WaterTransport", ContainerExt::HaulerType::waterTransport)
        .value("AirTransport", ContainerExt::HaulerType::airTransport)
        .export_values();

    py::class_<ContainerMapExt, QObject>(m, "ContainerMap")
        .def(py::init<const std::string &>())
        .def("add_container", &ContainerMapExt::addContainer)
        .def("get_container", &ContainerMapExt::getContainer)
        .def("remove_container", &ContainerMapExt::removeContainer)
        .def("to_json", &ContainerMapExt::toJson)
        .def("containers", &ContainerMapExt::containers)
        .def("size", &ContainerMapExt::size)
        .def("get_containers_by_next_destination", &ContainerMapExt::getContainersByNextDestination)
        .def("dequeue_containers_by_next_destination", &ContainerMapExt::dequeueContainerByNextDestination);


}
