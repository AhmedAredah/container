#ifndef CONTAINERMAPEXT_H
#define CONTAINERMAPEXT_H

#include "containerext.h"
#include "containermap.h"

class ContainerMapExt
{
public:
    explicit ContainerMapExt();

    ContainerMapExt(const std::string &dbLocation);
    ContainerMapExt(const QJsonObject &json);

    void addContainer(ContainerExt* container, double addingTime);

    void addContainers(const std::vector<ContainerExt*> &containers, double addingTime);
    void addContainers(const QJsonObject &json, double addingTime);

    ContainerExt* getContainer(const std::string &id);

    void removeContainer(const std::string &id);

    std::vector<ContainerExt *> getAllContainers();

    std::vector<ContainerExt *> getLatestContainers();

    int size() const;

    std::vector<ContainerExt*> getContainersByAddedTime(double referenceTime, const std::string &condition);

    std::vector<ContainerExt*> dequeueContainersByAddedTime(double referenceTime, const std::string &condition);

    std::vector<ContainerExt*> getContainersByNextDestination(const std::string &destination);

    std::vector<ContainerExt*> dequeueContainerByNextDestination(const std::string &destination);

    QJsonObject toJson() const;

private:
    ContainerCore::ContainerMap mContainerMap;

    // Helper method to safely cast ContainerCore::Container* to ContainerExt*
    ContainerExt* toContainerExt(ContainerCore::Container* base) const;

    std::vector<ContainerExt*> convertQVecContainerToSTDVecContainerExt(QVector<ContainerCore::Container*> original);
    std::map<std::string, ContainerExt *> convertQMapToSTDMapContainerExt(QMap<QString, ContainerCore::Container*> original);
};

#endif // CONTAINERMAPEXT_H
