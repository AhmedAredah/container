#include "packageext.h"

PackageExt::PackageExt() {}

PackageExt::PackageExt(const std::string id)
    : ContainerCore::Package(QString::fromStdString(id), nullptr)
{}

void PackageExt::setPackageID(const std::string &id) {
    ContainerCore::Package::setPackageID
        (QString::fromStdString(id));
}

std::string PackageExt::packageID() const {
    return ContainerCore::Package::packageID().toStdString();
}
