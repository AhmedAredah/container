#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "containerext.h"
#include "containermapext.h"
#include "packageext.h"
#include <iostream>

namespace py = pybind11;

PYBIND11_MODULE(ContainerPy, m) {
    m.doc() = "Pybind11 plugin for Container library";

    std::cout << "ContainerPy module has been successfully imported!" << std::endl;


    py::class_<PackageExt>(m, "Package")
        .def(py::init<const std::string &>(), py::arg("id"),
             "Constructor that initializes a Package with the specified ID.")
        .def("get_package_id", &PackageExt::packageID,
             "Get the package ID as std::string.")
        .def("set_package_id", &PackageExt::setPackageID, py::arg("id"),
             "Set the package ID using std::string.");

    py::class_<ContainerExt>(m, "Container")
        .def(py::init<const std::string &, ContainerExt::ContainerSize>(),
             py::arg("id"),
             py::arg("size"),
             "Constructor that initializes a Container with a specified size.")
        .def("get_container_id", &ContainerExt::getContainerID)
        .def("set_container_id", &ContainerExt::setContainerID, py::arg("id"))
        .def("get_container_size", &ContainerExt::getContainerSize)
        .def("set_container_size", &ContainerExt::setContainerSize, py::arg("size"))
        .def("add_package", &ContainerExt::addPackage, py::arg("package"),
             "Add a package to the container.")
        .def("add_custom_variable",
             static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, const std::string&)>(&ContainerExt::addCustomVariable),
             py::arg("hauler"), py::arg("key"), py::arg("value"),
             "Add a custom variable with string value.")
        .def("add_custom_variable",
             static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, int)>(&ContainerExt::addCustomVariable),
             py::arg("hauler"), py::arg("key"), py::arg("value"),
             "Add a custom variable with integer value.")
        .def("add_custom_variable",
             static_cast<void (ContainerExt::*)(ContainerExt::HaulerType, const std::string&, double)>(&ContainerExt::addCustomVariable),
             py::arg("hauler"), py::arg("key"), py::arg("value"),
             "Add a custom variable with double value.")
        .def("remove_custom_variable",
             &ContainerExt::removeCustomVariable,
             py::arg("hauler"), py::arg("key"),
             "Remove a custom variable by hauler and key.")
        .def("get_custom_variable",
             &ContainerExt::getCustomVariable,
             py::arg("hauler"), py::arg("key"),
             "Get a custom variable value as a string.")
        .def("set_container_current_location",
             &ContainerExt::setContainerCurrentLocation,
             py::arg("location"),
             "Set the container's current location.")
        .def("get_container_current_location",
             &ContainerExt::getContainerCurrentLocation,
             "Get the container's current location as a string.")
        .def("set_container_next_destinations",
             &ContainerExt::setContainerNextDestinations,
             py::arg("destinations"),
             "Set the container's next destinations.")
        .def("get_container_next_destinations",
             &ContainerExt::getContainerNextDestinations,
             "Get the container's next destinations as a list of strings.")
        .def("set_container_movement_history",
             &ContainerExt::setContainerMovementHistory,
             py::arg("history"),
             "Set the container's movement history.")
        .def("get_container_movement_history",
             &ContainerExt::getContainerMovementHistory,
             "Get the container's movement history as a list of strings.");


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

    py::class_<ContainerMapExt>(m, "ContainerMap")
        .def(py::init<>())
        .def(py::init<const std::string &>())
        .def("add_container",
            [](ContainerMapExt &self, const std::string &id, ContainerExt* container, double addingTime) {
                // Check if addingTime is NaN and pass it to the C++ function accordingly
                if (std::isnan(addingTime)) {
                    self.addContainer(id, container, std::nan(""));
                } else {
                    self.addContainer(id, container, addingTime);
                }
            }, py::arg("id"), py::arg("container"), py::arg("addingTime") = std::nan(""))
        .def("add_containers",
            [](ContainerMapExt &self, const std::vector<ContainerExt*> &containers, double addingTime) {
                // Check if addingTime is NaN and pass it to the C++ function accordingly
                if (std::isnan(addingTime)) {
                    self.addContainers(containers, std::nan(""));
                } else {
                    self.addContainers(containers, addingTime);
                }
            }, py::arg("containers"), py::arg("addingTime") = std::nan(""))
        .def("get_containers_by_added_time",
             &ContainerMapExt::getContainersByAddedTime,
             py::arg("referenceTime"), py::arg("condition"))
        .def("remove_container", &ContainerMapExt::removeContainer)
        .def("get_containers", &ContainerMapExt::containers)
        .def("size", &ContainerMapExt::size)
        .def("get_containers_by_next_destination", &ContainerMapExt::getContainersByNextDestination)
        .def("dequeue_containers_by_next_destination", &ContainerMapExt::dequeueContainerByNextDestination);


}
