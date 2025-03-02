#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "containerext.h"
#include "containermapext.h"
#include "packageext.h"
#include <iostream>

namespace py = pybind11;

py::object QJsonValueToPyObject(const QJsonValue& value);

py::dict QJsonObjectToPyDict(const QJsonObject& jsonObj) {
    py::dict pyDict;
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        const QString& key = it.key();
        const QJsonValue& value = it.value();
        pyDict[py::str(key.toStdString())] = QJsonValueToPyObject(value); // Convert key to py::str
    }
    return pyDict;
}

py::list QJsonArrayToPyList(const QJsonArray& jsonArray) {
    py::list pyList;
    for (const auto& value : jsonArray) {
        pyList.append(QJsonValueToPyObject(value));
    }
    return pyList;
}

py::object QJsonValueToPyObject(const QJsonValue& value) {
    switch (value.type()) {
    case QJsonValue::Bool:
        return py::bool_(value.toBool());
    case QJsonValue::Double:
        return py::float_(value.toDouble());
    case QJsonValue::String:
        return py::str(value.toString().toStdString());
    case QJsonValue::Array:
        return QJsonArrayToPyList(value.toArray());
    case QJsonValue::Object:
        return QJsonObjectToPyDict(value.toObject());
    case QJsonValue::Null:
    default:
        return py::none();
    }
}

py::dict PackageExtToPyDict(const PackageExt& pkg) {
    QJsonObject json = pkg.toJson();  // Assuming this returns QJsonObject
    return QJsonObjectToPyDict(json);
}

py::dict ContainerExtToPyDict(const ContainerExt& container) {
    QJsonObject json = container.toJson();  // Assuming this returns QJsonObject
    return QJsonObjectToPyDict(json);
}

py::dict ContainerMapExtToPyDict(const ContainerMapExt& map) {
    QJsonObject json = map.toJson();  // Assuming this returns QJsonObject
    return QJsonObjectToPyDict(json);
}

// Helper function to convert Python dict to QJsonObject
QJsonObject PyDictToQJsonObject(const py::dict &pyDict) {
    QJsonObject jsonObj;
    for (auto item : pyDict) {
        QString key = QString::fromStdString(py::str(item.first).cast<std::string>());
        py::object value = py::reinterpret_borrow<py::object>(item.second);

        // Handle primitive types and nested objects properly
        if (py::isinstance<py::bool_>(value)) {
            jsonObj[key] = value.cast<bool>();
        } else if (py::isinstance<py::int_>(value)) {
            jsonObj[key] = value.cast<int>();
        } else if (py::isinstance<py::float_>(value)) {
            jsonObj[key] = value.cast<double>();
        } else if (py::isinstance<py::str>(value)) {
            jsonObj[key] = QString::fromStdString(value.cast<std::string>());
        } else if (py::isinstance<py::list>(value)) {
            QJsonArray jsonArray;
            for (py::handle item : value) {
                py::object item_obj = py::reinterpret_borrow<py::object>(item);
                if (py::isinstance<py::str>(item_obj)) {
                    jsonArray.append(QString::fromStdString(item_obj.cast<std::string>()));
                } else if (py::isinstance<py::int_>(item_obj)) {
                    jsonArray.append(item_obj.cast<int>());
                } else if (py::isinstance<py::float_>(item_obj)) {
                    jsonArray.append(item_obj.cast<double>());
                } else if (py::isinstance<py::bool_>(item_obj)) {
                    jsonArray.append(item_obj.cast<bool>());
                } else if (py::isinstance<py::dict>(item_obj)) {
                    // If the list contains a dictionary, convert it to a QJsonObject
                    jsonArray.append(PyDictToQJsonObject(item_obj.cast<py::dict>()));
                } else {
                    qWarning() << "Unsupported data type in list for key:" << key;
                }
            }
            jsonObj[key] = jsonArray;
        } else if (py::isinstance<py::dict>(value)) {
            jsonObj[key] = PyDictToQJsonObject(value.cast<py::dict>());
        } else {
            qWarning() << "Unsupported data type for key:" << key;
        }
    }
    return jsonObj;
}




