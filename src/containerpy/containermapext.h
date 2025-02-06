#ifndef CONTAINERMAPEXT_H
#define CONTAINERMAPEXT_H

#include "containerext.h"
#include "containerLib/containermap.h"

class ContainerMapExt
{
public:
    explicit ContainerMapExt();

    ContainerMapExt(const std::string &dbLocation);
    ContainerMapExt(const QJsonObject &json);

    void addContainer(ContainerExt* container, double addingTime, double leavingTime);

    void addContainers(const std::vector<ContainerExt*> &containers, double addingTime, double leavingTime);
    void addContainers(const QJsonObject &json, double addingTime, double leavingTime);

    ContainerExt* getContainerByID(const std::string &id);

    void removeContainerByID(const std::string &id);

    std::vector<ContainerExt *> getAllContainers();

    std::vector<ContainerExt *> getLatestContainers();

    std::size_t size() const;

    std::vector<ContainerExt*> getContainersByAddedTime(const std::string &condition, double referenceTime);

    std::vector<ContainerExt*> dequeueContainersByAddedTime(const std::string &condition, double referenceTime);

    std::size_t countContainersByAddedTime(const std::string &condition, double referenceTime);

    std::vector<ContainerExt*> getContainersByLeavingTime(const std::string &condition, double referenceTime);

    std::vector<ContainerExt*> dequeueContainersByLeavingTime(const std::string &condition, double referenceTime);

    std::size_t countContainersByLeavingTime(const std::string &condition, double referenceTime);

    std::vector<ContainerExt*> getContainersByNextDestination(const std::string &destination);

    std::vector<ContainerExt*> dequeueContainerByNextDestination(const std::string &destination);

    static std::vector<ContainerExt*> loadContainersFromJson(const QJsonObject &json);

    std::size_t countContainersByNextDestination(const std::string &destination);

    QJsonObject toJson() const;

private:
    ContainerCore::ContainerMap mContainerMap;

    // Helper method to safely cast ContainerCore::Container* to ContainerExt*
    static ContainerExt* toContainerExt(ContainerCore::Container* base);

    static std::vector<ContainerExt*> convertQVecContainerToSTDVecContainerExt(QVector<ContainerCore::Container*> original);
    static std::map<std::string, ContainerExt *> convertQMapToSTDMapContainerExt(QMap<QString, ContainerCore::Container*> original);
};

#endif // CONTAINERMAPEXT_H
