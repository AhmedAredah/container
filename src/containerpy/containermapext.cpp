#include "containermapext.h"

ContainerMapExt::ContainerMapExt(QObject *parent)
    : ContainerCore::ContainerMap{parent}
{}

ContainerMapExt::ContainerMapExt(const std::string &dbLocation)
    : ContainerMap(QString::fromStdString(dbLocation))
{}

void ContainerMapExt::addContainer(const std::string &id, ContainerExt *container)
{
    ContainerCore::ContainerMap::addContainer(QString::fromStdString(id), container);
}

void ContainerMapExt::addContainers(const std::vector<ContainerExt *> &containers)
{
    for (auto& c : containers) {
        addContainer(c->getContainerID().toStdString(), c);
    }
}

ContainerExt *ContainerMapExt::getContainer(const std::string &id)
{
    return static_cast<ContainerExt *>(
        ContainerCore::ContainerMap::getContainer(QString::fromStdString(id)));
}

void ContainerMapExt::removeContainer(const std::string &id)
{
    ContainerCore::ContainerMap::removeContainer(QString::fromStdString(id));
}

std::map<std::string, ContainerExt *> ContainerMapExt::containers() const
{
    std::map<std::string, ContainerExt*> stdMap;
    QMap<QString, ContainerCore::Container*> qtMap =
        ContainerCore::ContainerMap::containers();  // Call the original function
    for (auto it = qtMap.begin(); it != qtMap.end(); ++it) {
        // Convert QString to std::string
        std::string key = it.key().toStdString();

        // Safely attempt to cast to ContainerExt*
        ContainerExt* value = dynamic_cast<ContainerExt*>(it.value());
        if (value) {
            stdMap[key] = value;  // Only add to map if cast is successful
        }
    }
    return stdMap;
}

std::vector<ContainerExt *> ContainerMapExt::
    getContainersByNextDestination(const std::string &destination)
{
    auto results =
        ContainerCore::ContainerMap::
        getContainersByNextDestination(QString::fromStdString(destination));

    std::vector<ContainerExt *> output;
    for (auto& r : results) {
        ContainerExt* value = static_cast<ContainerExt*>(r);
        output.push_back(value);
    }

    return output;
}


std::vector<ContainerExt *> ContainerMapExt::
    dequeueContainerByNextDestination(const std::string &destination)
{
    auto results =
        ContainerCore::ContainerMap::
        dequeueContainersByNextDestination(QString::fromStdString(destination));

    std::vector<ContainerExt *> output;
    for (auto& r : results) {
        ContainerExt* value = static_cast<ContainerExt*>(r);
        output.push_back(value);
    }

    return output;
}