PYBIND11_MODULE(ContainerPy, m) {
    m.doc() = "Pybind11 plugin for Container library";

    std::cout << "ContainerPy is developed by Ahmed Aredah!\nAll rights reserved!" << std::endl;


    py::class_<PackageExt>(m, "Package")
        .def(py::init<const std::string &>(), py::arg("id"),
             "Constructor that initializes a Package with the specified ID.")
        .def(py::init([](const py::dict &pyDict) {
                 return PackageExt(PyDictToQJsonObject(pyDict));
             }), py::arg("json_dict"),
             "Constructor that initializes a Package from a Python dictionary.")
        .def("get_package_id", &PackageExt::packageID,
             "Get the package ID as std::string.")
        .def("set_package_id", &PackageExt::setPackageID, py::arg("id"),
             "Set the package ID using std::string.")
        .def("to_json", [](PackageExt &self) {
                return PackageExtToPyDict(self);
            }, "Extract package information to a Python dictionary")
        .def("copy", &PackageExt::copy, py::return_value_policy::reference);

    py::class_<ContainerExt>(m, "Container")
        .def(py::init<const std::string &, ContainerExt::ContainerSize>(),
             py::arg("id"),
             py::arg("size"),
             "Constructor that initializes a Container with a specified size.")
        .def(py::init([](const py::dict &pyDict) {
                 return ContainerExt(PyDictToQJsonObject(pyDict));
             }), py::arg("json_dict"),
             "Constructor that initializes a Package from a Python dictionary.")
        .def("get_container_id", &ContainerExt::getContainerID)
        .def("set_container_id", &ContainerExt::setContainerID, py::arg("id"))
        .def("get_container_size", &ContainerExt::getContainerSize)
        .def("set_container_size", &ContainerExt::setContainerSize, py::arg("size"))
        .def("add_package", &ContainerExt::addPackage, py::arg("package"),
             "Add a package to the container.")
        .def("get_packages", &ContainerExt::getPackages, py::return_value_policy::reference)
        .def("get_container_added_time", &ContainerExt::getContainerAddedTime, py::return_value_policy::reference)
        .def("set_container_added_time", &ContainerExt::setContainerAddedTime)
        .def("get_container_leaving_time", &ContainerExt::getContainerLeavingTime, py::return_value_policy::reference)
        .def("set_container_leaving_time", &ContainerExt::setContainerLeavingTime)
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
             "Get the container's movement history as a list of strings.")
        .def("add_container_destination", &ContainerExt::addDestination,
            "Add a destination to a container's destinations list")
        .def("remove_container_destination", &ContainerExt::removeDestination,
            "Remove a destination from a container's destination list")
        .def("to_json", [](ContainerExt &self) {
                return ContainerExtToPyDict(self);
            }, "Extract Container information to a Python dictionary")
        .def("copy", &ContainerExt::copy, py::return_value_policy::reference);


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
        .value("NoHauler", ContainerExt::HaulerType::noHauler)
        .export_values();

    py::class_<ContainerMapExt>(m, "ContainerMap")
        .def(py::init<>())
        .def(py::init<const std::string &>())
        .def(py::init([](const py::dict &pyDict) {
                 return ContainerMapExt(PyDictToQJsonObject(pyDict));
             }), py::arg("json_dict"),
             "Constructor that initializes a Package from a Python dictionary.")
        .def("add_container",
            [](ContainerMapExt &self, ContainerExt* container, double addingTime, double leavingTime) {
                // Check if addingTime is NaN and pass it to the C++ function accordingly
                double mAT = 0;
                double mLT = 0;
                if (std::isnan(addingTime)) {
                    mAT = std::nan("");
                } else {
                    mAT = addingTime;
                }

                if (std::isnan(leavingTime)) {
                    mLT = std::nan("");
                } else {
                    mLT = leavingTime;
                }

                self.addContainer(container, mAT, mLT);
            }, py::arg("container"), py::arg("addingTime") = std::nan(""), py::arg("leavingTime") = std::nan(""))
        .def("add_containers",
            [](ContainerMapExt &self, const std::vector<ContainerExt*> &containers, double addingTime, double leavingTime) {
                // Check if addingTime is NaN and pass it to the C++ function accordingly
                double mAT = 0;
                double mLT = 0;
                if (std::isnan(addingTime)) {
                    mAT = std::nan("");
                } else {
                    mAT = addingTime;
                }

                if (std::isnan(leavingTime)) {
                    mLT = std::nan("");
                } else {
                    mLT = leavingTime;
                }
                self.addContainers(containers, mAT, mLT);
            }, py::arg("containers"), py::arg("addingTime") = std::nan(""), py::arg("leavingTime") = std::nan(""))
        .def("add_containers_from_dict",
            [](ContainerMapExt &self, const py::dict &pyDict, double addingTime, double leavingTime) {
                // Convert the Python dictionary to a QJsonObject
                QJsonObject jsonObj = PyDictToQJsonObject(pyDict);
                double mAT = 0;
                double mLT = 0;
                if (std::isnan(addingTime)) {
                    mAT = std::nan("");
                } else {
                    mAT = addingTime;
                }

                if (std::isnan(leavingTime)) {
                    mLT = std::nan("");
                } else {
                    mLT = leavingTime;
                }
                self.addContainers(jsonObj, mAT, mLT);
            }, py::arg("json_dict"), py::arg("addingTime") = std::nan(""), py::arg("leavingTime") = std::nan(""),
            "Add multiple containers to the ContainerMap from a JSON-like Python dictionary.")
        .def("remove_container_by_id", &ContainerMapExt::removeContainerByID)
        .def("get_all_containers", &ContainerMapExt::getAllContainers, py::return_value_policy::reference)
        .def("get_container_by_id", &ContainerMapExt::getContainerByID, py::return_value_policy::reference)
        .def("get_latest_containers", &ContainerMapExt::getLatestContainers, py::return_value_policy::reference)
        .def("size", &ContainerMapExt::size, py::return_value_policy::copy)
        .def("get_containers_by_added_time",
             &ContainerMapExt::getContainersByAddedTime,
             py::arg("condition"), py::arg("referenceTime"), py::return_value_policy::reference)
        .def("dequeue_containers_by_added_time", &ContainerMapExt::dequeueContainersByAddedTime,
             py::arg("condition"), py::arg("referenceTime"), py::return_value_policy::reference)
        .def("count_containers_by_added_time", &ContainerMapExt::countContainersByAddedTime,
             py::arg("condition"), py::arg("referenceTime"))
        .def("get_containers_by_leaving_time",
             &ContainerMapExt::getContainersByLeavingTime,
             py::arg("condition"), py::arg("referenceTime"), py::return_value_policy::reference)
        .def("dequeue_containers_by_leaving_time", &ContainerMapExt::dequeueContainersByLeavingTime,
             py::arg("condition"), py::arg("referenceTime"), py::return_value_policy::reference)
        .def("count_containers_by_leaving_time", &ContainerMapExt::countContainersByLeavingTime,
             py::arg("condition"), py::arg("referenceTime"))
        .def("get_containers_by_next_destination", &ContainerMapExt::getContainersByNextDestination, py::return_value_policy::reference)
        .def("dequeue_containers_by_next_destination", &ContainerMapExt::dequeueContainerByNextDestination, py::return_value_policy::reference)
        .def("count_containers_by_next_destination", &ContainerMapExt::countContainersByNextDestination)
        .def("to_json", [](ContainerMapExt &self) {
                return ContainerMapExtToPyDict(self);
            }, "Extract ContainerMap information to a Python dictionary")
        .def("clear", &ContainerMapExt::clear)
        .def_static("load_containers_from_json",
                    [](const py::dict &pyDict) {
                        QJsonObject jsonObj = PyDictToQJsonObject(pyDict);
                        return ContainerMapExt::loadContainersFromJson(jsonObj);
                    },
                    py::arg("json_dict"),
                    py::return_value_policy::reference,
                    "Load containers from a JSON dictionary and return them as a list of Container objects"
                    );


}
