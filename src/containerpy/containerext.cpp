#include "containerext.h"
#include "package.h"
#include "packageext.h"
ContainerExt::ContainerExt(QObject *parent)
    : ContainerCore::Container{parent}
{}

ContainerExt::ContainerExt(const std::string &id, ContainerSize size)
    : Container(QString::fromStdString(id), size) {}

std::string ContainerExt::getContainerIDStd() const
{
    return Container::getContainerID().toStdString();
}

void ContainerExt::setContainerIDStd(const std::string &id)
{
    Container::setContainerID(QString::fromStdString(id));
}

std::vector<PackageExt *> ContainerExt::getPackages() const
{
    QVector<ContainerCore::Package*> qtPackages = Container::getPackages();
    std::vector<PackageExt*> stdPackages;

    for (ContainerCore::Package* package : qtPackages) {
        PackageExt* ext = static_cast<PackageExt*>(package);
        if (ext) {
            stdPackages.push_back(ext);
        }
    }

    return stdPackages;
}

void ContainerExt::setPackages(const std::vector<PackageExt*>& stdPackages) {
    QVector<ContainerCore::Package*> qtPackages;
    for (PackageExt* ext : stdPackages) {
        qtPackages.append(ext); // Safe as PackageExt is derived from Package
    }
    Container::setPackages(qtPackages); // Call the base class method
}

void ContainerExt::addPackage(PackageExt* package) {
    Container::addPackage(package); // Directly pass PackageExt* to base class method
}

void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  const std::string &value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = QString::fromStdString(value);
    Container::addCustomVariable(hauler, qKey, qValue);
}


void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  int value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = value;  // Automatically constructs a QVariant from int
    Container::addCustomVariable(hauler, qKey, qValue);
}

void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  double value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = value;  // Automatically constructs a QVariant from double
    Container::addCustomVariable(hauler, qKey, qValue);
}

// Method to remove a custom variable using std::string
void ContainerExt::removeCustomVariable(HaulerType hauler,
                                     const std::string &key) {
    QString qKey = QString::fromStdString(key);
    Container::removeCustomVariable(hauler, qKey);
}

// Method to get a custom variable's value using std::string
std::string ContainerExt::getCustomVariable(HaulerType hauler,
                                         const std::string &key) const {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = Container::getCustomVariable(hauler, qKey);

    if (qValue.typeId() == QMetaType::QString) {
        return qValue.toString().toStdString();
    } else if (qValue.typeId() == QMetaType::Int) {
        return std::to_string(qValue.toInt());
    } else if (qValue.typeId() == QMetaType::Double) {
        return std::to_string(qValue.toDouble());
    }
    return "Unsupported type";
}

std::string ContainerExt::getContainerCurrentLocationStd() const
{
    return Container::getContainerCurrentLocation().toStdString();
}

void ContainerExt::setContainerCurrentLocationStd(const std::string &location) {
    Container::setContainerCurrentLocation(QString::fromStdString(location));
}

std::vector<std::string> ContainerExt::getContainerNextDestinationsStd() const
{
    std::vector<std::string> results;
    for (auto &e : Container::getContainerNextDestinations()) {
        results.push_back(e.toStdString());
    }
    return results;
}

void ContainerExt::setContainerNextDestinationsStd(
    const std::vector<std::string> &destinations)
{
    QVector<QString> values;
    for (auto& e : destinations) {
        values.push_back(QString::fromStdString(e));
    }
    Container::setContainerNextDestinations(values);
}

void ContainerExt::addDestinationStd(const std::string &destination) {
    Container::addDestination(QString::fromStdString(destination));
}

bool ContainerExt::removeDestinationStd(const std::string &destination) {
    return Container::removeDestination(QString::fromStdString(destination));
}

std::vector<std::string> ContainerExt::getContainerMovementHistoryStd() const {
    std::vector<std::string> results;
    for (auto &e : Container::getContainerMovementHistory()) {
        results.push_back(e.toStdString());
    }
    return results;
}

void ContainerExt::setContainerMovementHistoryStd(
    const std::vector<std::string> &history) {

    QVector<QString> values;
    for (auto& e : history) {
        values.push_back(QString::fromStdString(e));
    }
    Container::setContainerMovementHistory(values);
}
void ContainerExt::addMovementHistoryStd(const std::string &history) {
    Container::addMovementHistory(QString::fromStdString(history));
}

bool ContainerExt::removeMovementHistoryStd(const std::string &history) {
    return Container::removeMovementHistory(QString::fromStdString(history));
}
