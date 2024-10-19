#ifndef CONTAINEREXT_H
#define CONTAINEREXT_H

#include <container.h>
#include "packageext.h"

class ContainerExt
{
public:
    enum ContainerSize {
        twentyFT,
        twentyFT_HighCube,
        fourtyFT,
        fourtyFT_HighCube,
        fortyFiveFT,
        fortyFiveFT_HighCube,
        tenFT,
        thirtyFT,
        fortyEightFT,
        fiftyThreeFT,
        sixtyFT
    };

    enum HaulerType {
        truck,
        train,
        waterTransport,
        airTransport
    };

    // Constructor and Destructor
    ContainerExt(const std::string &id, ContainerSize size);
    ~ContainerExt();

    // Copy Constructor and Assignment Operator
    ContainerExt(const ContainerExt &other);
    ContainerExt& operator=(const ContainerExt &other);

    std::string getContainerID() const;
    void setContainerIDStd(const std::string &id);

    ContainerSize getContainerSize() const;
    void setContainerSize(ContainerSize size);

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

    ContainerCore::Container *getBaseContainer();

private:
    ContainerCore::Container *mContainer;

    // Helper method to clean up mContainer
    void cleanup();
};

#endif // CONTAINEREXT_H
