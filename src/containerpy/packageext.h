#ifndef PACKAGEEXT_H
#define PACKAGEEXT_H

#include "package.h"

class PackageExt
{
public:
    // Constructor that initializes mPackage with a new Package instance
    PackageExt(const std::string &id);

    // Constructor that creates a copy of the given Package
    PackageExt(const ContainerCore::Package &pkg);

    PackageExt(ContainerCore::Package *pkg);

    // Copy constructor
    PackageExt(const PackageExt &other);

    // Assignment operator
    PackageExt& operator=(const PackageExt &other);

    // Destructor
    ~PackageExt();

    // Setter for Package ID
    void setPackageID(const std::string &id);

    // Getter for Package ID
    std::string packageID() const;

    // Returns a pointer to the base Package
    ContainerCore::Package* getBasePackage();

private:
    ContainerCore::Package *mPackage;

    // Helper function to clean up mPackage
    void cleanup();
};

#endif // PACKAGEEXT_H
