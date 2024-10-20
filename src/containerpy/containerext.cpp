#include "containerext.h"
#include "container.h"
#include "package.h"
#include "packageext.h"

ContainerExt::ContainerExt(const std::string &id, ContainerSize size)
{
    mContainer = new ContainerCore::Container(QString::fromStdString(id), static_cast<ContainerCore::Container::ContainerSize>(size));
}

ContainerExt::ContainerExt(const QJsonObject &json)
{
    mContainer = new ContainerCore::Container(json, nullptr);
}

ContainerExt::~ContainerExt() {
    cleanup();
}

ContainerExt::ContainerExt(const ContainerExt &other)
{
    mContainer = new ContainerCore::Container(*other.mContainer);
}

ContainerExt::ContainerExt(ContainerCore::Container *other)
{
    mContainer = other;
}

ContainerExt& ContainerExt::operator=(const ContainerExt &other) {
    if (this != &other) {
        cleanup();
        mContainer = new ContainerCore::Container(*other.mContainer);
    }
    return *this;
}

void ContainerExt::cleanup() {
    if (mContainer) {
        delete mContainer;
        mContainer = nullptr;
    }
}

std::string ContainerExt::getContainerID() const {
    return mContainer ? mContainer->getContainerID().toStdString() : "";
}

void ContainerExt::setContainerID(const std::string &id) {
    if (mContainer) {
        mContainer->setContainerID(QString::fromStdString(id));
    }
}

double ContainerExt::getContainerAddedTime() const
{
    if (mContainer) {
        return mContainer->getContainerAddedTime();
    }

    return std::nan("containerNotDefined");
}

void ContainerExt::setContainerAddedTime(double &time)
{
    if (mContainer) {
        mContainer->setContainerAddedTime(time);
    }
}

ContainerExt::ContainerSize ContainerExt::getContainerSize() const
{
    return static_cast<ContainerExt::ContainerSize>(mContainer->getContainerSize());
}

void ContainerExt::setContainerSize(ContainerSize size)
{
    mContainer->setContainerSize(static_cast<ContainerCore::Container::ContainerSize>(size));
}

std::vector<PackageExt*> ContainerExt::getPackages() const {
    std::vector<PackageExt*> stdPackages;
    if (mContainer) {
        QVector<ContainerCore::Package*> qtPackages = mContainer->getPackages();
        for (ContainerCore::Package* package : qtPackages) {
            PackageExt* ext = new PackageExt(*package);
            if (ext) {
                stdPackages.push_back(ext);
            }
        }
    }
    return stdPackages;
}

void ContainerExt::setPackages(const std::vector<PackageExt*>& stdPackages) {
    if (mContainer) {
        QVector<ContainerCore::Package*> qtPackages;
        for (PackageExt* ext : stdPackages) {
            qtPackages.append(ext->getBasePackage());
        }
        mContainer->setPackages(qtPackages);
    }
}

void ContainerExt::addPackage(PackageExt* package) {
    if (mContainer && package) {
        mContainer->addPackage(package->getBasePackage());
    }
}

void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  const std::string &value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = QString::fromStdString(value);
    mContainer->addCustomVariable(static_cast<ContainerCore::Container::HaulerType>(hauler), qKey, qValue);
}


void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  int value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = value;  // Automatically constructs a QVariant from int
    mContainer->addCustomVariable(static_cast<ContainerCore::Container::HaulerType>(hauler), qKey, qValue);
}

void ContainerExt::addCustomVariable(HaulerType hauler, const std::string &key,
                                  double value) {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = value;  // Automatically constructs a QVariant from double
    mContainer->addCustomVariable(static_cast<ContainerCore::Container::HaulerType>(hauler), qKey, qValue);
}

// Method to remove a custom variable using std::string
void ContainerExt::removeCustomVariable(HaulerType hauler,
                                     const std::string &key) {
    QString qKey = QString::fromStdString(key);
    mContainer->removeCustomVariable(static_cast<ContainerCore::Container::HaulerType>(hauler), qKey);
}

// Method to get a custom variable's value using std::string
std::string ContainerExt::getCustomVariable(HaulerType hauler,
                                         const std::string &key) const {
    QString qKey = QString::fromStdString(key);
    QVariant qValue = mContainer->getCustomVariable(static_cast<ContainerCore::Container::HaulerType>(hauler), qKey);

    if (qValue.typeId() == QMetaType::QString) {
        return qValue.toString().toStdString();
    } else if (qValue.typeId() == QMetaType::Int) {
        return std::to_string(qValue.toInt());
    } else if (qValue.typeId() == QMetaType::Double) {
        return std::to_string(qValue.toDouble());
    }
    return "Unsupported type";
}

std::string ContainerExt::getContainerCurrentLocation() const
{
    return mContainer->getContainerCurrentLocation().toStdString();
}

void ContainerExt::setContainerCurrentLocation(const std::string &location) {
    mContainer->setContainerCurrentLocation(QString::fromStdString(location));
}

std::vector<std::string> ContainerExt::getContainerNextDestinations() const
{
    std::vector<std::string> results;
    for (auto &e : mContainer->getContainerNextDestinations()) {
        results.push_back(e.toStdString());
    }
    return results;
}

void ContainerExt::setContainerNextDestinations(
    const std::vector<std::string> &destinations)
{
    QVector<QString> values;
    for (auto& e : destinations) {
        values.push_back(QString::fromStdString(e));
    }
    mContainer->setContainerNextDestinations(values);
}

void ContainerExt::addDestination(const std::string &destination) {
    mContainer->addDestination(QString::fromStdString(destination));
}

bool ContainerExt::removeDestination(const std::string &destination) {
    return mContainer->removeDestination(QString::fromStdString(destination));
}

std::vector<std::string> ContainerExt::getContainerMovementHistory() const {
    std::vector<std::string> results;
    for (auto &e : mContainer->getContainerMovementHistory()) {
        results.push_back(e.toStdString());
    }
    return results;
}

void ContainerExt::setContainerMovementHistory(
    const std::vector<std::string> &history) {

    QVector<QString> values;
    for (auto& e : history) {
        values.push_back(QString::fromStdString(e));
    }
    mContainer->setContainerMovementHistory(values);
}
void ContainerExt::addMovementHistory(const std::string &history) {
    mContainer->addMovementHistory(QString::fromStdString(history));
}

bool ContainerExt::removeMovementHistory(const std::string &history) {
    return mContainer->removeMovementHistory(QString::fromStdString(history));
}

QJsonObject ContainerExt::toJson() const
{
    return mContainer->toJson();
}

ContainerCore::Container *ContainerExt::getBaseContainer()
{
    return mContainer;
}
