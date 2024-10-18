#ifndef CONTAINERMAPEXT_H
#define CONTAINERMAPEXT_H

#include "containerext.h"
#include "containermap.h"

class ContainerMapExt : public ContainerCore::ContainerMap
{
public:
    explicit ContainerMapExt(QObject *parent = nullptr);

    ContainerMapExt(const std::string &dbLocation);

    void addContainer(const std::string &id, ContainerExt* container);

    void addContainers(const std::vector<ContainerExt*> &containers);

    ContainerExt* getContainer(const std::string &id);

    void removeContainer(const std::string &id);

    std::map<std::string, ContainerExt*> containers() const;

    std::vector<ContainerExt*> getContainersByNextDestination(const std::string &destination);

    std::vector<ContainerExt*> dequeueContainerByNextDestination(const std::string &destination);


};

#endif // CONTAINERMAPEXT_H
