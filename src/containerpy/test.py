import unittest
import os
from ContainerPy import Package, Container, ContainerMap, ContainerSize, HaulerType


class TestPackage(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        self.pkg = Package("PKG123")

    def test_package_creation_by_id(self):
        """Test creating a package with an ID."""
        self.assertEqual(self.pkg.get_package_id(), "PKG123")

    def test_set_package_id(self):
        """Test setting a new package ID."""
        self.pkg.set_package_id("PKG124")
        self.assertEqual(self.pkg.get_package_id(), "PKG124")

    def test_package_creation_by_dict(self):
        """Test creating a package using a JSON-like dictionary."""
        pkg_dict = {"packageID": "PKG125"}
        pkg = Package(pkg_dict)
        self.assertEqual(pkg.get_package_id(), "PKG125")

    def test_to_json(self):
        """Test converting package to JSON-like dictionary."""
        expected = {"packageID": "PKG123"}
        self.assertEqual(self.pkg.to_json(), expected)


class TestContainer(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        self.container = Container("CNT001", ContainerSize.TwentyFT)

    def test_container_creation_by_id_and_size(self):
        """Test creating a container with ID and size."""
        self.assertEqual(self.container.get_container_id(), "CNT001")
        self.assertEqual(self.container.get_container_size(), ContainerSize.TwentyFT)
    
    # def test_add_and_retrieve_package(self):
    #     """Test adding a package to a container."""
    #     pkg = Package("PKG123")
    #     self.container.add_package(pkg)

    def test_custom_variables(self):
        """Test adding and retrieving custom variables."""
        self.container.add_custom_variable(HaulerType.Truck, "Temperature", "Cold")
        self.container.add_custom_variable(HaulerType.Truck, "LoadWeight", 100)
        self.assertEqual(
            self.container.get_custom_variable(HaulerType.Truck, "Temperature"), "Cold"
        )
        self.assertEqual(
            self.container.get_custom_variable(HaulerType.Truck, "LoadWeight"), "100"
        )

    def test_set_and_get_current_location(self):
        """Test setting and getting current location."""
        self.container.set_container_current_location("Warehouse A")
        self.assertEqual(self.container.get_container_current_location(), "Warehouse A")

    def test_set_and_get_next_destinations(self):
        """Test setting and retrieving next destinations."""
        self.container.set_container_next_destinations(["Port B", "Port C"])
        self.assertEqual(
            self.container.get_container_next_destinations(), ["Port B", "Port C"]
        )

    def test_set_and_get_movement_history(self):
        """Test setting and retrieving movement history."""
        self.container.set_container_movement_history(["Warehouse A", "In Transit"])
        self.assertEqual(
            self.container.get_container_movement_history(),
            ["Warehouse A", "In Transit"],
        )

    def test_set_added_time(self):
        """Test setting the added time."""
        self.container.set_container_added_time(100000)
        t = self.container.get_container_added_time()
        self.assertEqual(t, 100000)

    def test_to_json(self):
        """Test converting container to JSON-like dictionary."""
        expected = {
            "containerID": "CNT001",
            "containerMovementHistory": [],
            "containerNextDestinations": [],
            "addedTime": 100000,
            "containerSize": 0.0,
            "containerCurrentLocation": "Unknown",
            "customVariables": {},
            "packages": [],
        }
        self.assertDictEqual(self.container.to_json(), expected)


class TestContainerMap(unittest.TestCase):
    def setUp(self):
        """Set up test environment."""
        self.container_map = ContainerMap()

    def test_add_and_retrieve_container(self):
        """Test adding and retrieving a container by ID."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container, addingTime=1629488400.0)
        retrieved = self.container_map.get_container_by_id("CNT001")
        self.assertEqual(retrieved.get_container_id(), "CNT001")

    def test_add_multiple_containers(self):
        """Test adding multiple containers."""
        container1 = Container("CNT002", ContainerSize.FourtyFT)
        container2 = Container("CNT003", ContainerSize.FiftyThreeFT)
        self.container_map.add_containers([container1, container2], addingTime=1629488500.0)
        self.assertEqual(len(self.container_map.get_all_containers()), 2)

    def test_get_latest_containers(self):
        """Test retrieving the latest containers."""
        container1 = Container("CNT002", ContainerSize.FourtyFT)
        container2 = Container("CNT003", ContainerSize.FiftyThreeFT)
        self.container_map.add_containers([container1, container2], addingTime=1629488500.0)
        latest_containers = self.container_map.get_latest_containers()
        self.assertTrue(len(latest_containers) <= 200)

    def test_add_containers_from_dict(self):
        """Test adding containers using a JSON-like dictionary."""
        container_data = {
            "containers": [
                {
                    "containerID": "CNT004",
                    "containerSize": 1.0,
                    "containerCurrentLocation": "Port D",
                    "containerMovementHistory": ["Docked"],
                    "containerNextDestinations": ["Port E"],
                    "customVariables": {},
                    "packages": [],
                    "addedTime": 1629488600.0
                }
            ]
        }
        self.container_map.add_containers_from_dict(container_data)
        self.assertEqual(len(self.container_map.get_all_containers()), 1)

    def test_remove_container(self):
        """Test removing a container by ID."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container)
        self.container_map.remove_container_by_id("CNT001")
        self.assertEqual(len(self.container_map.get_all_containers()), 0)

    def test_get_containers_by_added_time(self):
        """Test retrieving containers by added time."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container, addingTime=1629488400.0)
        containers = self.container_map.get_containers_by_added_time(1629488500.0, "<")
        self.assertTrue(len(containers) > 0)

    def test_get_containers_by_next_destination(self):
        """Test retrieving containers by next destination."""
        container_data = {
            "containers": [
                {
                    "containerID": "CNT005",
                    "containerSize": 1.0,
                    "containerCurrentLocation": "Port D",
                    "containerMovementHistory": ["Docked"],
                    "containerNextDestinations": ["Port F"],
                    "customVariables": {},
                    "packages": [],
                    "addedTime": 1629488600.0
                }
            ]
        }
        self.container_map.add_containers_from_dict(container_data)
        containers = self.container_map.get_containers_by_next_destination("Port F")
        self.assertEqual(len(containers), 1)

class TestContainerMapWithDatabase(unittest.TestCase):
    DB_FILE = "/home/ahmed/Documents/test_container_map.sql"

    def setUp(self):
        """Set up test environment and initialize ContainerMap with a database."""
        # Ensure the previous test database is deleted to avoid conflicts
        if os.path.exists(self.DB_FILE):
            os.remove(self.DB_FILE)

        # Initialize ContainerMap with the database file
        self.container_map = ContainerMap(self.DB_FILE)

    def test_add_and_retrieve_container(self):
        """Test adding and retrieving a container by ID from the database."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container, addingTime=1629488400.0)
        retrieved = self.container_map.get_container_by_id("CNT001")
        self.assertIsNotNone(retrieved)
        self.assertEqual(retrieved.get_container_id(), "CNT001")

    def test_add_multiple_containers(self):
        """Test adding multiple containers to the database."""
        container1 = Container("CNT002", ContainerSize.FourtyFT)
        container2 = Container("CNT003", ContainerSize.FiftyThreeFT)
        self.container_map.add_containers([container1, container2], addingTime=1629488500.0)
        self.assertEqual(len(self.container_map.get_all_containers()), 2)

    def test_get_latest_containers(self):
        """Test retrieving the latest containers from the database."""
        container1 = Container("CNT002", ContainerSize.FourtyFT)
        container2 = Container("CNT003", ContainerSize.FiftyThreeFT)
        self.container_map.add_containers([container1, container2], addingTime=1629488500.0)
        latest_containers = self.container_map.get_latest_containers()
        self.assertTrue(len(latest_containers) <= 200)

    def test_add_containers_from_dict(self):
        """Test adding containers using a JSON-like dictionary to the database."""
        container_data = {
            "containers": [
                {
                    "containerID": "CNT004",
                    "containerSize": 1.0,
                    "containerCurrentLocation": "Port D",
                    "containerMovementHistory": ["Docked"],
                    "containerNextDestinations": ["Port E"],
                    "customVariables": {},
                    "packages": [],
                    "addedTime": 1629488600.0
                }
            ]
        }
        self.container_map.add_containers_from_dict(container_data)
        self.assertEqual(len(self.container_map.get_all_containers()), 1)

    def test_remove_container(self):
        """Test removing a container by ID from the database."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container)
        self.container_map.remove_container_by_id("CNT001")
        self.assertEqual(len(self.container_map.get_all_containers()), 0)

    def test_get_containers_by_added_time(self):
        """Test retrieving containers by added time from the database."""
        container = Container("CNT001", ContainerSize.TwentyFT)
        self.container_map.add_container(container, addingTime=1629488400.0)
        containers = self.container_map.get_containers_by_added_time(1629488500.0, "<")
        self.assertTrue(len(containers) > 0)

    def test_get_containers_by_next_destination(self):
        """Test retrieving containers by next destination from the database."""
        container_data = {
            "containers": [
                {
                    "containerID": "CNT005",
                    "containerSize": 1.0,
                    "containerCurrentLocation": "Port D",
                    "containerMovementHistory": ["Docked"],
                    "containerNextDestinations": ["Port F"],
                    "customVariables": {},
                    "packages": [],
                    "addedTime": 1629488600.0
                }
            ]
        }
        self.container_map.add_containers_from_dict(container_data)
        containers = self.container_map.get_containers_by_next_destination("Port F")
        self.assertEqual(len(containers), 1)

if __name__ == "__main__":
    unittest.main()
