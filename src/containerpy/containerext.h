#ifndef CONTAINEREXT_H
#define CONTAINEREXT_H

#include <container.h>
#include "packageext.h"

class ContainerExt : public ContainerCore::Container
{
public:
    explicit ContainerExt(QObject *parent = nullptr);
    ContainerExt(const std::string &id, ContainerSize size);

    std::string getContainerIDStd() const;
    void setContainerIDStd(const std::string &id);

    // Getter and Setter for containerSize
    ContainerSize getContainerSize() const;
    void setContainerSize(ContainerSize size);

    // Getter and Setter for packages
    std::vector<PackageExt*> getPackages() const;
    void setPackages(const std::vector<PackageExt*> &packages);
    void addPackage(PackageExt *package);

    void addCustomVariable(HaulerType hauler, const std::string &key, const std::string &value);
    void addCustomVariable(HaulerType hauler, const std::string &key, int value);
    void addCustomVariable(HaulerType hauler, const std::string &key, double value);
    void removeCustomVariable(HaulerType hauler, const std::string &key);
    std::string getCustomVariable(HaulerType hauler, const std::string &key) const;

    std::string getContainerCurrentLocationStd() const;
    void setContainerCurrentLocationStd(const std::string &location);

    std::vector<std::string> getContainerNextDestinationsStd() const;
    void setContainerNextDestinationsStd(const std::vector<std::string> &destinations);
    void addDestinationStd(const std::string &destination);
    bool removeDestinationStd(const std::string &destination);

    std::vector<std::string> getContainerMovementHistoryStd() const;
    void setContainerMovementHistoryStd(const std::vector<std::string> &history);
    void addMovementHistoryStd(const std::string &history);
    bool removeMovementHistoryStd(const std::string &history);
};

#endif // CONTAINEREXT_H
