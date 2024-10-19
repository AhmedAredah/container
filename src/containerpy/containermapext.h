#ifndef CONTAINERMAPEXT_H
#define CONTAINERMAPEXT_H

#include "containerext.h"
#include "containermap.h"

class ContainerMapExt
{
public:
    explicit ContainerMapExt();

    ContainerMapExt(const std::string &dbLocation);

    void addContainer(const std::string &id, ContainerExt* container, double addingTime);

    void addContainers(const std::vector<ContainerExt*> &containers, double addingTime);

    std::vector<ContainerExt*> getContainersByAddedTime(double referenceTime, const std::string &condition);

    ContainerExt* getContainer(const std::string &id);

    void removeContainer(const std::string &id);

    std::map<std::string, ContainerExt*> containers() const;

    int size() const;

    std::vector<ContainerExt*> getContainersByNextDestination(const std::string &destination);

    std::vector<ContainerExt*> dequeueContainerByNextDestination(const std::string &destination);

private:
    ContainerCore::ContainerMap mContainerMap;

    // Helper method to safely cast ContainerCore::Container* to ContainerExt*
    ContainerExt* toContainerExt(ContainerCore::Container* base) const;

    std::vector<ContainerExt*> convertQVecContainerToSTDVecContainerExt(QVector<ContainerCore::Container*> original);
};

#endif // CONTAINERMAPEXT_H
