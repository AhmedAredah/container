#include <QtTest>
#include "containerLib/container.h"
#include "containerLib/containermap.h"
#include "containerLib/package.h"

using namespace ContainerCore;

// Test class for Container and Package
class TestContainer : public QObject {
    Q_OBJECT

private slots:
    // Initialization and cleanup
    void initTestCase();
    void cleanupTestCase();

    // Package tests
    void testPackageInitialization();
    void testPackageID();
    void testPackageJsonSerialization();

    // Container tests
    void testContainerInitialization();
    void testCustomVariables();
    void testCurrentLocation();
    void testNextDestinations();
    void testMovementHistory();
    void testJsonSerialization();

    // ContainerMap tests
    void testContainerMapOperations();
    void testContainerMapJsonSerialization();
};

void TestContainer::initTestCase() {
    qDebug() << "Initializing test environment...";
}

void TestContainer::cleanupTestCase() {
    qDebug() << "Cleaning up test environment...";
}

// Test Package initialization
void TestContainer::testPackageInitialization() {
    Package package1("PKG001");
    QCOMPARE(package1.packageID(), QString("PKG001"));

    QJsonObject json;
    json["packageID"] = "PKG002";
    Package package2(json);
    QCOMPARE(package2.packageID(), QString("PKG002"));
}

// Test Package ID setting and getting
void TestContainer::testPackageID() {
    Package package("PKG001");
    package.setPackageID("PKG_NEW");
    QCOMPARE(package.packageID(), QString("PKG_NEW"));
}

// Test Package JSON serialization
void TestContainer::testPackageJsonSerialization() {
    Package package("PKG001");
    QJsonObject json = package.toJson();
    QCOMPARE(json["packageID"].toString(), QString("PKG001"));

    Package newPackage(json);
    QCOMPARE(newPackage.packageID(), QString("PKG001"));
}

// Test Container initialization
void TestContainer::testContainerInitialization() {
    Container container("TEST001", Container::twentyFT);
    QCOMPARE(container.getContainerID(), QString("TEST001"));
    QCOMPARE(container.getContainerSize(), Container::twentyFT);
}

// Test custom variables in a Container
void TestContainer::testCustomVariables() {
    Container container("TEST001", Container::twentyFT);

    container.addCustomVariable(ContainerCore::Container::HaulerType::truck,
                                "weight", 500);
    QVariant value = container.getCustomVariable(
        ContainerCore::Container::HaulerType::truck, "weight");
    QCOMPARE(value.toInt(), 500);

    container.removeCustomVariable(
        ContainerCore::Container::HaulerType::truck, "weight");
    QVERIFY(!container.getCustomVariablesForHauler(
                          ContainerCore::Container::HaulerType::truck)
                 .contains("weight"));
}

// Test current location in a Container
void TestContainer::testCurrentLocation()
{
    Container container("TEST001", Container::twentyFT);

    // Set the current location
    container.setContainerCurrentLocation("Warehouse A");

    // Now check that it was set correctly
    QCOMPARE(container.getContainerCurrentLocation(), QString("Warehouse A"));
}

// Test next destinations in a Container
void TestContainer::testNextDestinations() {
    Container container("TEST001", Container::twentyFT);

    container.addDestination("Port B");
    container.addDestination("Port C");

    QCOMPARE(container.getContainerNextDestinations().size(), 2);

    container.removeDestination("Port B");
    QCOMPARE(container.getContainerNextDestinations().size(), 1);
}

// Test movement history in a Container
void TestContainer::testMovementHistory() {
    Container container("TEST001", Container::twentyFT);

    container.addMovementHistory("Moved to Warehouse A");
    QCOMPARE(container.getContainerMovementHistory().size(), 1);

    container.removeMovementHistory("Moved to Warehouse A");
    QCOMPARE(container.getContainerMovementHistory().size(), 0);
}

// Test JSON serialization in a Container
void TestContainer::testJsonSerialization() {
    Container container("TEST001", Container::twentyFT);
    container.setContainerCurrentLocation("Warehouse A");
    container.addDestination("Port B");

    QJsonObject json = container.toJson();
    QCOMPARE(json["containerID"].toString(), QString("TEST001"));
    QCOMPARE(json["containerCurrentLocation"].toString(),
             QString("Warehouse A"));

    QJsonArray destinations = json["containerNextDestinations"].toArray();
    QCOMPARE(destinations.size(), 1);
    QCOMPARE(destinations[0].toString(), QString("Port B"));
}

// Test ContainerMap operations
void TestContainer::testContainerMapOperations() {
    ContainerMap map;
    Container* container = new Container("TEST001", Container::twentyFT);
    map.addContainer(container->getContainerID(), container);
    QCOMPARE(map.size(), 1);
    Container* retrievedContainer = map.getContainerByID("TEST001");
    QVERIFY(retrievedContainer != nullptr);
    QCOMPARE(retrievedContainer->getContainerID(), QString("TEST001"));
    map.removeContainerByID("TEST001");
    QCOMPARE(map.size(), 0);
}

// Test ContainerMap JSON serialization
void TestContainer::testContainerMapJsonSerialization() {
    ContainerMap map;

    Container* container = new Container("TEST001", Container::twentyFT);
    map.addContainer(container->getContainerID(), container);

    QJsonObject json = map.toJson();
    QJsonArray containers = json["containers"].toArray();
    QCOMPARE(containers.size(), 1);

    QVector<Container *> loadedContainers =
        ContainerMap::loadContainersFromJson(json);
    QCOMPARE(loadedContainers.size(), 1);
    QCOMPARE(loadedContainers[0]->getContainerID(), QString("TEST001"));
}

// Main function to run tests
QTEST_MAIN(TestContainer)
#include "test_container.moc"
