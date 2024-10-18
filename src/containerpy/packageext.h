#ifndef PACKAGEEXT_H
#define PACKAGEEXT_H

#include "package.h"

class PackageExt : public ContainerCore::Package
{
public:
    PackageExt();
    PackageExt(const std::string id);

    void setPackageID(const std::string &id);
    std::string packageID() const;
};

#endif // PACKAGEEXT_H
