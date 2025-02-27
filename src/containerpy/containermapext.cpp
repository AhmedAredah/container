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

void ContainerMapExt::addContainer(ContainerExt *container, double addingTime, double leavingTime)
{
    if (container) {
        mContainerMap.addContainer(QString::fromStdString(container->getContainerID()), container->getBaseContainer(), addingTime, leavingTime);
    }
}

void ContainerMapExt::addContainers(const std::vector<ContainerExt *> &containers, double addingTime, double leavingTime)
{
    for (auto& c : containers) {
        if (c) {
            mContainerMap.addContainer(QString::fromStdString(c->getContainerID()), c->getBaseContainer(), addingTime, leavingTime);
        }
    }
}

void ContainerMapExt::addContainers(const QJsonObject &json, double addingTime, double leavingTime)
{
    mContainerMap.addContainers(json, addingTime, leavingTime);
}

std::vector<ContainerExt*> ContainerMapExt::getContainersByAddedTime(const std::string &condition, double referenceTime) {
    auto results = mContainerMap.getContainersByAddedTime(QString::fromStdString(condition), referenceTime);

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::vector<ContainerExt *> ContainerMapExt::dequeueContainersByAddedTime(const std::string &condition, double referenceTime)
{
    auto results = mContainerMap.dequeueContainersByAddedTime(QString::fromStdString(condition), referenceTime);

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::size_t ContainerMapExt::countContainersByAddedTime(const std::string &condition, double referenceTime) {
    return mContainerMap.countContainersByAddedTime(QString::fromStdString(condition), referenceTime);
}

std::vector<ContainerExt *> ContainerMapExt::getContainersByLeavingTime(const std::string &condition, double referenceTime)
{
    auto results = mContainerMap.getContainersByLeavingTime(QString::fromStdString(condition), referenceTime);

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::vector<ContainerExt *> ContainerMapExt::dequeueContainersByLeavingTime(const std::string &condition, double referenceTime)
{
    auto results = mContainerMap.dequeueContainersByLeavingTime(QString::fromStdString(condition), referenceTime);

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::size_t ContainerMapExt::countContainersByLeavingTime(const std::string &condition, double referenceTime) {
    return mContainerMap.countContainersByLeavingTime(QString::fromStdString(condition), referenceTime);
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

std::size_t ContainerMapExt::size() const
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

std::vector<ContainerExt *> ContainerMapExt::loadContainersFromJson(const QJsonObject &json)
{
    auto results = ContainerCore::ContainerMap::loadContainersFromJson(json);

    return convertQVecContainerToSTDVecContainerExt(results);
}

std::size_t ContainerMapExt::countContainersByNextDestination(const std::string &destination) {
    return mContainerMap.countContainersByNextDestination(QString::fromStdString(destination));
}

QJsonObject ContainerMapExt::toJson() const
{
    return mContainerMap.toJson();
}

// Helper method to safely cast ContainerCore::Container* to ContainerExt*
ContainerExt* ContainerMapExt::toContainerExt(ContainerCore::Container* base)
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

void ContainerMapExt::clear() {
    mContainerMap.clear();
}
