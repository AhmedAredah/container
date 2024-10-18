#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "container.h"
#include "containermap.h"
#include "package.h"

namespace py = pybind11;
using namespace ContainerCore;

PYBIND11_MODULE(container_module, m) {
    m.doc() = "Pybind11 plugin for Container library";

    py::class_<Package>(m, "Package")
        .def(py::init<>())
        .def("get_package_id", &ContainerCore::Package::packageID)
        .def("set_package_id", &ContainerCore::Package::setPackageID);

    py::class_<ContainerCore::Container, QObject>(m, "Container")
        .def(py::init<>())
        .def("get_container_id", &ContainerCore::Container::getContainerID)
        .def("set_container_id", &ContainerCore::Container::setContainerID)
        .def("add_package", &ContainerCore::Container::addPackage)
        .def("to_json", &ContainerCore::Container::toJson)
        .def("set_container_current_location", &ContainerCore::Container::setContainerCurrentLocation)
        .def("get_container_current_location", &ContainerCore::Container::getContainerCurrentLocation)
        .def("set_container_next_destinations", &ContainerCore::Container::setContainerNextDestinations)
        .def("get_container_next_destinations", &ContainerCore::Container::getContainerNextDestinations)
        .def("set_container_movement_history", &ContainerCore::Container::setContainerMovementHistory)
        .def("get_container_movement_history", &ContainerCore::Container::getContainerMovementHistory);


    py::class_<ContainerCore::ContainerMap, QObject>(m, "ContainerMap")
        .def(py::init<>())
        .def("add_container", &ContainerCore::ContainerMap::addContainer)
        .def("get_container", &ContainerCore::ContainerMap::getContainer)
        .def("remove_container", &ContainerCore::ContainerMap::removeContainer)
        .def("to_json", &ContainerCore::ContainerMap::toJson)
        .def("containers", &ContainerCore::ContainerMap::containers)
        .def("size", &ContainerCore::ContainerMap::size)
        .def("get_containers_by_next_destination", &ContainerCore::ContainerMap::getContainersByNextDestination)
        .def("dequeue_containers_by_next_destination", &ContainerMap::dequeueContainersByNextDestination);


}
