#include <QCoreApplication>
#include <QDebug>
#include "src/containermap.h"
#include "src/container.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);


    // Create an instance of ContainerMap
    ContainerCore::ContainerMap containerMap;

    // Add some containers to the map
    for (int i = 0; i < 5; ++i) {
        QString id = QString("Container%1").arg(i);
        qDebug() << "Adding container with ID:" << id; // Add this line
        ContainerCore::Container* container = new ContainerCore::Container(id, ContainerCore::Container::thirtyFT);
        qDebug() << "added containeer: " << container->getContainerID();
        containerMap.addContainer(id, container);
    }

    qDebug() << "outside" ;

    // Retrieve containers from the map and print their details
    for (int i = 0; i < 5; ++i) {
        QString id = QString("Container%1").arg(i);
        qDebug() << "Retrieving container with ID:" << id;

        ContainerCore::Container* retrievedContainer = containerMap.getContainer(id);

        if (retrievedContainer) {
            qDebug() << "Retrieved container with ID:" << retrievedContainer->getContainerID();
        } else {
            qDebug() << "Failed to retrieve container with ID:" << id;
        }
    }

    // Remove a container from the map
    containerMap.removeContainer("Container2");
    qDebug() << "Removed container with ID: Container2";

    // Attempt to retrieve the removed container
    ContainerCore::Container* removedContainer = containerMap.getContainer("Container2");
    if (removedContainer) {
        qDebug() << "Error: Retrieved a container that was supposed to be removed.";
    } else {
        qDebug() << "Successfully verified removal of container with ID: Container2";
    }

    return app.exec();
}
