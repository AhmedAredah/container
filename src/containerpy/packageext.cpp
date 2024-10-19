#include "packageext.h"

// Constructor that initializes mPackage with a new Package instance
PackageExt::PackageExt(const std::string &id)
    : mPackage(new ContainerCore::Package(QString::fromStdString(id)))
{}

// Constructor that creates a copy of the given Package
PackageExt::PackageExt(const ContainerCore::Package &pkg)
    : mPackage(new ContainerCore::Package(pkg))
{}

PackageExt::PackageExt(ContainerCore::Package *pkg)
{
    mPackage = pkg;
}

// Copy constructor to handle deep copy
PackageExt::PackageExt(const PackageExt &other)
    : mPackage(new ContainerCore::Package(*other.mPackage))
{}

// Assignment operator for deep copy
PackageExt& PackageExt::operator=(const PackageExt &other) {
    if (this != &other) {
        cleanup();  // Clean up existing resource
        mPackage = new ContainerCore::Package(*other.mPackage);
    }
    return *this;
}

// Destructor to delete dynamically allocated memory safely
PackageExt::~PackageExt() {
    cleanup();
}

// Cleanup function to safely delete mPackage
void PackageExt::cleanup() {
    if (mPackage) {
        delete mPackage;
        mPackage = nullptr;
    }
}

void PackageExt::setPackageID(const std::string &id) {
    if (mPackage) {
        mPackage->setPackageID(QString::fromStdString(id));
    }
}

std::string PackageExt::packageID() const {
    return mPackage ? mPackage->packageID().toStdString() : "";
}

ContainerCore::Package* PackageExt::getBasePackage() {
    return mPackage;
}
