#include "containermapext.h"

ContainerMapExt::ContainerMapExt()
    : mContainerMap{nullptr}
{
        mContainerMap.setIsRunningThroughPython(true);
}

ContainerMapExt::ContainerMapExt(const std::string &dbLocation)
    : mContainerMap(QString::fromStdString(dbLocation))
{
    mContainerMap.setIsRunningThroughPython(true);
}

ContainerMapExt::ContainerMapExt(const QJsonObject &json)
    : mContainerMap(json, nullptr)
{
        mContainerMap.setIsRunningThroughPython(true);
}

void ContainerMapExt::addContainer(ContainerExt *container, double addingTime)
{
    if (container) {
        mContainerMap.addContainer(QString::fromStdString(container->getContainerID()), container->getBaseContainer(), addingTime);
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

void ContainerMapExt::addContainers(const QJsonObject &json, double addingTime)
{
    mContainerMap.addContainers(json, addingTime);
}

std::vector<ContainerExt*> ContainerMapExt::getContainersByAddedTime(double referenceTime, const std::string &condition) {
    auto results = mContainerMap.getContainersByAddedTime(referenceTime, QString::fromStdString(condition));

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::vector<ContainerExt *> ContainerMapExt::dequeueContainersByAddedTime(double referenceTime, const std::string &condition)
{
    auto results = mContainerMap.dequeueContainersByAddedTime(referenceTime, QString::fromStdString(condition));

    return convertQVecContainerToSTDVecContainerExt(results);
}

ContainerExt* ContainerMapExt::getContainerByID(const std::string &id)
{
    ContainerCore::Container* baseContainer = mContainerMap.getContainerByID(QString::fromStdString(id));
    return toContainerExt(baseContainer);
}

void ContainerMapExt::removeContainerByID(const std::string &id)
{
    mContainerMap.removeContainerByID(QString::fromStdString(id));
}

std::vector<ContainerExt *> ContainerMapExt::getAllContainers()
{
    QMap<QString, ContainerCore::Container*> qtMap = mContainerMap.getAllContainers();
    auto qtMapVec = qtMap.values();
    return convertQVecContainerToSTDVecContainerExt(qtMapVec);
}

std::vector<ContainerExt *> ContainerMapExt::getLatestContainers() {
    QMap<QString, ContainerCore::Container*> qtMap = mContainerMap.getLatestContainers();
    auto qtMapVec = qtMap.values();
    return convertQVecContainerToSTDVecContainerExt(qtMapVec);
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

QJsonObject ContainerMapExt::toJson() const
{
    return mContainerMap.toJson();
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

std::map<std::string, ContainerExt *> ContainerMapExt::convertQMapToSTDMapContainerExt(QMap<QString, ContainerCore::Container*> original) {
    std::map<std::string, ContainerExt*> stdMap;

    for (auto it = original.begin(); it != original.end(); ++it) {
        std::string key = it.key().toStdString();
        ContainerExt* value = toContainerExt(it.value());

        if (value) {
            stdMap[key] = value;
        }
    }
    return stdMap;
}
