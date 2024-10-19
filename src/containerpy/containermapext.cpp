#include "containermapext.h"

ContainerMapExt::ContainerMapExt(QObject *parent)
    : mContainerMap{parent}
{}

ContainerMapExt::ContainerMapExt(const std::string &dbLocation)
    : mContainerMap(QString::fromStdString(dbLocation))
{}

void ContainerMapExt::addContainer(const std::string &id, ContainerExt *container)
{
    if (container) {
        mContainerMap.addContainer(QString::fromStdString(id), container->getBaseContainer());
    }
}

void ContainerMapExt::addContainers(const std::vector<ContainerExt *> &containers)
{
    for (auto& c : containers) {
        if (c) {
            mContainerMap.addContainer(QString::fromStdString(c->getContainerID()), c->getBaseContainer());
        }
    }
}

ContainerExt* ContainerMapExt::getContainer(const std::string &id)
{
    ContainerCore::Container* baseContainer = mContainerMap.getContainer(QString::fromStdString(id));
    return toContainerExt(baseContainer);
}

void ContainerMapExt::removeContainer(const std::string &id)
{
    mContainerMap.removeContainer(QString::fromStdString(id));
}

std::map<std::string, ContainerExt *> ContainerMapExt::containers() const
{
    std::map<std::string, ContainerExt*> stdMap;
    QMap<QString, ContainerCore::Container*> qtMap = mContainerMap.containers();

    for (auto it = qtMap.begin(); it != qtMap.end(); ++it) {
        std::string key = it.key().toStdString();
        ContainerExt* value = toContainerExt(it.value());

        if (value) {
            stdMap[key] = value;
        }
    }
    return stdMap;
}

int ContainerMapExt::size() const
{
    return mContainerMap.size();
}

std::vector<ContainerExt*> ContainerMapExt::getContainersByNextDestination(const std::string &destination)
{
    auto results = mContainerMap.getContainersByNextDestination(QString::fromStdString(destination));
    std::vector<ContainerExt*> output;

    for (auto& r : results) {
        ContainerExt* value = toContainerExt(r);
        if (value) {
            output.push_back(value);
        }
    }

    return output;
}


std::vector<ContainerExt*> ContainerMapExt::dequeueContainerByNextDestination(const std::string &destination)
{
    auto results = mContainerMap.dequeueContainersByNextDestination(QString::fromStdString(destination));
    std::vector<ContainerExt*> output;

    for (auto& r : results) {
        ContainerExt* value = toContainerExt(r);
        if (value) {
            output.push_back(value);
        }
    }

    return output;
}

// Helper method to safely cast ContainerCore::Container* to ContainerExt*
ContainerExt* ContainerMapExt::toContainerExt(ContainerCore::Container* base) const
{
    // Use dynamic_cast for safety in case of polymorphism
    return dynamic_cast<ContainerExt*>(base);
}
