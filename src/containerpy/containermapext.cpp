#include "containermapext.h"

ContainerMapExt::ContainerMapExt()
    : mContainerMap{nullptr}
{}

ContainerMapExt::ContainerMapExt(const std::string &dbLocation)
    : mContainerMap(QString::fromStdString(dbLocation))
{}

void ContainerMapExt::addContainer(const std::string &id, ContainerExt *container, double addingTime)
{
    if (container) {
        mContainerMap.addContainer(QString::fromStdString(id), container->getBaseContainer(), addingTime);
    }
}

void ContainerMapExt::addContainers(const std::vector<ContainerExt *> &containers, double addingTime)
{
    for (auto& c : containers) {
        if (c) {
            mContainerMap.addContainer(QString::fromStdString(c->getContainerID()), c->getBaseContainer(), addingTime);
        }
    }
}

std::vector<ContainerExt*> ContainerMapExt::getContainersByAddedTime(double referenceTime, const std::string &condition) {
    auto results = mContainerMap.getContainersByAddedTime(referenceTime, QString::fromStdString(condition));

    return convertQVecContainerToSTDVecContainerExt(results);
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

    return convertQVecContainerToSTDVecContainerExt(results);
}


std::vector<ContainerExt*> ContainerMapExt::dequeueContainerByNextDestination(const std::string &destination)
{
    auto results = mContainerMap.dequeueContainersByNextDestination(QString::fromStdString(destination));

    return convertQVecContainerToSTDVecContainerExt(results);
}

// Helper method to safely cast ContainerCore::Container* to ContainerExt*
ContainerExt* ContainerMapExt::toContainerExt(ContainerCore::Container* base) const
{
    return new ContainerExt(base);
}

std::vector<ContainerExt*> ContainerMapExt::convertQVecContainerToSTDVecContainerExt(QVector<ContainerCore::Container*> original) {
    std::vector<ContainerExt*> output;

    for (auto& r : original) {
        ContainerExt* value = toContainerExt(r);
        if (value) {
            output.push_back(value);
        }
    }

    return output;
}
