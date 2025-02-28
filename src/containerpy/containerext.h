#ifndef CONTAINEREXT_H
#define CONTAINEREXT_H

#include "containerLib/container.h"
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
        airTransport,
        noHauler
    };

    // Constructor and Destructor
    ContainerExt(const std::string &id, ContainerSize size);
    ContainerExt(const QJsonObject &json);
    ~ContainerExt();

    // Copy Constructor and Assignment Operator
    ContainerExt(const ContainerExt &other);
    ContainerExt(ContainerCore::Container *other);
    ContainerExt& operator=(const ContainerExt &other);

    std::string getContainerID() const;
    void setContainerID(const std::string &id);

    double getContainerAddedTime() const;
    void setContainerAddedTime(double &time);

    double getContainerLeavingTime() const;
    void setContainerLeavingTime(double &time);

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

    std::string getContainerCurrentLocation() const;
    void setContainerCurrentLocation(const std::string &location);

    std::vector<std::string> getContainerNextDestinations() const;
    void setContainerNextDestinations(const std::vector<std::string> &destinations);
    void addDestination(const std::string &destination);
    bool removeDestination(const std::string &destination);

    std::vector<std::string> getContainerMovementHistory() const;
    void setContainerMovementHistory(const std::vector<std::string> &history);
    void addMovementHistory(const std::string &history);
    bool removeMovementHistory(const std::string &history);

    QJsonObject toJson() const;

    ContainerCore::Container *copy();

    ContainerCore::Container *getBaseContainer();

private:
    ContainerCore::Container *mContainer;

    // Helper method to clean up mContainer
    void cleanup();
};

#endif // CONTAINEREXT_H
